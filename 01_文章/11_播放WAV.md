对于WAV文件来说，可以直接使用*ffplay*命令播放，而且不用像PCM那样增加额外的参数。因为WAV的文件头中已经包含了相关的音频参数信息。

```sh
ffplay in.wav
```

接下来演示一下如何使用SDL播放WAV文件。

## 初始化子系统

```cpp
// 初始化Audio子系统
if (SDL_Init(SDL_INIT_AUDIO)) {
    qDebug() << "SDL_Init error:" << SDL_GetError();
    return;
}
```

## 加载WAV文件

```cpp
// 存放WAV的PCM数据和数据长度
typedef struct {
    Uint32 len = 0;
    int pullLen = 0;
    Uint8 *data = nullptr;
} AudioBuffer;

// WAV中的PCM数据
Uint8 *data;
// WAV中的PCM数据大小（字节）
Uint32 len;
// 音频参数
SDL_AudioSpec spec;

// 加载wav文件
if (!SDL_LoadWAV(FILENAME, &spec, &data, &len)) {
    qDebug() << "SDL_LoadWAV error:" << SDL_GetError();
    // 清除所有的子系统
    SDL_Quit();
    return;
}

// 回调
spec.callback = pull_audio_data;
// 传递给回调函数的userdata
AudioBuffer buffer;
buffer.len = len;
buffer.data = data;
spec.userdata = &buffer;
```

## 打开音频设备

```cpp
// 打开设备
if (SDL_OpenAudio(&spec, nullptr)) {
    qDebug() << "SDL_OpenAudio error:" << SDL_GetError();
    // 释放文件数据
    SDL_FreeWAV(data);
    // 清除所有的子系统
    SDL_Quit();
    return;
}
```

## 开始播放

```cpp
// 开始播放（0是取消暂停）
SDL_PauseAudio(0);

while (!isInterruptionRequested()) {
    if (buffer.len > 0) continue;
    // 每一个样本的大小
    int size = spec.channels * SDL_AUDIO_BITSIZE(spec.format) / 8;
    // 最后一次播放的样本数量
    int samples = buffer.pullLen / size;
    // 最后一次播放的时长
    int ms = samples * 1000 / spec.freq;
    SDL_Delay(ms);
    break;
}
```

## 回调函数

```cpp
// 等待音频设备回调(会回调多次)
void pull_audio_data(void *userdata,
                     // 需要往stream中填充PCM数据
                     Uint8 *stream,
                     // 希望填充的大小(samples * format * channels / 8)
                     int len
                    ) {
    // 清空stream
    SDL_memset(stream, 0, len);

    AudioBuffer *buffer = (AudioBuffer *) userdata;

    // 文件数据还没准备好
    if (buffer->len <= 0) return;

    // 取len、bufferLen的最小值
    buffer->pullLen = (len > (int) buffer->len) ? buffer->len : len;

    // 填充数据
    SDL_MixAudio(stream,
                 buffer->data,
                 buffer->pullLen,
                 SDL_MIX_MAXVOLUME);
    buffer->data += buffer->pullLen;
    buffer->len -= buffer->pullLen;
}
```

## 释放资源

```cpp
// 释放WAV文件数据
SDL_FreeWAV(data);

// 关闭设备
SDL_CloseAudio();

// 清除所有的子系统
SDL_Quit();
```