#include "mainwindow.h"
#include "sender.h"
#include "receiver.h"

#include <QDebug>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    QPushButton *btn = new QPushButton;
    btn->setText("按钮");
    btn->setFixedSize(100, 40);
    btn->setParent(this);
    connect(btn, &QPushButton::clicked,
            this, &MainWindow::handleClick);
//    connect(btn, &QPushButton::clicked, []() {
//           qDebug() << "点击了按钮";
//    });
//    Sender *sender = new Sender;
//    Receiver *receiver = new Receiver;
//    connect(btn, &QPushButton::clicked,
//            sender, &Sender::exit);
//    connect(sender, &Sender::exit, [](int n1, int n2) {
//           qDebug() << "Lambda" << n1 << n2;
//    });
//    emit sender->exit(10, 20);
//    connect(sender, &Sender::exit,
//            receiver, &Receiver::handleExit);
//    connect(sender, &Sender::exit2,
//            receiver, &Receiver::handleExit2);
//    // 连接2个信号
//    connect(sender, &Sender::exit,
//            sender, &Sender::exit2);
//    connect(sender, &Sender::exit2,
//            sender, &Sender::exit);
    // emit sender->exit2(10, 20);
    // qDebug() << emit sender->exit(10, 20);
    // emit sender->exit2();
//    delete sender;
//    delete receiver;
}

void MainWindow::handleClick() {
    qDebug() << "点击了按钮 - handleClick";
}

MainWindow::~MainWindow() {
}

