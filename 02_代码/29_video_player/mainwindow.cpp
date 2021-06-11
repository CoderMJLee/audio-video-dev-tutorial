#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#pragma mark - 构造、析构
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 创建播放器
    _player = new VideoPlayer();
    connect(_player, &VideoPlayer::stateChanged,
            this, &MainWindow::onPlayerStateChanged);
    connect(_player, &VideoPlayer::initFinished,
            this, &MainWindow::onPlayerInitFinished);
    connect(_player, &VideoPlayer::playFailed,
            this, &MainWindow::onPlayerPlayFailed);
}

MainWindow::~MainWindow() {
    delete ui;
    delete _player;
}

#pragma mark - 私有方法
void MainWindow::onPlayerPlayFailed(VideoPlayer *player) {
    QMessageBox::critical(nullptr, "提示", "哦豁，播放失败！");
}

void MainWindow::onPlayerInitFinished(VideoPlayer *player) {
    int64_t duration = player->getDuration();

    // 设置slider的范围
    ui->currentSlider->setRange(0, duration);

    // 设置label的文字
    ui->durationLabel->setText(getTimeText(duration));
}

void MainWindow::onPlayerStateChanged(VideoPlayer *player) {
    VideoPlayer::State state = player->getState();
    if (state == VideoPlayer::Playing) {
        ui->playBtn->setText("暂停");
    } else {
        ui->playBtn->setText("播放");
    }

    if (state == VideoPlayer::Stopped) {
        ui->playBtn->setEnabled(false);
        ui->stopBtn->setEnabled(false);
        ui->currentSlider->setEnabled(false);
        ui->volumnSlider->setEnabled(false);
        ui->silenceBtn->setEnabled(false);

        ui->durationLabel->setText(getTimeText(0));
        ui->currentSlider->setValue(0);

        // 显示打开文件的页面
        ui->playWidget->setCurrentWidget(ui->openFilePage);
    } else {
        ui->playBtn->setEnabled(true);
        ui->stopBtn->setEnabled(true);
        ui->currentSlider->setEnabled(true);
        ui->volumnSlider->setEnabled(true);
        ui->silenceBtn->setEnabled(true);

        // 显示播放视频的页面
        ui->playWidget->setCurrentWidget(ui->videoPage);
    }
}

void MainWindow::on_stopBtn_clicked() {
    _player->stop();
}

void MainWindow::on_openFileBtn_clicked() {
    QString filename = QFileDialog::getOpenFileName(nullptr,
                       "选择多媒体文件",
                       "F:/res",
                       "视频文件 (*.mp4 *.avi *.mkv);;"
                       "音频文件 (*.mp3 *.aac)");
    qDebug() << "打开文件" << filename;
    if (filename.isEmpty()) return;

    // 开始播放打开的文件
    _player->setFilename(filename.toUtf8().data());
    _player->play();

//    QStringList filenames = QFileDialog::getOpenFileNames(nullptr,
//                            "选择多媒体文件",
//                            "/",
//                            "视频文件 (*.mp4 *.avi *.mkv);;"
//                            "音频文件 (*.mp3 *.aac)");
//    foreach (QString filename, filenames) {
//        qDebug() << filename;
//    }
}

void MainWindow::on_currentSlider_valueChanged(int value) {
    ui->currentLabel->setText(getTimeText(value));
}

void MainWindow::on_volumnSlider_valueChanged(int value) {
    ui->volumnLabel->setText(QString("%1").arg(value));
}

void MainWindow::on_playBtn_clicked() {
    VideoPlayer::State state = _player->getState();
    if (state == VideoPlayer::Playing) {
        _player->pause();
    } else {
        _player->play();
    }
}

QString MainWindow::getTimeText(int value) {
    //    int h = seconds / 3600;
    //    int m = (seconds % 3600) / 60;
    //    int m = (seconds / 60) % 60;
    //    int s = seconds % 60;


//    QString h = QString("0%1").arg(seconds / 3600).right(2);
//    QString m = QString("0%1").arg((seconds / 60) % 60).right(2);
//    QString s = QString("0%1").arg(seconds % 60).right(2);
//    return QString("%1:%2:%3").arg(h).arg(m).arg(s);

    int64_t seconds =  value / 1000000;
    QLatin1Char fill = QLatin1Char('0');
    return QString("%1:%2:%3")
           .arg(seconds / 3600, 2, 10, fill)
           .arg((seconds / 60) % 60, 2, 10, fill)
           .arg(seconds % 60, 2, 10, fill);
}
