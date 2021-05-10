#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playthread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _widget = new QWidget(this);
    _widget->setGeometry(200, 50, 512, 512);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_playButton_clicked() {
//    PlayThread *thread = new PlayThread((void *) ui->label->winId(), this);

    PlayThread *thread = new PlayThread((void *) _widget->winId(), this);
    thread->start();
}
