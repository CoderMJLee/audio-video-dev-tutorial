#ifndef SENDER_H
#define SENDER_H

#include <QObject>

class Sender : public QObject {
    Q_OBJECT
public:
    explicit Sender(QObject *parent = nullptr);

signals:
    int exit(int n1, int n2);
    void exit2(int n1, int n2);
};

#endif // SENDER_H
