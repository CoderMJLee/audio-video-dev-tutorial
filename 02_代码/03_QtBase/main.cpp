#include "mainwindow.h"

#include <QApplication>

// 很多编程语言中，对象的内存只能放在堆空间中
// 在C++中，对象的内存可以放在栈中

int main(int argc, char *argv[]) {
    // 创建了一个QApplication对象
    // 调用QApplication的构造函数时，传递了2个参数
    // 一个Qt程序中永远只有一个QApplication对象
    QApplication a(argc, argv);
    // 创建主窗口MainWindow对象
    MainWindow w;
    // 显示窗口
    w.show();
    // 运行Qt程序
    return a.exec();
}
