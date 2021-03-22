## ffplay

可以使用*ffplay*播放《音频录制02\_编程》中录制好的PCM文件，测试一下是否录制成功。

播放PCM需要指定相关参数：

- *ar*：采样率
- *ac*：声道数
- *f*：采样格式
	- *s16le*：PCM signed 16-bit little-endian
	- 更多PCM的采样格式可以使用命令查看
		- Windows：*ffmpeg -formats | findstr PCM*
		- Mac：*ffmpeg -formats | grep PCM* 

```sh
ffplay -ar 44100 -ac 2 -f s16le out.pcm
```

接下来演示一下，如何通过编程的方式播放PCM数据。

## SDL

*ffplay*是基于FFmpeg、[SDL](https://www.libsdl.org/)两个库实现的。通过编程的方式播放音视频，也是需要用到这2个库。FFmpeg大家都已经清楚了，比较陌生的是SDL。

![SDL Logo](https://img2020.cnblogs.com/blog/497279/202103/497279-20210322194713070-1369162709.png)

### 简介

SDL（Simple DirectMedia Layer），是一个跨平台的C语言多媒体开发库。
- 支持Windows、Mac OS X、Linux、iOS、Android
- 提供对音频、键盘、鼠标、游戏操纵杆、图形硬件的底层访问
- 很多的视频播放软件、模拟器、受欢迎的游戏都在使用它
- 目前最新的稳定版是：2.0.14
- API文档：[wiki](https://wiki.libsdl.org/)

### 下载

SDL官网下载地址：[download-sdl2](https://www.libsdl.org/download-2.0.php)。

![SDL下载](https://img2020.cnblogs.com/blog/497279/202103/497279-20210319170720628-489825851.png)

#### Windows

由于我们使用的是MinGW编译器，所以选择下载[SDL2-devel-2.0.14-mingw.tar.gz](https://www.libsdl.org/release/SDL2-devel-2.0.14-mingw.tar.gz)。

解压后的目录结构如下图所示，跟FFmpeg的目录结构类似，因此就不再赘述每个文件夹的作用。

![Windows目录结构](https://img2020.cnblogs.com/blog/497279/202103/497279-20210319192209196-814877712.png)

#### Mac

从[brew官网](https://formulae.brew.sh/formula/ffmpeg)可以看得出来：之前执行*brew install ffmpeg*时，已经顺带安装了SDL，安装目录是：*/usr/local/Cellar/sdl2*。

![Mac目录结构](https://img2020.cnblogs.com/blog/497279/202103/497279-20210319193501184-1444915494.png)

如果没有这个目录，就执行*brew install sdl2*进行安装即可。

### HelloWorld

来个简单的SDL HelloWorld吧，打印一下SDL的版本号。

#### .pro文件

```sh
win32 {
    FFMPEG_HOME = F:/Dev/ffmpeg-4.3.2
    SDL_HOME = F:/Dev/SDL2-2.0.14/x86_64-w64-mingw32
}

macx {
    FFMPEG_HOME = /usr/local/Cellar/ffmpeg/4.3.2
    SDL_HOME = /usr/local/Cellar/sdl2/2.0.14_1
}

INCLUDEPATH += $${FFMPEG_HOME}/include
LIBS += -L$${FFMPEG_HOME}/lib \
        -lavdevice \
        -lavcodec \
        -lavformat \
        -lavutil

INCLUDEPATH += $${SDL_HOME}/include
LIBS += -L$${SDL_HOME}/lib \
        -lSDL2
```

在Windows环境中，还需要处理一下dll文件，参考：[《dll文件处理》](https://www.cnblogs.com/mjios/p/14466418.html#toc_title_14)。

#### cpp代码

```cpp
#include <SDL2/SDL.h>

SDL_version v;
SDL_VERSION(&v);
// 2 0 14
qDebug() << v.major << v.minor << v.patch;
```

## 播放PCM

### 初始化子系统

SDL分成好多个子系统（subsystem）：

- Video：显示和窗口管理
- Audio：音频设备管理
- Joystick：游戏摇杆控制
- Timers：定时器
- ...

目前只需要通过[SDL_init](https://wiki.libsdl.org/SDL_Init)函数初始化Audio子系统即可。

```cpp
// 初始化Audio子系统
if (SDL_Init(SDL_INIT_AUDIO)) {
    // 返回值不是0，就代表失败
    qDebug() << "SDL_Init Error" << SDL_GetError();
    return;
}
```

### 打开音频设备

```cpp

```

### 释放资源

```cpp
// 清除所有初始化的子系统
SDL_Quit();
```