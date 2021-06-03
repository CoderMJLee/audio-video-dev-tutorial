#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_stopBtn_clicked() {
    int count = ui->playWidget->count();
    int idx = ui->playWidget->currentIndex();
    ui->playWidget->setCurrentIndex(++idx % count);
}
