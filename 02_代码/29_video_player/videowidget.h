#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QImage>
#include "videoplayer.h"

/**
 * 显示（渲染）视频
 */
class VideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

public slots:
    void onPlayerFrameDecoded(VideoPlayer *player,
                              uint8_t *data,
                              VideoPlayer::VideoSwsSpec &spec);
private:
    QImage *_image = nullptr;
    QRect _rect;
    void paintEvent(QPaintEvent *event) override;

signals:

};

#endif // VIDEOWIDGET_H
