#include "audiothread.h"

#include <QDebug>
#include <QFile>

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}

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

//void freep(void **ptr) {
//    free(*ptr);
//    *ptr = nullptr;
//}

void AudioThread::run() {
    // 文件名
    const char *inFilename = "F:/44100_s16le_2.pcm";
    QFile inFile(inFilename);

    const char *outFilename = "F:/48000_f32le_1.pcm";
    QFile outFile(outFilename);

    // 输入参数
    AVSampleFormat inSampleFmt = AV_SAMPLE_FMT_S16;
    int inSampleRate = 44100;
    int inChLayout = AV_CH_LAYOUT_STEREO;

    // 输入缓冲区
    // 指向缓冲区的指针
    uint8_t **inData = nullptr;
    // 缓冲区的大小
    int inLinesize = 0;
    // 声道数
    int inChs = av_get_channel_layout_nb_channels(inChLayout);
    // 一个样本的大小
    int inBytesPerSample = inChs * av_get_bytes_per_sample(inSampleFmt);
    // 缓冲区的样本数量
    int inSamples = 1024;
    // 读取文件数据的大小
    int len = 0;

    // 输出参数
    AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_FLT;
    int outSampleRate = 48000;
    int outChLayout = AV_CH_LAYOUT_MONO;

    // 输出缓冲区
    // 指向缓冲区的指针
    uint8_t **outData = nullptr;
    // 缓冲区的大小
    int outLinesize = 0;
    // 声道数
    int outChs = av_get_channel_layout_nb_channels(outChLayout);
    // 一个样本的大小
    int outBytesPerSample = outChs * av_get_bytes_per_sample(outSampleFmt);
    // 缓冲区的样本数量
    int outSamples = 1024;

    // 返回结果
    int ret = 0;

    // 创建重采样上下文
    SwrContext *ctx = swr_alloc_set_opts(nullptr,
                                         // 输出参数
                                         outChLayout, outSampleFmt, outSampleRate,
                                         // 输入参数
                                         inChLayout, inSampleFmt, inSampleRate,
                                         0, nullptr);
    if (!ctx) {
        qDebug() << "swr_alloc_set_opts error";
        goto end;
    }

    // 初始化重采样上下文
    ret = swr_init(ctx);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "swr_init error:" << errbuf;
        goto end;
    }

    // 创建输入缓冲区
    ret = av_samples_alloc_array_and_samples(
              &inData,
              &inLinesize,
              inChs,
              inSamples,
              inSampleFmt,
              1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "av_samples_alloc_array_and_samples error:" << errbuf;
        goto end;
    }

    // 创建输出缓冲区
    ret = av_samples_alloc_array_and_samples(
              &outData,
              &outLinesize,
              outChs,
              outSamples,
              outSampleFmt,
              1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "av_samples_alloc_array_and_samples error:" << errbuf;
        goto end;
    }

    // 打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error:" << inFilename;
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error:" << outFilename;
        goto end;
    }

    // 读取文件数据
    while ((len = inFile.read((char *) inData[0], inLinesize)) > 0) {
        // 读取的样本数量
        inSamples = len / inBytesPerSample;

        // 重采样(返回值转换后的样本数量)
        ret = swr_convert(ctx,
                          outData, outSamples,
                          (const uint8_t **) inData, inSamples
                         );

        if (ret < 0) {
            ERROR_BUF(ret);
            qDebug() << "swr_convert error:" << errbuf;
            goto end;
        }

        // 将转换后的数据写入到输出文件中
        outFile.write((char *) outData[0], ret * outBytesPerSample);
    }

end:
    // 释放资源
    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放输入缓冲区
    if (inData) {
        av_freep(&inData[0]);
    }
    av_freep(&inData);

    // 释放输出缓冲区
    if (outData) {
        av_freep(&outData[0]);
    }
    av_freep(&outData);

    // 释放重采样上下文
    swr_free(&ctx);

//    void *ptr = malloc(100);
//    freep(&ptr);
//    free(ptr);
//    ptr = nullptr;
}
