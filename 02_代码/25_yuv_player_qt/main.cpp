#include "mainwindow.h"

#include <QApplication>
//#include "ffmpegs.h"

#undef main

int main(int argc, char *argv[]) {
//    RawVideoFile in = {
//        "F:/res/dragon_ball.yuv",
//        640, 480, AV_PIX_FMT_YUV420P
//    };
//    RawVideoFile out = {
//        "F:/res/dragon_ball.rgb",
//        600, 600, AV_PIX_FMT_NV42
//    };
//    FFmpegs::convertRawVideo(in, out);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
