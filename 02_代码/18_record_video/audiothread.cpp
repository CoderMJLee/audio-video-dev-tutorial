#include "audiothread.h"

#include <QDebug>
#include <QFile>

extern "C" {
// 设备
#include <libavdevice/avdevice.h>
// 格式
#include <libavformat/avformat.h>
// 工具（比如错误处理）
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
}

#ifdef Q_OS_WIN
    // 格式名称
    #define FMT_NAME "dshow"
    // 设备名称
    #define DEVICE_NAME "video=Integrated Camera"
    // YUV文件名
    #define FILENAME "F:/out.yuv"
#else
    #define FMT_NAME "avfoundation"
    #define DEVICE_NAME "0"
    #define FILEPATH "/Users/mj/Desktop/out.yuv"
#endif

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

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

// 当线程启动的时候（start），就会自动调用run函数
// run函数中的代码是在子线程中执行的
// 耗时操作应该放在run函数中
void AudioThread::run() {
    qDebug() << this << "开始执行----------";

    // 获取输入格式对象
    AVInputFormat *fmt = av_find_input_format(FMT_NAME);
    if (!fmt) {
        qDebug() << "av_find_input_format error" << FMT_NAME;
        return;
    }

    // 格式上下文（将来可以利用上下文操作设备）
    AVFormatContext *ctx = nullptr;

    // 设备参数
    AVDictionary *options = nullptr;
    av_dict_set(&options, "video_size", "640x480", 0);
    av_dict_set(&options, "pixel_format", "yuyv422", 0);
    av_dict_set(&options, "framerate", "30", 0);

//    av_dict_set(&options, "video_size", "1280x720", 0);
//    av_dict_set(&options, "pixel_format", "yuyv422", 0);
//    av_dict_set(&options, "framerate", "10", 0);

    // 打开设备
    int ret = avformat_open_input(&ctx, DEVICE_NAME, fmt, &options);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avformat_open_input error" << errbuf;
        return;
    }

    // 文件名
    QFile file(FILENAME);

    // 打开文件
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "file open error" << FILENAME;

        // 关闭设备
        avformat_close_input(&ctx);
        return;
    }

    // 计算一帧的大小
    AVCodecParameters *params = ctx->streams[0]->codecpar;
    AVPixelFormat pixFmt = (AVPixelFormat) params->format;

    int imageSize = av_image_get_buffer_size(
                        pixFmt,
                        params->width,
                        params->height,
                        1);

//    qDebug() << imageSize;
//    qDebug() << pixFmt << params->width << params->height;
//    qDebug() << av_pix_fmt_desc_get(pixFmt)->name;
//    int pixSize = av_get_bits_per_pixel(av_pix_fmt_desc_get(pixFmt)) >> 3;
//    int imageSize = params->width * params->height * pixSize;

    // 数据包
    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        // 不断采集数据
        ret = av_read_frame(ctx, pkt);

        if (ret == 0) { // 读取成功
            // 将数据写入文件
            file.write((const char *) pkt->data, imageSize);

            // windows：614400
            // mac：615680
            // qDebug() << pkt->size;

            // 释放资源
            av_packet_unref(pkt);
        } else if (ret == AVERROR(EAGAIN)) { // 资源临时不可用
            continue;
        } else { // 其他错误
            ERROR_BUF(ret);
            qDebug() << "av_read_frame error" << errbuf << ret;
            break;
        }
    }

    // 释放资源
    av_packet_free(&pkt);

    // 关闭文件
    file.close();

    // 关闭设备
    avformat_close_input(&ctx);

    qDebug() << this << "正常结束----------";
}
