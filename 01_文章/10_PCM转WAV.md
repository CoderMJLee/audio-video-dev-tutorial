播放器是无法直接播放PCM的，因为播放器并不知道PCM的采样率、声道数、位深度等参数。当PCM转成某种特定的音频文件格式后（比如转成WAV），就能够被播放器识别播放了。

本文通过2种方式（命令行、编程）演示一下：如何将PCM转成WAV。

## [WAV文件格式](https://wavefilegem.com/how_wave_files_work.html)

在进行PCM转WAV之前，先再来认识一下[WAV的文件格式](http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html)。

- WAV、AVI文件都是基于RIFF标准的文件格式
- RIFF（Resource Interchange File Format，资源交换文件格式）由Microsoft和IBM提出
- 所以WAV、AVI文件的最前面4个字节都是**RIFF**四个字符

找遍了全网，并没有找到令我十分满意的WAV文件格式图，于是按照自己的理解画了一张表格，个人觉得还是极其通俗易懂的。

![WAV文件格式](https://img2020.cnblogs.com/blog/497279/202103/497279-20210319021131588-1269411109.png)

每一个chunk（数据块）都由3部分组成：
- **id**：chunk的标识
- **data size**：chunk的数据部分大小，字节为单位
- **data**，chunk的数据部分

整个WAV文件是一个RIFF chunk，它的data由3部分组成：
- **format**：文件类型
- **fmt chunk**
  - **音频参数**相关的chunk
  - 它的data里面有采样率、声道数、位深度等参数信息
- **data chunk**
  - **音频数据**相关的chunk
  - 它的data就是真正的音频数据（比如PCM数据）

RIFF chunk除去data chunk的data（音频数据）后，剩下的内容可以称为：WAV文件头，一般是44字节。

## 命令行

通过下面的命令可以将PCM转成WAV。

```sh
ffmpeg -ar 44100 -ac 2 -f s16le -i out.pcm out.wav
```

需要注意的是：上面命令生成的WAV文件头有78字节。对比44字节的文件头，它多增加了一个34字节大小的LIST chunk。

关于LIST chunk的参考资料：

  - [What is a “LIST” chunk in a RIFF/Wav header?](https://stackoverflow.com/questions/63929283/what-is-a-list-chunk-in-a-riff-wav-header)
  - [List chunk (of a RIFF file)](https://www.recordingblogs.com/wiki/list-chunk-of-a-wave-file)

加上一个输出文件参数*-bitexact*可以去掉LIST Chunk。

```sh
ffmpeg -ar 44100 -ac 2 -f s16le -i out.pcm -bitexact out2.wav
```

## 编程

在PCM数据的前面插入一个44字节的[WAV文件头](https://www.cnblogs.com/mjios/p/14466420.html#toc_title_28)，就可以将PCM转成WAV。

### WAV的文件头结构

WAV的文件头结构大概如下所示：

```cpp
#define AUDIO_FORMAT_PCM 1
#define AUDIO_FORMAT_FLOAT 3

// WAV文件头（44字节）
typedef struct {
    // RIFF chunk的id
    uint8_t riffChunkId[4] = {'R', 'I', 'F', 'F'};
    // RIFF chunk的data大小，即文件总长度减去8字节
    uint32_t riffChunkDataSize;

    // "WAVE"
    uint8_t format[4] = {'W', 'A', 'V', 'E'};

    /* fmt chunk */
    // fmt chunk的id
    uint8_t fmtChunkId[4] = {'f', 'm', 't', ' '};
    // fmt chunk的data大小：存储PCM数据时，是16
    uint32_t fmtChunkDataSize = 16;
    // 音频编码，1表示PCM，3表示Floating Point
    uint16_t audioFormat = AUDIO_FORMAT_PCM;
    // 声道数
    uint16_t numChannels;
    // 采样率
    uint32_t sampleRate;
    // 字节率 = sampleRate * blockAlign
    uint32_t byteRate;
    // 一个样本的字节数 = bitsPerSample * numChannels >> 3
    uint16_t blockAlign;
    // 位深度
    uint16_t bitsPerSample;

    /* data chunk */
    // data chunk的id
    uint8_t dataChunkId[4] = {'d', 'a', 't', 'a'};
    // data chunk的data大小：音频数据的总长度，即文件总长度减去文件头的长度(一般是44)
    uint32_t dataChunkDataSize;
} WAVHeader;
```

### PCM转WAV核心实现

封装到了FFmpegs类的pcm2wav函数中。

```cpp
#include <QFile>
#include <QDebug>

class FFmpegs {
public:
    FFmpegs();
    static void pcm2wav(WAVHeader &header,
                        const char *pcmFilename,
                        const char *wavFilename);
};

void FFmpegs::pcm2wav(WAVHeader &header,
                      const char *pcmFilename,
                      const char *wavFilename) {
    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.sampleRate * header.blockAlign;

    // 打开pcm文件
    QFile pcmFile(pcmFilename);
    if (!pcmFile.open(QFile::ReadOnly)) {
        qDebug() << "文件打开失败" << pcmFilename;
        return;
    }
    header.dataChunkDataSize = pcmFile.size();
    header.riffChunkDataSize = header.dataChunkDataSize
                               + sizeof (WAVHeader) - 8;

    // 打开wav文件
    QFile wavFile(wavFilename);
    if (!wavFile.open(QFile::WriteOnly)) {
        qDebug() << "文件打开失败" << wavFilename;

        pcmFile.close();
        return;
    }

    // 写入头部
    wavFile.write((const char *) &header, sizeof (WAVHeader));

    // 写入pcm数据
    char buf[1024];
    int size;
    while ((size = pcmFile.read(buf, sizeof (buf))) > 0) {
        wavFile.write(buf, size);
    }

    // 关闭文件
    pcmFile.close();
    wavFile.close();
}
```

### 调用函数

```cpp
// 封装WAV的头部
WAVHeader header;
header.numChannels = 2;
header.sampleRate = 44100;
header.bitsPerSample = 16;

// 调用函数
FFmpegs::pcm2wav(header, "F:/in.pcm", "F:/out.wav");
```