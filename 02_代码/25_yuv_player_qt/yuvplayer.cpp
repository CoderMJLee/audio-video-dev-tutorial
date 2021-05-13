#include "yuvplayer.h"

#include <QDebug>
#include <QPainter>
#include "ffmpegs.h"

extern "C" {
#include <libavutil/imgutils.h>
}

YuvPlayer::YuvPlayer(QWidget *parent) : QWidget(parent) {
    // 设置背景色
    setAttribute(Qt::WA_StyledBackground);
//    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background: black");
}

YuvPlayer::~YuvPlayer() {
    _file.close();
    freeCurrentImage();
}

void YuvPlayer::play() {
    _timerId = startTimer(1000 / _yuv.fps);
    _state = YuvPlayer::Playing;
}

void YuvPlayer::pause() {
    if (_timerId) {
        killTimer(_timerId);
    }
    _state = YuvPlayer::Paused;
}

void YuvPlayer::stop() {
    if (_timerId) {
        killTimer(_timerId);
    }
    _state = YuvPlayer::Stopped;
}

bool YuvPlayer::isPlaying() {
    return _state == YuvPlayer::Playing;
}

YuvPlayer::State YuvPlayer::getState() {
    return _state;
}

void YuvPlayer::setYuv(Yuv &yuv) {
    _yuv = yuv;

    // 打开文件
    _file.setFileName(yuv.filename);
    if (!_file.open(QFile::ReadOnly)) {
        qDebug() << "file open error" << yuv.filename;
    }
}

// 当组件想重绘的时候，就会调用这个函数
// 想要绘制什么内容，在这个函数中实现
void YuvPlayer::paintEvent(QPaintEvent *event) {
    if (!_currentImage) return;

    qDebug() << "paintEvent";

    // 将图片绘制到当前组件上
    QPainter(this).drawImage(QRect(0, 0, width(), height()), *_currentImage);
//    QPainter(this).drawImage(QPoint(0, 0), *_currentImage);
}

void YuvPlayer::timerEvent(QTimerEvent *event) {
    // 图片大小
    int imgSize = av_image_get_buffer_size(_yuv.pixelFormat,
                                           _yuv.width,
                                           _yuv.height,
                                           1);
    char data[imgSize];
    if (_file.read(data, imgSize) > 0) {
        RawVideoFrame in = {
            data,
            _yuv.width, _yuv.height,
            _yuv.pixelFormat
        };
        RawVideoFrame out = {
            nullptr,
            _yuv.width, _yuv.height,
            AV_PIX_FMT_RGB24
        };
        FFmpegs::convertRawVideo(in, out);

        freeCurrentImage();
        _currentImage = new QImage((uchar *) out.pixels,
                                   out.width, out.height, QImage::Format_RGB888);

        // 刷新
        update();
    } else {
        // 文件数据已经读取完毕
        killTimer(_timerId);
    }
}

void YuvPlayer::freeCurrentImage() {
    if (!_currentImage) return;
    free(_currentImage->bits());
    delete _currentImage;
    _currentImage = nullptr;
}
