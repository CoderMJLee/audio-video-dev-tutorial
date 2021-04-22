#include "audiothread.h"

#include <QDebug>
#include "ffmpegs.h"

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

void AudioThread::run() {
    AudioDecodeSpec out;
    out.filename = "F:/out.pcm";

    FFmpegs::aacDecode("F:/in.aac", out);

    qDebug() << "采样率：" << out.sampleRate;
    qDebug() << "采样格式：" << av_get_sample_fmt_name(out.sampleFmt);
    qDebug() << "声道数：" << av_get_channel_layout_nb_channels(out.chLayout);
}
