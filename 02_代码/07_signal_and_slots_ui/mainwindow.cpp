#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_loginButton_clicked() {
    qDebug() << "on_loginButton_clicked";
}

void MainWindow::on_registerButton_clicked() {
    qDebug() << "on_registerButton_clicked";
}
