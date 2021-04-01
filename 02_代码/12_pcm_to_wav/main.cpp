#include "mainwindow.h"

#include <QApplication>
#include "ffmpegs.h"

extern "C" {
// 设备
#include <libavdevice/avdevice.h>
}

int main(int argc, char *argv[]) {
//    WAVHeader header;
//    header.sampleRate = 44100;
//    header.bitsPerSample = 32;
//    header.numChannels = 1;
//    header.audioFormat = 3;
//    // pcm转wav文件
//    FFmpegs::pcm2wav(header, "F:/44100_f32le_1.pcm", "F:/44100_f32le_1.wav");

    // 注册设备
    avdevice_register_all();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
