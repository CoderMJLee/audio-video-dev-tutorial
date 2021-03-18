#include "mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    QPushButton *btn = new QPushButton;
    btn->setText("关闭");
    btn->setFixedSize(100, 30);
    btn->setParent(this);
    // 连接信号与槽
    // 点击按钮，关闭MainWindow窗口
    // btn发出信号
    // MainWindow接收信号，调用槽函数：close
    connect(btn, &QPushButton::clicked,
            this, &MainWindow::close);
}

MainWindow::~MainWindow() {
}

