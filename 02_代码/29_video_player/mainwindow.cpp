#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 创建播放器
    _player = new VideoPlayer();
    connect(_player, &VideoPlayer::stateChanged,
            this, &MainWindow::onPlayerStateChanged);
}

MainWindow::~MainWindow() {
    delete ui;
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

        ui->durationLabel->setText("00:00:00");
        ui->currentSlider->setValue(0);
        ui->volumnSlider->setValue(ui->volumnSlider->maximum());
    } else {
        ui->playBtn->setEnabled(true);
        ui->stopBtn->setEnabled(true);
        ui->currentSlider->setEnabled(true);
        ui->volumnSlider->setEnabled(true);
        ui->silenceBtn->setEnabled(true);
    }
}

void MainWindow::on_stopBtn_clicked() {
    _player->stop();
}

void MainWindow::on_openFileBtn_clicked() {
    QString filename = QFileDialog::getOpenFileName(nullptr,
                       "选择多媒体文件",
                       "/",
                       "视频文件 (*.mp4 *.avi *.mkv);;"
                       "音频文件 (*.mp3 *.aac)");
    qDebug() << "打开文件" << filename;
    if (filename.isEmpty()) return;

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
