#include "videoplayer.h"
#include <QDebug>
#include <thread>

int VideoPlayer::initVideoInfo() {
    // 初始化解码器
    int ret = initDecoder(&_vDecodeCtx, &_vStream, AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);

    // 初始化视频像素格式转换
    ret = initSws();
    RET(initSws);

    // 开启新的线程去解码视频数据
    std::thread([this]() {
        decodeVideo();
    }).detach();

    return 0;
}

int VideoPlayer::initSws() {
    // 初始化视频像素格式转换的上下文
//    _vSwsCtx = sws_getContext();

    // 初始化视频像素格式转换的输入frame
    _vSwsInFrame = av_frame_alloc();
    if (!_vSwsInFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // 初始化视频像素格式转换的输出frame
    _vSwsOutFrame = av_frame_alloc();
    if (!_vSwsOutFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    return 0;
}

void VideoPlayer::addVideoPkt(AVPacket &pkt) {
    _vMutex.lock();
    _vPktList.push_back(pkt);
    _vMutex.signal();
    _vMutex.unlock();
}

void VideoPlayer::clearVideoPktList() {
    _vMutex.lock();
    for (AVPacket &pkt : _vPktList) {
        av_packet_unref(&pkt);
    }
    _vPktList.clear();
    _vMutex.unlock();
}

void VideoPlayer::freeVideo() {

}

void VideoPlayer::decodeVideo() {
    while (true) {
        _vMutex.lock();

        if (_vPktList.empty()) {
            _vMutex.unlock();
            continue;
        }

        // 取出头部的视频包
        AVPacket pkt = _vPktList.front();
        _vPktList.pop_front();
        _vMutex.unlock();

        // 发送压缩数据到解码器
        int ret = avcodec_send_packet(_vDecodeCtx, &pkt);
        // 释放pkt
        av_packet_unref(&pkt);
        CONTINUE(avcodec_send_packet);

        while (true) {
            // 获取解码后的数据
            ret = avcodec_receive_frame(_vDecodeCtx, _vSwsInFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                continue;
            } else CONTINUE(avcodec_receive_frame);

            // 像素格式的转换
            // _vSwsInFrame(yuv420p) -> _vSwsOutFrame(rgb24)
        }
    }
}
