#include "videoplayer.h"
#include <QDebug>

int VideoPlayer::initAudioInfo() {
    // 初始化解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStream, AVMEDIA_TYPE_AUDIO);
    RET(initDecoder);

    // 初始化frame
    _aFrame = av_frame_alloc();
    if (!_aFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // 初始化SDL
    ret = initSDL();
    RET(initSDL);

    return 0;
}

void VideoPlayer::sdlAudioCallbackFunc(void *userdata, Uint8 *stream, int len) {
    VideoPlayer *player = (VideoPlayer *) userdata;
    player->sdlAudioCallback(stream, len);
}

int VideoPlayer::initSDL() {
    // 音频参数
    SDL_AudioSpec spec;
    // 采样率
    spec.freq = 44100;
    // 采样格式（s16le）
    spec.format = AUDIO_S16LSB;
    // 声道数
    spec.channels = 2;
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

    // 开始播放
    SDL_PauseAudio(0);

    return 0;
}

void VideoPlayer::addAudioPkt(AVPacket &pkt) {
    _aMutex->lock();
    _aPktList->push_back(pkt);
    _aMutex->signal();
    _aMutex->unlock();
}

void VideoPlayer::clearAudioPktList() {
    _aMutex->lock();
    for (AVPacket &pkt : *_aPktList) {
        av_packet_unref(&pkt);
    }
    _aPktList->clear();
    _aMutex->unlock();
}

void VideoPlayer::sdlAudioCallback(Uint8 *stream, int len) {
    // len：SDL音频缓冲区剩余的大小（还未填充的大小）
    while (len > 0) {
        int dataSize = decodeAudio();
        qDebug() << dataSize;
        if (dataSize <= 0) {

        } else {

        }

//        // 将一个pkt包解码后的pcm数据填充到SDL的音频缓冲区
//        SDL_MixAudio(stream, src, srcLen, SDL_MIX_MAXVOLUME);

//        // 移动偏移量
//        len -= srcLen;
//        stream += srcLen;
    }
}

int VideoPlayer::decodeAudio() {
    // 解锁
    _aMutex->lock();

//    while (_aPktList->empty()) {
//        _aMutex->wait();
//    }
    if (_aPktList->empty()) {
        _aMutex->unlock();
        return 0;
    }

    // 取出头部的数据包
    AVPacket &pkt = _aPktList->front();
    // 从头部中删除
    _aPktList->pop_front();

    // 解锁
    _aMutex->unlock();

    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(_aDecodeCtx, &pkt);
    // 释放pkt
    av_packet_unref(&pkt);
    RET(avcodec_send_packet);

    // 获取解码后的数据
    ret = avcodec_receive_frame(_aDecodeCtx, _aFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return 0;
    } else RET(avcodec_receive_frame);

    // 将解码后的数据写入文件
    qDebug() << _aFrame->sample_rate
             << _aFrame->channels
             << av_get_sample_fmt_name((AVSampleFormat) _aFrame->format);

    // 由于解码出来的PCM。跟SDL要求的PCM格式可能不一致
    // 需要进行重采样


    return _aFrame->nb_samples
           * _aFrame->channels
           * av_get_bytes_per_sample((AVSampleFormat) _aFrame->format);
}
