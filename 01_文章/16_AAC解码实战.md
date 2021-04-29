> 本文主要讲解：如何将AAC编码后的数据解码成PCM。

## 命令行

用法非常简单：

```sh
ffmpeg -c:a libfdk_aac -i in.aac -f s16le out.pcm
```

- *-c:a libfdk_aac*
  - 使用fdk-aac解码器
  - 需要注意的是：这个参数要写在aac文件那边，也就是属于**输入**参数

- *-f s16le*
  - 设置PCM文件最终的采样格式

## 编程

需要用到2个库：

```cpp
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));
```

### 函数声明

我们最终会将AAC解码的操作封装到一个函数中。

```cpp
// 解码后的PCM参数
typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} AudioDecodeSpec;

class FFmpegs {
public:
    FFmpegs();

    static void aacDecode(const char *inFilename,
                          AudioDecodeSpec &out);
};
```

### 函数实现

#### 变量定义

```cpp
// 输入缓冲区的大小
#define IN_DATA_SIZE 20480
// 需要再次读取输入文件数据的阈值
#define REFILL_THRESH 4096

// 返回结果
int ret = 0;

// 每次从输入文件中读取的长度
int inLen = 0;
// 是否已经读取到了输入文件的尾部
int inEnd = 0;

// 用来存放读取的文件数据
// 加上AV_INPUT_BUFFER_PADDING_SIZE是为了防止某些优化过的reader一次性读取过多导致越界
char inDataArray[IN_DATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
char *inData = inDataArray;

// 文件
QFile inFile(inFilename);
QFile outFile(out.filename);

// 解码器
AVCodec *codec = nullptr;
// 上下文
AVCodecContext *ctx = nullptr;
// 解析器上下文
AVCodecParserContext *parserCtx = nullptr;

// 存放解码前的数据
AVPacket *pkt = nullptr;
// 存放解码后的数据
AVFrame *frame = nullptr;
```

#### 获取解码器

```cpp
// 获取解码器
codec = avcodec_find_decoder_by_name("libfdk_aac");
if (!codec) {
    qDebug() << "decoder libfdk_aac not found";
    return;
}
```

#### 初始化解析器上下文

```cpp
// 初始化解析器上下文
parserCtx = av_parser_init(codec->id);
if (!parserCtx) {
    qDebug() << "av_parser_init error";
    return;
}
```

#### 创建上下文

```cpp
// 创建上下文
ctx = avcodec_alloc_context3(codec);
if (!ctx) {
    qDebug() << "avcodec_alloc_context3 error";
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

#### 创建AVFrame

```cpp
// 创建AVFrame
frame = av_frame_alloc();
if (!frame) {
    qDebug() << "av_frame_alloc error";
    goto end;
}
```

#### 打开解码器

```cpp
// 打开解码器
ret = avcodec_open2(ctx, codec, nullptr);
if (ret < 0) {
    ERROR_BUF(ret);
    qDebug() << "avcodec_open2 error" << errbuf;
    goto end;
}
```

#### 打开文件

```cpp
// 打开文件
if (!inFile.open(QFile::ReadOnly)) {
    qDebug() << "file open error:" << inFilename;
    goto end;
}
if (!outFile.open(QFile::WriteOnly)) {
    qDebug() << "file open error:" << out.filename;
    goto end;
}
```

#### 解码

```cpp
// 读取数据
inLen = inFile.read(inData, IN_DATA_SIZE);
while (inLen > 0) {
    // 经过解析器上下文处理
    ret = av_parser_parse2(parserCtx, ctx,
                           &pkt->data, &pkt->size,
                           (uint8_t *) inData, inLen,
                           AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "av_parser_parse2 error" << errbuf;
        goto end;
    }
	// 跳过已经解析过的数据
    inData += ret;
    // 减去已经解析过的数据大小
    inLen -= ret;

    // 解码
    if (pkt->size > 0 && decode(ctx, pkt, frame, outFile) < 0) {
        goto end;
    }

    // 如果数据不够了，再次读取文件
    if (inLen < REFILL_THRESH && !inEnd) {
        // 剩余数据移动到缓冲区前
        memmove(inDataArray, inData, inLen);
        inData = inDataArray;

        // 跨过已有数据，读取文件数据
        int len = inFile.read(inData + inLen, IN_DATA_SIZE - inLen);
        if (len > 0) {
            inLen += len;
        } else {
            inEnd = 1;
        }
    }
}

// flush解码器
//    pkt->data = NULL;
//    pkt->size = 0;
decode(ctx, nullptr, frame, outFile);
```

具体的解码操作在**decode**函数中。

```cpp
static int decode(AVCodecContext *ctx,
                  AVPacket *pkt,
                  AVFrame *frame,
                  QFile &outFile) {
    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(ctx, pkt);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_send_packet error" << errbuf;
        return ret;
    }

    while (true) {
        // 获取解码后的数据
        ret = avcodec_receive_frame(ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            ERROR_BUF(ret);
            qDebug() << "avcodec_receive_frame error" << errbuf;
            return ret;
        }
        // 将解码后的数据写入文件
        outFile.write((char *) frame->data[0], frame->linesize[0]);
    }
}
```

#### 设置输出参数

```cpp
// 设置输出参数
out.sampleRate = ctx->sample_rate;
out.sampleFmt = ctx->sample_fmt;
out.chLayout = ctx->channel_layout;
```

#### 释放资源

```cpp
end:
    inFile.close();
    outFile.close();
    av_frame_free(&frame);
    av_packet_free(&pkt);
    av_parser_close(parserCtx);
    avcodec_free_context(&ctx);
```

### 函数调用

```cpp
AudioDecodeSpec out;
out.filename = "F:/out.pcm";
FFmpegs::aacDecode("F:/in.aac", out);
// 44100
qDebug() << out.sampleRate;
// s16
qDebug() << av_get_sample_fmt_name(out.sampleFmt);
// 2
qDebug() << av_get_channel_layout_nb_channels(out.chLayout);
```