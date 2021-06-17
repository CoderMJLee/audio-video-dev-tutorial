#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <list>
#include "condmutex.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#define ERROR_BUF \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

#define END(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        setState(Stopped); \
        emit playFailed(this); \
        goto end; \
    }

#define RET(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        return ret; \
    }

/**
 * 预处理视频数据（不负责显示、渲染视频）
 */
class VideoPlayer : public QObject {
    Q_OBJECT
public:
    // 状态
    typedef enum {
        Stopped = 0,
        Playing,
        Paused
    } State;

    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    /** 播放 */
    void play();
    /** 暂停 */
    void pause();
    /** 停止 */
    void stop();
    /** 是否正在播放中 */
    bool isPlaying();
    /** 获取当前的状态 */
    State getState();
    /** 设置文件名 */
    void setFilename(const char *filename);
    /** 获取总时长（单位是微秒，1秒=10^3毫秒=10^6微秒） */
    int64_t getDuration();

signals:
    void stateChanged(VideoPlayer *player);
    void initFinished(VideoPlayer *player);
    void playFailed(VideoPlayer *player);

private:
    /******** 音频相关 ********/
    /** 解码上下文 */
    AVCodecContext *_aDecodeCtx = nullptr;
    /** 流 */
    AVStream *_aStream = nullptr;
    /** 存放解码后的数据 */
    AVFrame *_aFrame = nullptr;
    /** 存放音频包的列表 */
    std::list<AVPacket> *_aPktList = nullptr;
    /** 音频包列表的锁 */
    CondMutex *_aMutex = nullptr;

    /** 初始化音频信息 */
    int initAudioInfo();
    /** 初始化SDL */
    int initSDL();
    /** 添加数据包到音频包列表中 */
    void addAudioPkt(AVPacket &pkt);
    /** 清空音频包列表 */
    void clearAudioPktList();
    /** SDL填充缓冲区的回调函数 */
    static void sdlAudioCallbackFunc(void *userdata, Uint8 *stream, int len);
    /** SDL填充缓冲区的回调函数 */
    void sdlAudioCallback(Uint8 *stream, int len);
    /** 音频解码 */
    int decodeAudio();

    /******** 视频相关 ********/
    /** 解码上下文 */
    AVCodecContext *_vDecodeCtx = nullptr;
    /** 流 */
    AVStream *_vStream = nullptr;
    /** 存放解码后的数据 */
    AVFrame *_vFrame = nullptr;
    /** 存放视频包的列表 */
    std::list<AVPacket> *_vPktList = nullptr;
    /** 视频包列表的锁 */
    CondMutex *_vMutex = nullptr;


    /** 初始化视频信息 */
    int initVideoInfo();
    /** 添加数据包到视频包列表中 */
    void addVideoPkt(AVPacket &pkt);
    /** 清空视频包列表 */
    void clearVideoPktList();


    /******** 其他 ********/
    /** 当前的状态 */
    State _state = Stopped;
    /** 文件名 */
    const char *_filename;
    /** 解封装上下文 */
    AVFormatContext *_fmtCtx = nullptr;

    /** 初始化解码器和解码上下文 */
    int initDecoder(AVCodecContext **decodeCtx,
                    AVStream **stream,
                    AVMediaType type);
    /** 改变状态 */
    void setState(State state);
    /** 读取文件数据 */
    void readFile();
};

#endif // VIDEOPLAYER_H
