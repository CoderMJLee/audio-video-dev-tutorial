# 包含了core、gui两个模块
QT       += core gui

# QT版本大于4，就包含widgets模块
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# 启用c++11的语法（标准）
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 源代码
SOURCES += \
    main.cpp \
    mainwindow.cpp

# 头文件
HEADERS += \
    mainwindow.h

# ui文件
FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 设置FFmpeg头文件的位置，以便Qt能够找到它们
INCLUDEPATH += ../include
#INCLUDEPATH += F:/Dev/ffmpeg-4.3.2/include

# 设置FFmpeg导入库的位置
#LIBS += -L F:/Dev/ffmpeg-4.3.2/lib \
LIBS += -L ../lib \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lpostproc \
        -lswresample \
        -lswscale
