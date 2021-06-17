#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>
#include <QDebug>

#pragma mark - 构造、析构
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 创建互斥锁
    _mutex = SDL_CreateMutex();
    // 创建条件变量
    _cond1 = SDL_CreateCond();
    _cond2 = SDL_CreateCond();

    // 创建链表
    _list = new std::list<QString>();

    // 创建消费者
    consume("消费者1");
    consume("消费者2");
    consume("消费者3");
    consume("消费者4");

    // 创建生产者
    produce("生产者1");
    produce("生产者2");
    produce("生产者3");
}

MainWindow::~MainWindow() {
    delete ui;
    delete _list;
    SDL_DestroyMutex(_mutex);
    SDL_DestroyCond(_cond1);
    SDL_DestroyCond(_cond2);
}

void MainWindow::on_produceBtn_clicked() {
}

void MainWindow::consume(QString name) {
    std::thread([this, name]() {
        SDL_LockMutex(_mutex);

        while (true) {
            qDebug() << name << "开始消费";
            while (!_list->empty()) {
                qDebug() << _list->front();
                // 删除头部
                _list->pop_front();
                // 睡眠500ms
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            // 唤醒生产者：赶紧开始生产
            SDL_CondSignal(_cond2);
            // 等待生产者唤醒
            SDL_CondWait(_cond1, _mutex);
        }

        SDL_UnlockMutex(_mutex);
    }).detach();
}

void MainWindow::produce(QString name) {
    std::thread([this, name]() {
        SDL_LockMutex(_mutex);
        while (true) {
            qDebug() << name << "开始生产";

            _list->push_back(QString("%1").arg(++_index));
            _list->push_back(QString("%1").arg(++_index));
            _list->push_back(QString("%1").arg(++_index));

            // 唤醒消费者：赶紧开始消费
            SDL_CondSignal(_cond1);
            // 等待消费者
            SDL_CondWait(_cond2, _mutex);
        }
        SDL_UnlockMutex(_mutex);
    }).detach();
}
