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

在Windows环境中，还需要处理一下dll文件，参考：[《dll文件处理》](https://www.cnblogs.com/mjios/p/14465178.html#toc_title_14)。

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

目前只用到了音频功能，所以只需要通过[SDL_init](https://wiki.libsdl.org/SDL_Init)函数初始化Audio子系统即可。

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
/* 一些宏定义 */
// 采样率
#define SAMPLE_RATE 44100
// 采样格式
#define SAMPLE_FORMAT AUDIO_S16LSB
// 采样大小
#define SAMPLE_SIZE SDL_AUDIO_BITSIZE(SAMPLE_FORMAT)
// 声道数
#define CHANNELS 2
// 音频缓冲区的样本数量
#define SAMPLES 1024

// 用于存储读取的音频数据和长度
typedef struct {
    int len = 0;
    int pullLen = 0;
    Uint8 *data = nullptr;
} AudioBuffer;

// 音频参数
SDL_AudioSpec spec;
// 采样率
spec.freq = SAMPLE_RATE;
// 采样格式（s16le）
spec.format = SAMPLE_FORMAT;
// 声道数
spec.channels = CHANNELS;
// 音频缓冲区的样本数量（这个值必须是2的幂）
spec.samples = SAMPLES;
// 回调
spec.callback = pull_audio_data;
// 传递给回调的参数
AudioBuffer buffer;
spec.userdata = &buffer;

// 打开音频设备
if (SDL_OpenAudio(&spec, nullptr)) {
    qDebug() << "SDL_OpenAudio Error" << SDL_GetError();
    // 清除所有初始化的子系统
    SDL_Quit();
    return;
}
```

### 打开文件

```cpp
#define FILENAME "F:/in.pcm"

// 打开文件
QFile file(FILENAME);
if (!file.open(QFile::ReadOnly)) {
    qDebug() << "文件打开失败" << FILENAME;
    // 关闭音频设备
    SDL_CloseAudio();
    // 清除所有初始化的子系统
    SDL_Quit();
    return;
}
```

### 开始播放

```cpp
// 每个样本占用多少个字节
#define BYTES_PER_SAMPLE ((SAMPLE_SIZE * CHANNELS) / 8)
// 文件缓冲区的大小
#define BUFFER_SIZE (SAMPLES * BYTES_PER_SAMPLE)

// 开始播放
SDL_PauseAudio(0);

// 存放文件数据
Uint8 data[BUFFER_LEN];

while (!isInterruptionRequested()) {
    // 只要从文件中读取的音频数据，还没有填充完毕，就跳过
    if (buffer.len > 0) continue;

    buffer.len = file.read((char *) data, BUFFER_SIZE);

    // 文件数据已经读取完毕
    if (buffer.len <= 0) {
        // 剩余的样本数量
        int samples = buffer.pullLen / BYTES_PER_SAMPLE;
        int ms = samples * 1000 / SAMPLE_RATE;
        SDL_Delay(ms);
        break;
    }

    // 读取到了文件数据
    buffer.data = data;
}
```

### 回调函数

```cpp
// userdata：SDL_AudioSpec.userdata
// stream：音频缓冲区（需要将音频数据填充到这个缓冲区）
// len：音频缓冲区的大小（SDL_AudioSpec.samples * 每个样本的大小）
void pull_audio_data(void *userdata, Uint8 *stream, int len) {
    // 清空stream
    SDL_memset(stream, 0, len);

    // 取出缓冲信息
    AudioBuffer *buffer = (AudioBuffer *) userdata;
    if (buffer->len == 0) return;

    // 取len、bufferLen的最小值（为了保证数据安全，防止指针越界）
    buffer->pullLen = (len > buffer->len) ? buffer->len : len;
    
    // 填充数据
    SDL_MixAudio(stream,
                 buffer->data,
                 buffer->pullLen,
                 SDL_MIX_MAXVOLUME);
    buffer->data += buffer->pullLen;
    buffer->len -= buffer->pullLen;
}
```

### 释放资源

```cpp
// 关闭文件
file.close();
// 关闭音频设备
SDL_CloseAudio();
// 清理所有初始化的子系统
SDL_Quit();
```