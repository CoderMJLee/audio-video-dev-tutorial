#include "mainwindow.h"

#include <QApplication>

#undef main

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/*
音视频同步：
1.视频同步到音频

2.音频同步到视频
*/
