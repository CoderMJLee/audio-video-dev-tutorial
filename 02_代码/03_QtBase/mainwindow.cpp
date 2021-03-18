#include "mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include "mypushbutton.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    qDebug() << this;
    qDebug() << (parent == nullptr);
    qDebug() << "MainWindow被创建了";
    // 设置窗口标题
    setWindowTitle("主窗口");
    // 设置窗口大小
//    resize(600, 600);
    setFixedSize(600, 600);
    // 设置窗口位置
    move(100, 100);
    // 添加第1个按钮
    QPushButton *btn = new QPushButton;
    btn->setText("登录");
    btn->setFixedSize(100, 30);
    btn->move(100, 200);
    // 设置按钮的父控件
    btn->setParent(this);
//    btn->show();
    // 添加第2个按钮
//    new QPushButton("注册", this);
    QPushButton *btn2 = new MyPushButton(this);
    btn2->setText("注册");
}

MainWindow::~MainWindow() {
    qDebug() << "MainWindow被销毁了";
}

