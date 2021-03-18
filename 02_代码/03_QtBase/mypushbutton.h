#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>

class MyPushButton : public QPushButton {
    Q_OBJECT
    ~MyPushButton();

public:
    explicit MyPushButton(QWidget *parent = nullptr);

signals:

};

#endif // MYPUSHBUTTON_H
