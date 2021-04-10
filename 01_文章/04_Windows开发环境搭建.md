## FFmpeg
### 为什么选择FFmpeg？

每个主流平台基本都有自己的音视频开发库（API），用以处理音视频数据，比如：

- **iOS**：AVFoundation、AudioUnit等
- **Android**：MediaPlayer、MediaCodec等
- **Windows**：DirectShow等
- ...

为了让所有平台的开发者都能够学习到音视频开发的通用技术，本教程主要讲解跨平台的音视频开发库[FFmpeg](http://ffmpeg.org/)。其实只要你掌握了FFmpeg，也可以很快上手其他音视频开发库，因为底层原理都是一样的，你最终操作的都是一样的数据，比如MP3、MP4文件。

### 简介

![FFmpeg的Logo](https://img2020.cnblogs.com/blog/497279/202103/497279-20210312213545679-1171268432.png)

FFmpeg的名字由FF和mpeg组成。

- FF是**F**ast **F**orward（译为：快进、快速前进）的简称
- mpeg来源于**M**oving **P**icture **E**xperts **G**roup（简称：MPEG，译为：动态影像专家小组）
	- MPEG是源自ISO与IEC等国际组织的工作小组
	- MPEG的其中一项工作是：制定音视频相关的标准（比如压缩标准）

有很多知名项目都采用了FFmpeg，比如：

- [Google Chrome](https://www.chromium.org/audio-video)：浏览器
- Firefox：浏览器
- [VLC](https://www.videolan.org)：跨平台播放器
- [MPlayer](http://www.mplayerhq.hu)
- [ijkplayer](https://github.com/bilibili/ijkplayer)：bilibili团队开发的Android/iOS播放器框架
- Perian：Apple公司的QuickTime组件

### 下载

网上已经有编译好的FFmpeg，目前最新的Release版本是4.3.2，可以直接下载到项目中进行开发使用。

- [Windows版本下载]( https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-github )：[ffmpeg-4.3.2-2021-02-27-full_build-shared.7z](https://github.com/GyanD/codexffmpeg/releases/download/4.3.2-2021-02-27/ffmpeg-4.3.2-2021-02-27-full_build-shared.7z)
- 源码下载：[ffmpeg-4.3.2.tar.xz](https://ffmpeg.org/releases/ffmpeg-4.3.2.tar.xz)
	- 网上编译好的FFmpeg一般是完整版，体积比较大
	- 日后我们可以根据实际需要，对FFmpeg的源码进行裁剪后再进行重新编译 

解压**ffmpeg-4.3.2-2021-02-27-full_build-shared.7z**文件后，目录结构如下所示：

- **bin**
  - 有编译好的可执行程序：ffmpeg.exe、ffplay.exe、ffprobe.exe，可以直接在命令行上使用，比如
    - *ffplay xx.mp4*：可以直接播放某个视频
    - *ffmpeg -version*：可以查看FFmpeg的版本号
  - 有运行时需要用到的动态库文件（*.dll）
- **doc**：FFmpeg的使用文档
- **include**：开发时需要包含的头文件
- **lib**：链接时需要用到的库文件

![目录结构](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100109685-401144677.png)

## Qt

如果要想开发一个播放器，肯定得编写界面，同样为了保证跨平台开发，这里采用的GUI库是[Qt](https://www.qt.io/)，官方的开发工具QtCreator是跨平台的（支持Windows、Mac、Linux）。Qt开发采用的编程语言是C++。

### 下载

本教程中选择*5.14.2*版本（从*5.15*版本开始是收费版本）。

- 下载地址：https://download.qt.io/archive/qt/5.14/5.14.2/
- Windows系统选择下载：[qt-opensource-windows-x86-5.14.2.exe](https://download.qt.io/archive/qt/5.14/5.14.2/qt-opensource-windows-x86-5.14.2.exe)

![qt安装包](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223356865-1879489588.png)


### 安装

![Next](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223446602-376823512.png)

需要先注册，再进行登录。
![先注册，再登录](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223448612-1507195124.png)

![下一步](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223450410-1093878860.png)

![下一步](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223452846-307812161.png)

![选择安装路径](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223454495-331255702.png)

勾选MinGW 64bit编译器（我电脑的系统是64bit）。
![勾选编译器](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223456117-1313660492.png)

如果后期想阅读Qt源码，就选择勾选Qt源码。
![勾选源码](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303154753827-460388891.png)

默认已经勾选了QtCreator。
![默认已经勾选QtCreator](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223457841-1880158681.png)

![许可协议](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223459568-105685695.png)

![开始菜单快捷方式](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223501405-1820014781.png)

![安装](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223503444-2105003129.png)

### 解决控制台中文乱码

工具 -> 选项。

![](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223542462-1404459733.png)

文本编辑器 -> 行为 -> 文本编码 -> 默认编码。

![设置为UTF-8](https://img2020.cnblogs.com/blog/497279/202103/497279-20210301223544116-1548051098.png)

## 开发初探
### 第一个Qt项目

打开QtCreator，文件 -> 新建文件或项目。

![新建项目](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100337496-1819818685.png)

Application -> Qt Widgets Application。

![Qt Widgets Application](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100339554-1027670443.png)

设置项目名称和项目的存放路径。
![项目名称和项目的存放路径](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100341452-1760777115.png)
![下一步](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100344890-990895790.png)

QtCreator默认会创建一个MainWindow类（主窗口）。
![下一步](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100452551-1579725199.png)
![下一步](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100454228-2065381027.png)

勾选MinGW编译器。
![下一步](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100455958-1241447130.png)
![完成](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100458057-274318672.png)

双击打开**mainwindow.ui**文件，可以往右边的主窗口中添加一些界面元素（比如按钮）。
![双击打开mainwindow.ui文件](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100509272-1615603672.png)
![拖拽](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100511129-245180460.png)

点击左下角的运行按钮（绿色三角形），即可看到一个窗口界面，这个就是QtCreator默认创建的主窗口（MainWindow对象）。

![](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100520847-1054974024.png)
![](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100522293-1578268250.png)

### 消除警告

每次运行Qt程序，你的控制台可能都会出现以下警告信息：**QT_DEVICE_PIXEL_RATIO**已经过期。


```shell
Warning: QT_DEVICE_PIXEL_RATIO is deprecated. Instead use:
   QT_AUTO_SCREEN_SCALE_FACTOR to enable platform plugin controlled per-screen factors.
   QT_SCREEN_SCALE_FACTORS to set per-screen DPI.
   QT_SCALE_FACTOR to set the application global scale factor.
```

解决方案：设置环境变量**QT_SCALE_FACTOR**为1即可。

```cpp
#include "mainwindow.h"

#include <QApplication>

// 导入头文件【也可以不导入，因为<QApplication>中已经包含了<QByteArray>】
// #include <QByteArray>

int main(int argc, char *argv[]) {
    // 通过qputenv函数设置QT_SCALE_FACTOR为1
    qputenv("QT_SCALE_FACTOR", QByteArray("1"));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
```

### 集成FFmpeg到Qt项目中

在Windows中，我们最终是通过调用FFmpeg动态库（dll）中的函数来操作音视频数据，使用dll的其中一种方式是需要用到3种文件：

- **.h**：头文件（Header File）
  - 包含了函数的声明
  - 通过*#include*去导入相应的头文件
- **.dll**：动态链接库（Dynamic Link Library）
  - 包含了函数的具体实现代码
  - Windows程序会在运行过程中，动态调用dll中的函数
- **.lib**或**.dll.a**：（动态链接库的）导入库（Import Library）
  - **.lib**：用于MSVC编译器中
  - **.dll.a**：用于MinGW编译器中
  - 包含了dll中函数的入口，用于辅助找到并调用dll中的函数
  - 最终需要链接到Windows程序中（比如合并到exe文件中）

值得一提的是，在Windows中，静态链接库（Static Link Library）的扩展名也是**.lib**、**.dll.a**。静态链接库和导入库的区别是：

- 静态链接库：包含了函数的具体实现代码
- 导入库：不包含函数的具体实现代码（函数的具体实现代码存储在dll中）

#### 修改**.pro**文件

```shell
# 设置头文件的目录，以便编译器能够找到头文件
INCLUDEPATH += %FFMPEG_HOME%/include

# 设置导入库的目录和需要链接的导入库
LIBS += -L%FFMPEG_HOME%/lib \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lpostproc \
        -lswscale \
        -lswresample
```

- \#号后面的内容是注释
- **%FFMPEG_HOME%**表示**ffmpeg-4.3.2-2021-02-27-full_build-shared.7z**解压后的目录
  - 需要根据你的实际情况修改为真实的路径
- **-L**：设置导入库的目录，以便编译器能够找到导入库
- **-l**：设置需要链接的导入库名称
  - 导入库名称需要去掉文件名前面的lib，比如libavcodec.dll.a就写成avcodec

#### 调用函数

在**main.cpp**中调用*av_version_info*函数，获取FFmpeg的版本信息。

```cpp
#include "mainwindow.h"

#include <QApplication>

// 为了使用qDebug函数
#include <QDebug>

// FFmpeg是C语言库
// 有了extern "C"，才能在C++中导入C语言函数
extern "C" {
#include <libavcodec/avcodec.h>
}

int main(int argc, char *argv[]) {
    // 打印版本信息
    qDebug() << av_version_info();
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
```

#### 拷贝dll文件

Qt程序编译成功后，会生成一个exe文件
- exe是Windows中的一种可执行文件
- 我们编写的程序代码最终都存在了exe文件中

为了保证exe在运行时能成功调用FFmpeg的函数，那么就得让exe能够找到FFmpeg的dll文件（动态库文件）。可以将**%FFMPEG_HOME%/bin**目录中的所有dll文件拷贝到exe文件所在的目录中，exe在运行的时候，能自动找到跟它同一目录下的dll文件。

![](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100542709-187646790.png)

可以通过查看构建目录得知exe文件所在的目录。

![](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100555098-1663556108.png)

运行程序后，如果能在控制台看到**4.3.2**字样的输出信息，说明FFmpeg已经集成成功。

![](https://img2020.cnblogs.com/blog/497279/202103/497279-20210303100603623-1729439411.png)

#### dll的搜索顺序

exe在寻找dll文件时，大概按照以下优先级顺序去查找（这里只列出了大概的搜索路径，并没有写全）：
1. exe所在的目录

2. Windows的System目录
   - C:/Windows/System
   - C:/Windows/System32
3. Windows目录
   - C:/Windows
4. 环境变量Path中的路径
    - 所以可以考虑将**%FFMPEG_HOME%/bin**目录配置到Path变量中
    - 置于如何配置Windows的环境变量Path，这是基本开发常识了，就不再讲解了

第1种方式需要在每一个Qt程序中都拷贝一份FFmpeg的dll文件，第2~4种方式可以让多个Qt程序共用同一份FFmpeg的dll文件。

### .pro文件

.pro文件是Qt项目的主配置文件。

```shell
# 包含了core、gui两个模块
QT       += core gui

# 高于4版本，就包含widgets模块
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
```

### 常用快捷键
- 字体缩放：Ctrl + 鼠标滚轮
- 帮助文档：F1
- 注释：Ctrl + /
- 同名的.h、.cpp文件之间切换：F4