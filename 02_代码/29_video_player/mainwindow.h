#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videoplayer.h"

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
    void onPlayerStateChanged(VideoPlayer *player);
    void onPlayerInitFinished(VideoPlayer *player);
    void onPlayerPlayFailed(VideoPlayer *player);

    void on_stopBtn_clicked();

    void on_openFileBtn_clicked();

    void on_currentSlider_valueChanged(int value);

    void on_volumnSlider_valueChanged(int value);

    void on_playBtn_clicked();

private:
    Ui::MainWindow *ui;
    VideoPlayer *_player;

    QString getTimeText(int value);
};
#endif // MAINWINDOW_H
