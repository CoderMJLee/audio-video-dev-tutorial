#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QWidget>
#include <QFile>

extern "C" {
#include <libavutil/avutil.h>
}

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pixelFormat;
    int fps;
} Yuv;

class YuvPlayer : public QWidget {
    Q_OBJECT
public:
    // 状态
    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
        Finished
    } State;

    explicit YuvPlayer(QWidget *parent = nullptr);
    ~YuvPlayer();

    void play();
    void pause();
    void stop();
    bool isPlaying();

    void setYuv(Yuv &yuv);

    State getState();
signals:
    void stateChanged();

private:
    int _timerId = 0;
    State _state = Stopped;
    Yuv _yuv;
    /** 一帧图片的大小 */
    int _imgSize;
    /** 刷帧的时间间隔 */
    int _interval;
    bool _playing;
    QFile *_file = nullptr;
    QImage *_currentImage = nullptr;
    QRect _dstRect;

    /** 改变状态 */
    void setState(State state);

    /** 关闭文件 */
    void closeFile();

    /** 释放图片 */
    void freeCurrentImage();

    /** 停止定时器 */
    void stopTimer();

    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // YUVPLAYER_H
