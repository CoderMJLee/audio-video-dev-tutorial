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

private:
    QFile _file;
    int _timerId = 0;
    State _state = Stopped;
    Yuv _yuv;
    bool _playing;
    QImage *_currentImage = nullptr;
    QRect _dstRect;

    void freeCurrentImage();

    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // YUVPLAYER_H
