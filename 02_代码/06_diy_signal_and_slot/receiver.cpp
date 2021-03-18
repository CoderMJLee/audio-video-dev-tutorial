#include "receiver.h"
#include <QDebug>

Receiver::Receiver(QObject *parent) : QObject(parent) {
}

int Receiver::handleExit(int n1, int n2) {
    qDebug() << "Receiver::handleExit" << n1;
    return n1 + 20;
}

void Receiver::handleExit2(int n1, int n2) {
    qDebug() << "Receiver::handleExit2";
}
