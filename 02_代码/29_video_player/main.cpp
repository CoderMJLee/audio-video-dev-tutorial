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

/*
1.现实时间
比如一个视频的时长是120秒，其中120秒就是现实时间
比如一个视频播放到了第58秒，其中第58秒就是现实时间

2.FFmpeg时间
1> 时间戳（timestamp），类型是int64_t
2> 时间基（time base\unit），是时间戳的单位，类型是AVRational

3.FFmpeg时间 与 现实时间的转换
1> 现实时间 = 时间戳 * (时间基的分子 / 时间基的分母)
2> 现实时间 = 时间戳 * av_q2d(时间基)
3> 时间戳 = 现实时间 / (时间基的分子 / 时间基的分母)
4> 时间戳 = 现实时间 / av_q2d(时间基)
*/
