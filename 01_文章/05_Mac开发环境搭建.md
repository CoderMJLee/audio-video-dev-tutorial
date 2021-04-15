> 在不同的Mac环境下，实践出来的效果可能跟本教程会有所差异。我的Mac环境是：**Intel CPU**、**macOS Big Sur（11.2.2）**。

## FFmpeg

### 安装

在Mac环境中，直接使用**Homebrew**安装FFmpeg即可。还没有安装Homebrew，或者嫌Homebrew下载速度慢的童鞋，可以参考这篇文章：[《Mac安装Homebrew》](https://www.cnblogs.com/mjios/p/14497925.html)。

```sh
brew install ffmpeg
```

安装完毕后，如果能成功在命令行查看FFmpeg的版本号，那说明已经安装成功。

```sh
ffmpeg -version
```

![查看FFmpeg版本](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091847451-909618210.png)

### 目录结构

通过*brew install*安装的软件会存放到*/usr/local/Cellar*目录中，通过以下命令可以打开FFmpeg的安装目录。

```sh
cd /usr/local/Cellar/ffmpeg

open .
```

![FFmpeg的目录结构](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091849938-1949532476.png)

- **bin**：有编译好的可执行程序：ffmpeg、ffplay等，可以直接在命令行上使用，比如
  - *ffplay xx.mp4*：可以直接播放某个视频
  - *ffmpeg -version*：可以查看FFmpeg的版本号
- **include**：开发时需要包含的头文件
- **lib**：链接时需要用到的库文件

## Qt

### 安装

通过*brew install*安装Qt，最终被安装在**/usr/local/Cellar/qt**目录。

```sh
brew install qt
```

通过*brew install --cask*安装Qt Creator，最终被安装在**/usr/local/Caskroom/qt-creator**目录。

```sh
brew install --cask qt-creator
```

### 配置

通过brew安装的Qt和Qt Creator是分开的，需要在Qt Creator中设置一下Qt的路径。

![偏好设置](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091858412-1175661336.png)

![设置Qt](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091902276-1180333441.png)

Qt的路径是在**/usr/local**中，默认是隐藏的。

- 可以使用快捷键**Command + Shift + .**显示隐藏文件和文件夹
- 可以使用快捷键**Command + Shift + G**手动输入Qt的文件夹：**/usr/local/Cellar/qt**

![Qt的文件夹](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308094451598-1114940355.png)

选择**bin**目录下的**qmake**。

![qmake](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091907992-1827070266.png)

![确定](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091910653-1198646103.png)

设置64bit那一项为默认开发环境。

![默认开发环境](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091917012-237493796.png)

滚动到底下，选择刚才设置的Qt版本。

![选择Qt版本](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091920775-1471770940.png)

## 开发
### 新建项目

新建项目的操作和[Windows版本](https://www.cnblogs.com/mjios/p/14465178.html#toc_title_8)差不多。

![选择开发环境](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091835299-2085632160.png)

运行后的效果如下图所示。

![运行效果](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308094856613-232615381.png)

### 集成FFmpeg到Qt项目中

集成FFmpeg的操作也和[Windows版本](https://www.cnblogs.com/mjios/p/14465178.html#toc_title_10)类似，修改**.pro**文件。

```sh
# 设置头文件路径
INCLUDEPATH += /usr/local/Cellar/ffmpeg/4.3.2/include

# 设置库文件路径
LIBS += -L/usr/local/Cellar/ffmpeg/4.3.2/lib \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lpostproc \
        -lswscale \
        -lswresample \
        -lavresample
```

打印FFmpeg版本号。

```cpp
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
}

int main(int argc, char *argv[]) {
    // 打印版本号
    qDebug() << av_version_info();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
```

![FFmpeg版本号](https://img2020.cnblogs.com/blog/497279/202103/497279-20210308091840267-1005574161.png)