#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SDL2/SDL.h>
#include <QFile>

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
    void on_playButton_clicked();

private:
    Ui::MainWindow *ui;

    QWidget *_widget;
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    QFile _file;
    int _timerId;

    void timerEvent(QTimerEvent *event);
};
#endif // MAINWINDOW_H
