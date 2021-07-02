#include "videoslider.h"
#include <QMouseEvent>
#include <QStyle>

VideoSlider::VideoSlider(QWidget *parent) : QSlider(parent) {

}

void VideoSlider::mousePressEvent(QMouseEvent *ev) {
    // 根据点击位置的x值，计算出对应的value
    // valueRange = max - min
    // value = min + (x / width) * valueRange
    // int value = minimum() + (ev->pos().x() * 1.0 / width()) * (maximum() - minimum());
    // setValue(value);

    int value = QStyle::sliderValueFromPosition(minimum(),
                maximum(),
                ev->pos().x(),
                width());
    setValue(value);

    QSlider::mousePressEvent(ev);

    // 发出信号
    emit clicked(this);
}
