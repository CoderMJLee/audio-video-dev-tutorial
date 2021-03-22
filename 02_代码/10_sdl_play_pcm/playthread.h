#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QThread>

class PlayThread : public QThread {
    Q_OBJECT
private:
    void run();

public:
    explicit PlayThread(QObject *parent = nullptr);
    ~PlayThread();

signals:

};

#endif // PLAYTHREAD_H
