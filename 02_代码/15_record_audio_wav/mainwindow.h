#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <audiothread.h>

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
    void on_audioButton_clicked();
    void onTimeChanged(unsigned long long ms);

private:
    Ui::MainWindow *ui;
    AudioThread *_audioThread = nullptr;
};
#endif // MAINWINDOW_H
