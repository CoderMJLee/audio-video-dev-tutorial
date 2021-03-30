#include "mainwindow.h"

#include <QApplication>
#include "ffmpegs.h"

extern "C" {
// 设备
#include <libavdevice/avdevice.h>
}

int main(int argc, char *argv[]) {
    WAVHeader header;
    header.riffChunkDataSize = 1767996;
    header.sampleRate = 44100;
    header.bitsPerSample = 16;
    header.numChannels = 2;
    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.sampleRate * header.blockAlign;
    header.dataChunkDataSize = 1767960;
    // pcm转wav文件
    FFmpegs::pcm2wav(header, "F:/in.pcm", "F:/in.wav");

    // 注册设备
    avdevice_register_all();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
