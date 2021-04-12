#include "playthread.h"

#include <SDL2/SDL.h>
#include <QDebug>
#include <QFile>

#define FILENAME "/Users/mj/Desktop/in.wav"

/*

// 每个样本占用多少个字节
#define BYTES_PER_SAMPLE ((SAMPLE_SIZE * CHANNELS) >> 3)
// 文件缓冲区的大小
#define BUFFER_SIZE (SAMPLES * BYTES_PER_SAMPLE)
*/

typedef struct {
    int len = 0;
    int pullLen = 0;
    Uint8 *data = nullptr;
} AudioBuffer;

PlayThread::PlayThread(QObject *parent) : QThread(parent) {
    connect(this, &PlayThread::finished,
            this, &PlayThread::deleteLater);

}

PlayThread::~PlayThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();

    qDebug() << this << "析构了";
}

// 等待音频设备回调(会回调多次)
void pull_audio_data(void *userdata,
                     // 需要往stream中填充PCM数据
                     Uint8 *stream,
                     // 希望填充的大小(samples * format * channels / 8)
                     int len
                    ) {
    qDebug() << "pull_audio_data" << len;

    // 清空stream（静音处理）
    SDL_memset(stream, 0, len);

    // 取出AudioBuffer
    AudioBuffer *buffer = (AudioBuffer *) userdata;

    // 文件数据还没准备好
    if (buffer->len <= 0) return;

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

/*
SDL播放音频有2种模式：
Push（推）：【程序】主动推送数据给【音频设备】
Pull（拉）：【音频设备】主动向【程序】拉取数据
*/
void PlayThread::run() {
    // 初始化Audio子系统
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug() << "SDL_Init error" << SDL_GetError();
        return;
    }

    // 加载wav文件
    SDL_AudioSpec spec;
    // 指向PCM数据
    Uint8 *data = nullptr;
    // PCM数据的长度
    Uint32 len = 0;
    if (!SDL_LoadWAV(FILENAME, &spec, &data, &len)) {
        qDebug() << "SDL_LoadWAV error" << SDL_GetError();
        // 清除所有的子系统
        SDL_Quit();
        return;
    }

    // 音频缓冲区的样本数量
    spec.samples = 1024;
    // 设置回调
    spec.callback = pull_audio_data;
    // 设置userdata
    AudioBuffer buffer;
    buffer.data = data;
    buffer.len = len;
    spec.userdata = &buffer;

    // 打开设备
    if (SDL_OpenAudio(&spec, nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();
        // 清除所有的子系统
        SDL_Quit();
        return;
    }

    // 开始播放（0是取消暂停）
    SDL_PauseAudio(0);

    // 计算一些参数
    int sampleSize = SDL_AUDIO_BITSIZE(spec.format);
    int bytesPerSample = (sampleSize * spec.channels) >> 3;

    // 存放从文件中读取的数据
    while (!isInterruptionRequested()) {
        // 只要从文件中读取的音频数据，还没有填充完毕，就跳过
        if (buffer.len > 0) continue;

        // 文件数据已经读取完毕
        if (buffer.len <= 0) {
            // 剩余的样本数量
            int samples = buffer.pullLen / bytesPerSample;
            int ms = samples * 1000 / spec.freq;
            SDL_Delay(ms);
            break;
        }
    }

    // 释放WAV文件数据
    SDL_FreeWAV(data);

    // 关闭设备
    SDL_CloseAudio();

    // 清除所有的子系统
    SDL_Quit();
}
