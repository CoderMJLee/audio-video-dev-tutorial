#include "mypushbutton.h"
#include <QDebug>

MyPushButton::MyPushButton(QWidget *parent) : QPushButton(parent) {
}

MyPushButton::~MyPushButton() {
    qDebug() << "MyPushButton::~MyPushButton()";
}
