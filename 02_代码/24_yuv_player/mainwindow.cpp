#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "yuvplayer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 创建播放器
    _player = new YuvPlayer(this);
    int w = 400;
    int h = 400;
    int x = (width() - w) >> 1;
    int y = (height() - h) >> 1;
    _player->setGeometry(x, y, w, h);

    // 设置需要播放的文件
    Yuv yuv = {
        "F:/res/dragon_ball.yuv",
        640, 480,
        AV_PIX_FMT_YUV420P,
        30
    };
    _player->setYuv(yuv);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_playButton_clicked() {
    if (_player->isPlaying()) { // 歌曲正在播放
        _player->pause();

        ui->playButton->setText("播放");
    } else { // 歌曲没有正在播放
        _player->play();

        ui->playButton->setText("暂停");
    }
}

void MainWindow::on_stopButton_clicked() {
    _player->stop();
}
