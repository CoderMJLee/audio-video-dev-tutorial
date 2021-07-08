#include "videoplayer.h"
#include <QDebug>

int VideoPlayer::initAudioInfo() {
    // 初始化解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStream, AVMEDIA_TYPE_AUDIO);
    RET(initDecoder);

    // 初始化音频重采样
    ret = initSwr();
    RET(initSwr);

    // 初始化SDL
    ret = initSDL();
    RET(initSDL);

    return 0;
}

int VideoPlayer::initSwr() {
    // 重采样输入参数
    _aSwrInSpec.sampleFmt = _aDecodeCtx->sample_fmt;
    _aSwrInSpec.sampleRate = _aDecodeCtx->sample_rate;
    _aSwrInSpec.chLayout = _aDecodeCtx->channel_layout;
    _aSwrInSpec.chs = _aDecodeCtx->channels;

    // 重采样输出参数
    _aSwrOutSpec.sampleFmt = AV_SAMPLE_FMT_S16;
    _aSwrOutSpec.sampleRate = 44100;
    _aSwrOutSpec.chLayout = AV_CH_LAYOUT_STEREO;
    _aSwrOutSpec.chs = av_get_channel_layout_nb_channels(_aSwrOutSpec.chLayout);
    _aSwrOutSpec.bytesPerSampleFrame = _aSwrOutSpec.chs
                                       * av_get_bytes_per_sample(_aSwrOutSpec.sampleFmt);

    // 创建重采样上下文
    _aSwrCtx = swr_alloc_set_opts(nullptr,
                                  // 输出参数
                                  _aSwrOutSpec.chLayout,
                                  _aSwrOutSpec.sampleFmt,
                                  _aSwrOutSpec.sampleRate,
                                  // 输入参数
                                  _aSwrInSpec.chLayout,
                                  _aSwrInSpec.sampleFmt,
                                  _aSwrInSpec.sampleRate,
                                  0, nullptr);
    if (!_aSwrCtx) {
        qDebug() << "swr_alloc_set_opts error";
        return -1;
    }

    // 初始化重采样上下文
    int ret = swr_init(_aSwrCtx);
    RET(swr_init);

    // 初始化重采样的输入frame
    _aSwrInFrame = av_frame_alloc();
    if (!_aSwrInFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // 初始化重采样的输出frame
    _aSwrOutFrame = av_frame_alloc();
    if (!_aSwrOutFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // _aSwrOutFrame的data[0]指向的内存空间
    ret = av_samples_alloc(_aSwrOutFrame->data,
                           _aSwrOutFrame->linesize,
                           _aSwrOutSpec.chs,
                           4096, _aSwrOutSpec.sampleFmt, 1);
    RET(av_samples_alloc);

    return 0;
}

int VideoPlayer::initSDL() {
    // 音频参数
    SDL_AudioSpec spec;
    // 采样率
    spec.freq = _aSwrOutSpec.sampleRate;
    // 采样格式（s16le）
    spec.format = AUDIO_S16LSB;
    // 声道数
    spec.channels = _aSwrOutSpec.chs;
    // 音频缓冲区的样本数量（这个值必须是2的幂）
    spec.samples = 512;
    // 回调
    spec.callback = sdlAudioCallbackFunc;
    // 传递给回调的参数
    spec.userdata = this;

    // 打开音频设备
    if (SDL_OpenAudio(&spec, nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();
        return -1;
    }

    return 0;
}

// 引用：披着对象外衣的指针
void VideoPlayer::addAudioPkt(AVPacket &pkt) {
    _aMutex.lock();
    _aPktList.push_back(pkt);
    _aMutex.signal();
    _aMutex.unlock();
}

void VideoPlayer::clearAudioPktList() {
    _aMutex.lock();
    for (AVPacket &pkt : _aPktList) {
        av_packet_unref(&pkt);
    }
    _aPktList.clear();
    _aMutex.unlock();
}

void VideoPlayer::freeAudio() {
    _aTime = 0;
    _aSwrOutIdx = 0;
    _aSwrOutSize = 0;
    _aStream = nullptr;
    _aCanFree = false;
    _aSeekTime = -1;

    clearAudioPktList();
    avcodec_free_context(&_aDecodeCtx);
    swr_free(&_aSwrCtx);
    av_frame_free(&_aSwrInFrame);
    if (_aSwrOutFrame) {
        av_freep(&_aSwrOutFrame->data[0]);
        av_frame_free(&_aSwrOutFrame);
    }

    // 停止播放
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}

void VideoPlayer::sdlAudioCallbackFunc(void *userdata, Uint8 *stream, int len) {
    VideoPlayer *player = (VideoPlayer *) userdata;
    player->sdlAudioCallback(stream, len);
}

void VideoPlayer::sdlAudioCallback(Uint8 *stream, int len) {
    // 清零（静音）
    SDL_memset(stream, 0, len);

    // len：SDL音频缓冲区剩余的大小（还未填充的大小）
    while (len > 0) {
        if (_state == Paused) break;
        if (_state == Stopped) {
            _aCanFree = true;
            break;
        }

        // 说明当前PCM的数据已经全部拷贝到SDL的音频缓冲区了
        // 需要解码下一个pkt，获取新的PCM数据
        if (_aSwrOutIdx >= _aSwrOutSize) {
            // 全新PCM的大小
            _aSwrOutSize = decodeAudio();
            // 索引清0
            _aSwrOutIdx = 0;
            // 没有解码出PCM数据，那就静音处理
            if (_aSwrOutSize <= 0) {
                // 假定PCM的大小
                _aSwrOutSize = 1024;
                // 给PCM填充0（静音）
                memset(_aSwrOutFrame->data[0], 0, _aSwrOutSize);
            }
        }

        // 本次需要填充到stream中的PCM数据大小
        int fillLen = _aSwrOutSize - _aSwrOutIdx;
        fillLen = std::min(fillLen, len);

        // 获取当前音量
        int volumn = _mute ? 0 : ((_volumn * 1.0 / Max) * SDL_MIX_MAXVOLUME);

        // 填充SDL缓冲区
        SDL_MixAudio(stream,
                     _aSwrOutFrame->data[0] + _aSwrOutIdx,
                     fillLen, volumn);

        // 移动偏移量
        len -= fillLen;
        stream += fillLen;
        _aSwrOutIdx += fillLen;
    }
}

int VideoPlayer::decodeAudio() {
    // 加锁
    _aMutex.lock();

//    while (_aPktList.empty()) {
//        _aMutex.wait();
//    }
    if (_aPktList.empty()) {
        _aMutex.unlock();
        return 0;
    }

    // 取出头部的数据包
    AVPacket pkt = _aPktList.front();
    // 从头部中删除
    _aPktList.pop_front();
    // 解锁
    _aMutex.unlock();

    // 保存音频时钟
    if (pkt.pts != AV_NOPTS_VALUE) {
        _aTime = av_q2d(_aStream->time_base) * pkt.pts;
        // 通知外界：播放时间点发生了改变
        emit timeChanged(this);
    }

    // 如果是视频，不能在这个位置判断（不能提前释放pkt，不然会导致B帧、P帧解码失败，画面撕裂）
    // 发现音频的时间是早于seekTime的，直接丢弃
    if (_aSeekTime >= 0) {
        if (_aTime < _aSeekTime) {
            // 释放pkt
            av_packet_unref(&pkt);
            return 0;
        } else {
            _aSeekTime = -1;
        }
    }

    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(_aDecodeCtx, &pkt);
    // 释放pkt
    av_packet_unref(&pkt);
    RET(avcodec_send_packet);

    // 获取解码后的数据
    ret = avcodec_receive_frame(_aDecodeCtx, _aSwrInFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return 0;
    } else RET(avcodec_receive_frame);

    // 重采样输出的样本数
    int outSamples = av_rescale_rnd(_aSwrOutSpec.sampleRate,
                                    _aSwrInFrame->nb_samples,
                                    _aSwrInSpec.sampleRate, AV_ROUND_UP);

    // 由于解码出来的PCM。跟SDL要求的PCM格式可能不一致
    // 需要进行重采样
    ret = swr_convert(_aSwrCtx,
                      _aSwrOutFrame->data,
                      outSamples,
                      (const uint8_t **) _aSwrInFrame->data,
                      _aSwrInFrame->nb_samples);
    RET(swr_convert);

    return ret * _aSwrOutSpec.bytesPerSampleFrame;
}
