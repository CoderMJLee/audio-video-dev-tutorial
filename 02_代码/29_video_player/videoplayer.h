#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <list>
#include "condmutex.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
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
        free(); \
        return; \
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

    // 音量
    typedef enum {
        Min = 0,
        Max = 100
    } Volumn;

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
    /** 设置音量 */
    void setVolumn(int volumn);
    int getVolumn();
    /** 设置静音 */
    void setMute(bool mute);
    bool isMute();

signals:
    void stateChanged(VideoPlayer *player);
    void initFinished(VideoPlayer *player);
    void playFailed(VideoPlayer *player);

private:
    /******** 音频相关 ********/
    typedef struct {
        int sampleRate;
        AVSampleFormat sampleFmt;
        int chLayout;
        int chs;
        int bytesPerSampleFrame;
    } AudioSwrSpec;

    /** 解码上下文 */
    AVCodecContext *_aDecodeCtx = nullptr;
    /** 流 */
    AVStream *_aStream = nullptr;
    /** 存放音频包的列表 */
    std::list<AVPacket> _aPktList;
    /** 音频包列表的锁 */
    CondMutex _aMutex;
    /** 音频重采样上下文 */
    SwrContext *_aSwrCtx = nullptr;
    /** 音频重采样输入\输出参数 */
    AudioSwrSpec _aSwrInSpec, _aSwrOutSpec;
    /** 音频重采样输入\输出frame */
    AVFrame *_aSwrInFrame = nullptr, *_aSwrOutFrame = nullptr;
    /** 音频重采样输出PCM的索引（从哪个位置开始取出PCM数据填充到SDL的音频缓冲区） */
    int _aSwrOutIdx = 0;
    /** 音频重采样输出PCM的大小 */
    int _aSwrOutSize = 0;
    /** 音量 */
    int _volumn = Max;
    /** 静音 */
    bool _mute = false;

    /** 初始化音频信息 */
    int initAudioInfo();
    /** 初始化音频重采样 */
    int initSwr();
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
    std::list<AVPacket> _vPktList;
    /** 视频包列表的锁 */
    CondMutex _vMutex;


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
    /** 释放资源 */
    void free();
    void freeAudio();
    void freeVideo();
};

#endif // VIDEOPLAYER_H
