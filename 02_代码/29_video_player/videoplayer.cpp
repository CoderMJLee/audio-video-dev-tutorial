#include "videoplayer.h"
#include <thread>
#include <QDebug>

#define AUDIO_MAX_PKT_SIZE 1000
#define VIDEO_MAX_PKT_SIZE 500

#pragma mark - 构造、析构
VideoPlayer::VideoPlayer(QObject *parent) : QObject(parent) {
    // 初始化Audio子系统
    if (SDL_Init(SDL_INIT_AUDIO)) {
        // 返回值不是0，就代表失败
        qDebug() << "SDL_Init error" << SDL_GetError();
        emit playFailed(this);
        return;
    }
}

VideoPlayer::~VideoPlayer() {
    // 不再对外发送消息
    disconnect();

    stop();

    SDL_Quit();
}

#pragma mark - 公共方法
void VideoPlayer::play() {
    if (_state == Playing) return;
    // 状态可能是：暂停、停止、正常完毕

    if (_state == Stopped) {
        // 开始线程：读取文件
        std::thread([this]() {
            readFile();
        }).detach();
    } else {
        // 改变状态
        setState(Playing);
    }
}

void VideoPlayer::pause() {
    if (_state != Playing) return;
    // 状态可能是：正在播放

    // 改变状态
    setState(Paused);
}

void VideoPlayer::stop() {
    if (_state == Stopped) return;
    // 状态可能是：正在播放、暂停、正常完毕

    // 改变状态
//    setState(Stopped);
    _state = Stopped;

    // 释放资源
    free();

    // 通知外界
    emit stateChanged(this);

//    std::thread([this]() {
//        SDL_Delay(100);
//        free();
//    }).detach();
}

bool VideoPlayer::isPlaying() {
    return _state == Playing;
}

VideoPlayer::State VideoPlayer::getState() {
    return _state;
}

void VideoPlayer::setFilename(QString &filename) {
//    const char *name = filename.toUtf8().data();
    const char *name = filename.toStdString().c_str();
    memcpy(_filename, name, strlen(name) + 1);
}

int VideoPlayer::getDuration() {
    return _fmtCtx ? round(_fmtCtx->duration * av_q2d(AV_TIME_BASE_Q)) : 0;
    // return _fmtCtx ? round(_fmtCtx->duration / 1000000.0) : 0;
}

int VideoPlayer::getTime() {
    return round(_aTime);
}

void VideoPlayer::setTime(int seekTime) {
    _seekTime = seekTime;
}

void VideoPlayer::setVolumn(int volumn) {
    _volumn = volumn;
}

int VideoPlayer::getVolumn() {
    return _volumn;
}

void VideoPlayer::setMute(bool mute) {
    _mute = mute;
}

bool VideoPlayer::isMute() {
    return _mute;
}

#pragma mark - 私有方法
void VideoPlayer::readFile() {
    // 返回结果
    int ret = 0;

    // 创建解封装上下文、打开文件
    ret = avformat_open_input(&_fmtCtx, _filename, nullptr, nullptr);
    END(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    END(avformat_find_stream_info);

    // 打印流信息到控制台
    av_dump_format(_fmtCtx, 0, _filename, 0);
    fflush(stderr);

    // 初始化音频信息
    _hasAudio = initAudioInfo() >= 0;
    // 初始化视频信息
    _hasVideo = initVideoInfo() >= 0;
    if (!_hasAudio && !_hasVideo) {
        fataError();
        return;
    }
//    // 初始化音频信息
//    initAudioInfo();
//    // 初始化视频信息
//    initVideoInfo();
//    if (!_aStream && !_vStream) {
//        fataError();
//        return;
//    }

    // 到此为止，初始化完毕
    emit initFinished(this);

    // 改变状态
    setState(Playing);

    // 音频解码子线程：开始工作
    SDL_PauseAudio(0);

    // 视频解码子线程：开始工作
    std::thread([this]() {
        decodeVideo();
    }).detach();

    // 从输入文件中读取数据
    AVPacket pkt;
    while (_state != Stopped) {
        // 处理seek操作
        if (_seekTime >= 0) {
            int streamIdx;
            if (_hasAudio) { // 优先使用音频流索引
                streamIdx = _aStream->index;
            } else {
                streamIdx = _vStream->index;
            }

            // 现实时间 -> 时间戳
            AVRational timeBase = _fmtCtx->streams[streamIdx]->time_base;
            int64_t ts = _seekTime / av_q2d(timeBase);
            ret = av_seek_frame(_fmtCtx, streamIdx, ts, AVSEEK_FLAG_BACKWARD);

            if (ret < 0) { // seek失败
                qDebug() << "seek失败" << _seekTime << ts << streamIdx;
                _seekTime = -1;
            } else {
                qDebug() << "seek成功" << _seekTime << ts << streamIdx;
                _vSeekTime = _seekTime;
                _aSeekTime = _seekTime;
                _seekTime = -1;
                // 恢复时钟
                _aTime = 0;
                _vTime = 0;
                // 清空之前读取的数据包
                clearAudioPktList();
                clearVideoPktList();
            }
        }

        int vSize = _vPktList.size();
        int aSize = _aPktList.size();

        if (vSize >= VIDEO_MAX_PKT_SIZE ||
                aSize >= AUDIO_MAX_PKT_SIZE) {
//            SDL_Delay(10);
            continue;
        }

        ret = av_read_frame(_fmtCtx, &pkt);
        if (ret == 0) {
            if (pkt.stream_index == _aStream->index) { // 读取到的是音频数据
                addAudioPkt(pkt);
            } else if (pkt.stream_index == _vStream->index) { // 读取到的是视频数据
                addVideoPkt(pkt);
            } else { // 如果不是音频、视频流，直接释放
                av_packet_unref(&pkt);
            }
        } else if (ret == AVERROR_EOF) { // 读到了文件的尾部
            if (vSize == 0 && aSize == 0) {
                // 说明文件正常播放完毕
                _fmtCtxCanFree = true;
                break;
            }
        } else {
            ERROR_BUF;
            qDebug() << "av_read_frame error" << errbuf;
            continue;
        }
    }
    if (_fmtCtxCanFree) { // 文件正常播放完毕
        stop();
    } else {
        // 标记一下：_fmtCtx可以释放了
        _fmtCtxCanFree = true;
    }
}

// 初始化解码器
int VideoPlayer::initDecoder(AVCodecContext **decodeCtx,
                             AVStream **stream,
                             AVMediaType type) {
    // 根据type寻找最合适的流信息
    // 返回值是流索引
    int ret = av_find_best_stream(_fmtCtx, type, -1, -1, nullptr, 0);
    RET(av_find_best_stream);

    // 检验流
    int streamIdx = ret;
    *stream = _fmtCtx->streams[streamIdx];
    if (!*stream) {
        qDebug() << "stream is empty";
        return -1;
    }

    // 为当前流找到合适的解码器
    AVCodec *decoder = avcodec_find_decoder((*stream)->codecpar->codec_id);
    if (!decoder) {
        qDebug() << "decoder not found" << (*stream)->codecpar->codec_id;
        return -1;
    }

    // 初始化解码上下文
    *decodeCtx = avcodec_alloc_context3(decoder);
    if (!decodeCtx) {
        qDebug() << "avcodec_alloc_context3 error";
        return -1;
    }

    // 从流中拷贝参数到解码上下文中
    ret = avcodec_parameters_to_context(*decodeCtx, (*stream)->codecpar);
    RET(avcodec_parameters_to_context);

    // 打开解码器
    ret = avcodec_open2(*decodeCtx, decoder, nullptr);
    RET(avcodec_open2);

    return 0;
}

void VideoPlayer::setState(State state) {
    if (state == _state) return;

    _state = state;

    emit stateChanged(this);
}

void VideoPlayer::free() {
    while (_hasAudio && !_aCanFree);
    while (_hasVideo && !_vCanFree);
    while (!_fmtCtxCanFree);
    avformat_close_input(&_fmtCtx);
    _fmtCtxCanFree = false;
    _seekTime = -1;

    freeAudio();
    freeVideo();
}

void VideoPlayer::fataError() {
    // 配合stop能够调用成功
    _state = Playing;
    stop();
    emit playFailed(this);
}

