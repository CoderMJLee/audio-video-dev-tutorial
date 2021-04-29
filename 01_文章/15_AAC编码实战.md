> 本文将分别通过命令行、编程2种方式进行AAC编码实战，使用的编码库是libfdk_aac。

## 要求

fdk-aac对输入的PCM数据是有[参数要求](https://wiki.hydrogenaud.io/index.php?title=Fraunhofer_FDK_AAC#Sample_Format)的，如果参数不对，就会出现以下错误：

```sh
[libfdk_aac @ 0x7fa3db033000] Unable to initialize the encoder: SBR library initialization error
Error initializing output stream 0:0 -- Error while opening encoder for output stream #0:0 - maybe incorrect parameters such as bit_rate, rate, width or height
Conversion failed!
```

### 采样格式

必须是16位整数PCM。

### 采样率

支持的采样率有（Hz）：

- 8000、11025、12000、16000、22050、24000、32000
- 44100、48000、64000、88200、96000

## 命令行

### 基本使用

最简单的用法如下所示：

```sh
# pcm -> aac
ffmpeg -ar 44100 -ac 2 -f s16le -i in.pcm -c:a libfdk_aac out.aac

# wav -> aac
# 为了简化指令，本文后面会尽量使用in.wav取代in.pcm
ffmpeg -i in.wav -c:a libfdk_aac out.aac
```

- *-ar 44100 -ac 2 -f s16le*
	- PCM输入数据的参数

- *-c:a*
	- 设置音频编码器
	- **c**表示codec（编解码器），**a**表示audio（音频）
	- 等价写法
		- *-codec:a*
		- *-acodec*
	- 需要注意的是：这个参数要写在aac文件那边，也就是属于**输出**参数

默认生成的aac文件是LC规格的。

```sh
ffprobe out.aac

# 输出结果如下所示
Audio: aac (LC), 44100 Hz, stereo, fltp, 120 kb/s
```

### [常用参数](https://ffmpeg.org/ffmpeg-all.html#libfdk_005faac)

- *-b:a*
	- 设置输出比特率
	- 比如*-b:a 96k*

```sh
ffmpeg -i in.wav -c:a libfdk_aac -b:a 96k out.aac
```

- *-profile:a*
	- 设置输出规格
	- 取值有：
		- **aac_low**：Low Complexity AAC (LC)，**默认值**
		- **aac_he**：High Efficiency AAC (HE-AAC)
		- **aac_he_v2**：High Efficiency AAC version 2 (HE-AACv2)
		- **aac_ld**：Low Delay AAC (LD)
		- **aac_eld**：Enhanced Low Delay AAC (ELD)
	- 一旦设置了输出规格，会自动设置一个合适的输出比特率
		- 也可以用过*-b:a*自行设置输出比特率

```sh
ffmpeg -i in.wav -c:a libfdk_aac -profile:a aac_he_v2 -b:a 32k out.aac
```

- *-vbr*
	- 开启[VBR](https://wiki.hydrogenaud.io/index.php?title=Fraunhofer_FDK_AAC#Bitrate_Modes)模式（Variable Bit Rate，可变比特率）
	- 如果开启了VBR模式，*-b:a*选项将会被忽略，但*-profile:a*选项仍然有效
	- 取值范围是0 ~ 5
		- 0：**默认值**，关闭VBR模式，开启CBR模式（Constant Bit Rate，固定比特率）
		- 1：质量最低（但是音质仍旧很棒）
		- 5：质量最高

| VBR  | kbps/channel | AOTs         |
| ---- | ------------ | ------------ |
| 1    | 20-32        | LC、HE、HEv2 |
| 2    | 32-40        | LC、HE、HEv2 |
| 3    | 48-56        | LC、HE、HEv2 |
| 4    | 64-72        | LC           |
| 5    | 96-112       | LC           |

AOT是Audio Object Type的简称。

```sh
ffmpeg -i in.wav -c:a libfdk_aac -vbr 1 out.aac
```

### 文件格式

我曾在[《重识音频》](https://www.cnblogs.com/mjios/p/14466420.html#toc_title_22)中提到，AAC编码的文件扩展名主要有3种：aac、m4a、mp4。

```sh
# m4a
ffmpeg -i in.wav -c:a libfdk_aac out.m4a

# mp4
ffmpeg -i in.wav -c:a libfdk_aac out.mp4
```

## 编程

需要用到2个库：

```cpp
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

// 错误处理
#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));
```

### 函数声明

我们最终会将PCM转AAC的操作封装到一个函数中。

```cpp
extern "C" {
#include <libavcodec/avcodec.h>
}

// 参数
typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} AudioEncodeSpec;

class FFmpegs {
public:
    FFmpegs();

    static void aacEncode(AudioEncodeSpec &in,
                          const char *outFilename);
};
```

### 函数实现

#### 变量定义

```cpp
// 编码器
AVCodec *codec = nullptr;
// 上下文
AVCodecContext *ctx = nullptr;

// 用来存放编码前的数据
AVFrame *frame = nullptr;
// 用来存放编码后的数据
AVPacket *pkt = nullptr;

// 返回结果
int ret = 0;

// 输入文件
QFile inFile(in.filename);
// 输出文件
QFile outFile(outFilename);
```

#### 获取编码器

下面的代码可以获取FFmpeg默认的AAC编码器（并不是libfdk_aac）。

```cpp
AVCodec *codec1 = avcodec_find_encoder(AV_CODEC_ID_AAC);

AVCodec *codec2 = avcodec_find_encoder_by_name("aac");

// true
qDebug() << (codec1 == codec2);

// aac
qDebug() << codec1->name;
```

不过我们最终要获取的是libfdk_aac。

```cpp
// 获取fdk-aac编码器
codec = avcodec_find_encoder_by_name("libfdk_aac");
if (!codec) {
    qDebug() << "encoder libfdk_aac not found";
    return;
}
```

#### 检查采样格式

接下来检查编码器是否支持当前的采样格式。

```cpp
// 检查采样格式
if (!check_sample_fmt(codec, in.sampleFmt)) {
    qDebug() << "Encoder does not support sample format"
             << av_get_sample_fmt_name(in.sampleFmt);
    return;
}
```

检查函数**check_sample_fmt**的实现如下所示。

```cpp
// 检查编码器codec是否支持采样格式sample_fmt
static int check_sample_fmt(const AVCodec *codec,
                            enum AVSampleFormat sample_fmt) {
    const enum AVSampleFormat *p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt) return 1;
        p++;
    }
    return 0;
}
```

#### 创建上下文

**avcodec_alloc_context3**后面的**3**说明这已经是第3版API，取代了此前的**avcodec_alloc_context**和**avcodec_alloc_context2**。

```cpp
// 创建上下文
ctx = avcodec_alloc_context3(codec);
if (!ctx) {
    qDebug() << "avcodec_alloc_context3 error";
    return;
}

// 设置参数
ctx->sample_fmt = in.sampleFmt;
ctx->sample_rate = in.sampleRate;
ctx->channel_layout = in.chLayout;
// 比特率
ctx->bit_rate = 32000;
// 规格
ctx->profile = FF_PROFILE_AAC_HE_V2;
```

#### 打开编码器

```cpp
// 打开编码器
ret = avcodec_open2(ctx, codec, nullptr);
if (ret < 0) {
    ERROR_BUF(ret);
    qDebug() << "avcodec_open2 error" << errbuf;
    goto end;
}
```

如果是想设置一些libfdk_aac特有的参数（比如vbr），可以通过options参数传递。

```cpp
AVDictionary *options = nullptr;
av_dict_set(&options, "vbr", "1", 0);
ret = avcodec_open2(ctx, codec, &options);
```

#### 创建AVFrame

AVFrame用来存放编码前的数据。

```cpp
// 创建AVFrame
frame = av_frame_alloc();
if (!frame) {
    qDebug() << "av_frame_alloc error";
    goto end;
}

// 样本帧数量（由frame_size决定）
frame->nb_samples = ctx->frame_size;
// 采样格式
frame->format = ctx->sample_fmt;
// 声道布局
frame->channel_layout = ctx->channel_layout;
// 创建AVFrame内部的缓冲区
ret = av_frame_get_buffer(frame, 0);
if (ret < 0) {
    ERROR_BUF(ret);
    qDebug() << "av_frame_get_buffer error" << errbuf;
    goto end;
}
```

#### 创建AVPacket

```cpp
// 创建AVPacket
pkt = av_packet_alloc();
if (!pkt) {
    qDebug() << "av_packet_alloc error";
    goto end;
}
```

#### 打开文件

```cpp
// 打开文件
if (!inFile.open(QFile::ReadOnly)) {
    qDebug() << "file open error" << in.filename;
    goto end;
}
if (!outFile.open(QFile::WriteOnly)) {
    qDebug() << "file open error" << outFilename;
    goto end;
}
```

#### 开始编码

```cpp
// frame->linesize[0]是缓冲区的大小
// 读取文件数据
while ((ret = inFile.read((char *) frame->data[0],
                          frame->linesize[0])) > 0) {
    // 最后一次读取文件数据时，有可能并没有填满frame的缓冲区
    if (ret < frame->linesize[0]) {
        // 声道数
        int chs = av_get_channel_layout_nb_channels(frame->channel_layout);
        // 每个样本的大小
        int bytes = av_get_bytes_per_sample((AVSampleFormat) frame->format);
        // 改为真正有效的样本帧数量
        frame->nb_samples = ret / (chs * bytes);
    }

    // 编码
    if (encode(ctx, frame, pkt, outFile) < 0) {
        goto end;
    }
}

// flush编码器
encode(ctx, nullptr, pkt, outFile);
```

**encode**函数专门用来进行编码，它的实现如下所示。

```cpp
// 音频编码
// 返回负数：中途出现了错误
// 返回0：编码操作正常完成
static int encode(AVCodecContext *ctx,
                  AVFrame *frame,
                  AVPacket *pkt,
                  QFile &outFile) {
    // 发送数据到编码器
    int ret = avcodec_send_frame(ctx, frame);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_send_frame error" << errbuf;
        return ret;
    }

    while (true) {
        // 从编码器中获取编码后的数据
        ret = avcodec_receive_packet(ctx, pkt);
        // packet中已经没有数据，需要重新发送数据到编码器（send frame）
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) { // 出现了其他错误
            ERROR_BUF(ret);
            qDebug() << "avcodec_receive_packet error" << errbuf;
            return ret;
        }

        // 将编码后的数据写入文件
        outFile.write((char *) pkt->data, pkt->size);

        // 释放资源
        av_packet_unref(pkt);
    }

    return 0;
}
```

#### 资源回收

```cpp
end:
    // 关闭文件
    inFile.close();
    outFile.close();

    // 释放资源
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);
```

### 函数调用

```cpp
AudioEncodeSpec in;
in.filename = "F:/in.pcm";
in.sampleRate = 44100;
in.sampleFmt = AV_SAMPLE_FMT_S16;
in.chLayout = AV_CH_LAYOUT_STEREO;

FFmpegs::aacEncode(in, "F:/out.aac");
```