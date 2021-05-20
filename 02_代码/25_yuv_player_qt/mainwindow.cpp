#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "yuvplayer.h"
#include <QDebug>

static int yuvIdx = 0;
static Yuv yuvs[] = {
    {
        "F:/res/dragon_ball.yuv",
        640, 480,
        AV_PIX_FMT_YUV420P,
        30
    },
    {
        "F:/res/in.yuv",
        512, 512,
        AV_PIX_FMT_YUV420P,
        30
    },
    {
        "F:/res/other/dragon_ball_02.yuv",
        888, 480,
        AV_PIX_FMT_YUV420P,
        30
    },
    {
        "F:/res/other/dragon_ball_03.yuv",
        852, 480,
        AV_PIX_FMT_YUV420P,
        30
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 创建播放器
    _player = new YuvPlayer(this);

    // 监听播放器
    connect(_player, &YuvPlayer::stateChanged,
            this, &MainWindow::onPlayerStateChanged);

    // 设置播放器的位置和尺寸
    int w = 500;
    int h = 500;
    int x = (width() - w) >> 1;
    int y = (height() - h) >> 1;
    _player->setGeometry(x, y, w, h);

    // 设置需要播放的文件
    _player->setYuv(yuvs[yuvIdx]);
    _player->play();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_playButton_clicked() {
    if (_player->isPlaying()) { // 歌曲正在播放
        _player->pause();
    } else { // 歌曲没有正在播放
        _player->play();
    }
}

void MainWindow::on_stopButton_clicked() {
    _player->stop();
}

void MainWindow::onPlayerStateChanged() {
    if (_player->getState() == YuvPlayer::Playing) {
        ui->playButton->setText("暂停");
    } else { // 不是处于正在播放状态
        ui->playButton->setText("播放");
    }
}

void MainWindow::on_nextButton_clicked() {
    int yuvCount = sizeof (yuvs) / sizeof (Yuv);
    yuvIdx = ++yuvIdx % yuvCount;

    _player->stop();
    _player->setYuv(yuvs[yuvIdx]);
    _player->play();
}
