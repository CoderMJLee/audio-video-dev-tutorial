#include "audiothread.h"

#include <QDebug>
#include <QFile>
#include <QDateTime>
#include "ffmpegs.h"

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
    // showSpec(ctx);

    // 文件名
    QString filename = FILEPATH;
    filename += QDateTime::currentDateTime().toString("MM_dd_HH_mm_ss");
    filename += ".wav";
    QFile file(filename);

    // 打开文件
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败" << filename;

        // 关闭设备
        avformat_close_input(&ctx);
        return;
    }

    // 获取输入流
    AVStream *stream = ctx->streams[0];
    // 获取音频参数
    AVCodecParameters *params = stream->codecpar;

    // 写入WAV文件头
    WAVHeader header;
    header.sampleRate = params->sample_rate;
    // 2
    header.bitsPerSample = av_get_bits_per_sample(params->codec_id);
    header.numChannels = params->channels;
    if (params->codec_id >= AV_CODEC_ID_PCM_F32BE) {
        header.audioFormat = AUDIO_FORMAT_FLOAT;
    }
    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.sampleRate * header.blockAlign;
//    header.dataChunkDataSize = 0;
    file.write((char *) &header, sizeof (WAVHeader));

    // 数据包
    AVPacket *pkt = av_packet_alloc();
    while (!isInterruptionRequested()) {
        // 不断采集数据
        ret = av_read_frame(ctx, pkt);

        if (ret == 0) { // 读取成功
            // 将数据写入文件
            file.write((const char *) pkt->data, pkt->size);

            // 计算录音时长
            header.dataChunkDataSize += pkt->size;
            unsigned long long ms = 1000.0 * header.dataChunkDataSize / header.byteRate;
            emit timeChanged(ms);

            // 释放资源
            av_packet_unref(pkt);
        } else if (ret == AVERROR(EAGAIN)) { // 资源临时不可用
            continue;
        } else { // 其他错误
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof (errbuf));
            qDebug() << "av_read_frame error" << errbuf << ret;
            break;
        }
    }

//    qDebug() << file.size() << header.dataChunkDataSize;

//    int size = file.size();

    // 写入dataChunkDataSize
//    header.dataChunkDataSize = size - sizeof (WAVHeader);
    file.seek(sizeof (WAVHeader) - sizeof (header.dataChunkDataSize));
    file.write((char *) &header.dataChunkDataSize, sizeof (header.dataChunkDataSize));

    // 写入riffChunkDataSize
    header.riffChunkDataSize = file.size()
                               - sizeof (header.riffChunkId)
                               - sizeof (header.riffChunkDataSize);
    file.seek(sizeof (header.riffChunkId));
    file.write((char *) &header.riffChunkDataSize, sizeof (header.riffChunkDataSize));

    // 释放资源
    av_packet_free(&pkt);

    // 关闭文件
    file.close();

    // 关闭设备
    avformat_close_input(&ctx);

    qDebug() << this << "正常结束----------";
}

void AudioThread::setStop(bool stop) {
    _stop = stop;
}
