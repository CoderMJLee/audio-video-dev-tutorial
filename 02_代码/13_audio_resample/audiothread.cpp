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

//void freep(void **ptr) {
//    free(*ptr);
//    *ptr = nullptr;
//}

void AudioThread::run() {
    // 44100_s16le_2 -> 48000_f32le_2 -> 48000_s32le_1 -> 44100_s16le_2

    ResampleAudioSpec ras1;
    ras1.filename = "F:/44100_s16le_2.pcm";
    ras1.sampleFmt = AV_SAMPLE_FMT_S16;
    ras1.sampleRate = 44100;
    ras1.chLayout = AV_CH_LAYOUT_STEREO;

    ResampleAudioSpec ras2;
    ras2.filename = "F:/48000_f32le_1.pcm";
    ras2.sampleFmt = AV_SAMPLE_FMT_FLT;
    ras2.sampleRate = 48000;
    ras2.chLayout = AV_CH_LAYOUT_MONO;

    ResampleAudioSpec ras3;
    ras3.filename = "F:/48000_s32le_1.pcm";
    ras3.sampleFmt = AV_SAMPLE_FMT_S32;
    ras3.sampleRate = 48000;
    ras3.chLayout = AV_CH_LAYOUT_MONO;

    ResampleAudioSpec ras4 = ras1;
    ras4.filename = "F:/44100_s16le_2_new.pcm";

    FFmpegs::resampleAudio(ras1, ras2);
    FFmpegs::resampleAudio(ras2, ras3);
    FFmpegs::resampleAudio(ras3, ras4);
}
