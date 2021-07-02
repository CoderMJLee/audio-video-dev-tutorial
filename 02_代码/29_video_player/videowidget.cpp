#include "videowidget.h"
#include <QDebug>
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    // 设置背景色
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background: black");
}

VideoWidget::~VideoWidget() {
    freeImage();
}

void VideoWidget::paintEvent(QPaintEvent *event) {
    if (!_image) return;

    // 将图片绘制到当前组件上
    QPainter(this).drawImage(_rect, *_image);
}

void VideoWidget::onPlayerStateChanged(VideoPlayer *player) {
    if (player->getState() != VideoPlayer::Stopped) return;

    freeImage();
    update();
}

void VideoWidget::onPlayerFrameDecoded(VideoPlayer *player,
                                       uint8_t *data,
                                       VideoPlayer::VideoSwsSpec &spec) {
    if (player->getState() == VideoPlayer::Stopped) return;

    // 释放之前的图片
    freeImage();

    // 创建新的图片
    if (data != nullptr) {
        _image = new QImage(data,
                            spec.width, spec.height,
                            QImage::Format_RGB888);

        // 计算最终的尺寸
        // 组件的尺寸
        int w = width();
        int h = height();

        // 计算rect
        int dx = 0;
        int dy = 0;
        int dw = spec.width;
        int dh = spec.height;

        // 计算目标尺寸
        if (dw > w || dh > h) { // 缩放
            if (dw * h > w * dh) { // 视频的宽高比 > 播放器的宽高比
                dh = w * dh / dw;
                dw = w;
            } else {
                dw = h * dw / dh;
                dh = h;
            }
        }

        // 居中
        dx = (w - dw) >> 1;
        dy = (h - dh) >> 1;

        _rect = QRect(dx, dy, dw, dh);
    }

    update();
}

void VideoWidget::freeImage() {
    if (_image) {
        av_free(_image->bits());
        delete _image;
        _image = nullptr;
    }
}
