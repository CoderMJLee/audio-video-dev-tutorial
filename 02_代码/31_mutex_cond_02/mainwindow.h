#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <list>
#include <SDL2/SDL.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_produceBtn_clicked();

private:
    Ui::MainWindow *ui;
    /** 互斥锁 */
    SDL_mutex *_mutex = nullptr;
    /** 条件变量：消费者等待，生产者唤醒 */
    SDL_cond *_cond1 = nullptr;
    /** 条件变量：生产者等待，消费者唤醒 */
    SDL_cond *_cond2 = nullptr;
    std::list<QString> *_list = nullptr;
    int _index = 0;

    void consume(QString name);
    void produce(QString name);
};
#endif // MAINWINDOW_H
