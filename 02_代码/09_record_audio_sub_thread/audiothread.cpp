#include "audiothread.h"

#include <QDebug>
#include <QFile>
#include <QDateTime>

extern "C" {
// 设备
#include <libavdevice/avdevice.h>
// 格式
#include <libavformat/avformat.h>
// 工具（比如错误处理）
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}

#ifdef Q_OS_WIN
    // 格式名称
    #define FMT_NAME "dshow"
    // 设备名称
    #define DEVICE_NAME "audio=线路输入 (3- 魅声T800)"
    // PCM文件名
    #define FILEPATH "F:/"
#else
    #define FMT_NAME "avfoundation"
    #define DEVICE_NAME ":0"
    #define FILEPATH "/Users/mj/Desktop/"
#endif

AudioThread::AudioThread(QObject *parent) : QThread(parent) {
    // 当监听到线程结束时（finished），就调用deleteLater回收内存
    connect(this, &AudioThread::finished,
            this, &AudioThread::deleteLater);
}

AudioThread::~AudioThread() {
    // 断开所有的连接
    disconnect();
    // 内存回收之前，正常结束线程
    requestInterruption();
    // 安全退出
    quit();
    wait();
    qDebug() << this << "析构（内存被回收）";
}

void showSpec(AVFormatContext *ctx) {
    // 获取输入流
    AVStream *stream = ctx->streams[0];
    // 获取音频参数
    AVCodecParameters *params = stream->codecpar;
    // 声道数
    qDebug() << params->channels;
    // 采样率
    qDebug() << params->sample_rate;
    // 采样格式
    qDebug() << params->format;
    // 每一个样本的一个声道占用多少个字节
    qDebug() << av_get_bytes_per_sample((AVSampleFormat) params->format);
}

// 当线程启动的时候（start），就会自动调用run函数
// run函数中的代码是在子线程中执行的
// 耗时操作应该放在run函数中
void AudioThread::run() {
    qDebug() << this << "开始执行----------";

    // 获取输入格式对象
    AVInputFormat *fmt = av_find_input_format(FMT_NAME);
    if (!fmt) {
        qDebug() << "获取输入格式对象失败" << FMT_NAME;
        return;
    }

    // 格式上下文（将来可以利用上下文操作设备）
    AVFormatContext *ctx = nullptr;
    // 打开设备
    int ret = avformat_open_input(&ctx, DEVICE_NAME, fmt, nullptr);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        qDebug() << "打开设备失败" << errbuf;
        return;
    }

    // 打印一下录音设备的参数信息
    showSpec(ctx);

    // 文件名
    QString filename = FILEPATH;

    filename += QDateTime::currentDateTime().toString("MM_dd_HH_mm_ss");
    filename += ".pcm";
    QFile file(filename);

    // 打开文件
    // WriteOnly：只写模式。如果文件不存在，就创建文件；如果文件存在，就会清空文件内容
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败" << filename;

        // 关闭设备
        avformat_close_input(&ctx);
        return;
    }

    // 数据包
//    AVPacket pkt;
    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        // 不断采集数据
//        ret = av_read_frame(ctx, &pkt);
        ret = av_read_frame(ctx, pkt);

        if (ret == 0) { // 读取成功
            // 将数据写入文件
//            file.write((const char *) pkt.data, pkt.size);

            file.write((const char *) pkt->data, pkt->size);
        } else if (ret == AVERROR(EAGAIN)) { // 资源临时不可用
            continue;
        } else { // 其他错误
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof (errbuf));
            qDebug() << "av_read_frame error" << errbuf << ret;
            break;
        }

        // 必须要加，释放pkt内部的资源
//        av_packet_unref(&pkt);
        av_packet_unref(pkt);
    }
//    while (!_stop && av_read_frame(ctx, &pkt) == 0) {
//        // 将数据写入文件
//        file.write((const char *) pkt.data, pkt.size);
//    }

    // 释放资源
    // 关闭文件
    file.close();

    // 释放资源
    av_packet_free(&pkt);

    // 关闭设备
    avformat_close_input(&ctx);

    qDebug() << this << "正常结束----------";
}

void AudioThread::setStop(bool stop) {
    _stop = stop;
}
