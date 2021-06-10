#include "videoplayer.h"

#pragma mark - 构造、析构
VideoPlayer::VideoPlayer(QObject *parent) : QObject(parent) {

}

VideoPlayer::~VideoPlayer() {

}

#pragma mark - 公共方法
void VideoPlayer::play() {
    if (_state == Playing) return;
    // 状态可能是：暂停、停止、正常完毕

    // 解封装、解码、播放，音视频同步

    // 多线程的知识

    // 创建子线程去解码

    // 解码后的格式不一定是我们播放器想要的？
    // PCM格式不是SDL支持的 S16 44100
    // YUV -> RGB

    setState(Playing);
}

void VideoPlayer::pause() {
    if (_state != Playing) return;
    // 状态可能是：正在播放

    setState(Paused);
}

void VideoPlayer::stop() {
    if (_state == Stopped) return;
    // 状态可能是：正在播放、暂停、正常完毕

    setState(Stopped);
}

bool VideoPlayer::isPlaying() {
    return _state == Playing;
}

VideoPlayer::State VideoPlayer::getState() {
    return _state;
}

void VideoPlayer::setFilename(const char *filename) {
    _filename = filename;
}

#pragma mark - 私有方法
void VideoPlayer::setState(State state) {
    if (state == _state) return;

    _state = state;

    emit stateChanged(this);
}
