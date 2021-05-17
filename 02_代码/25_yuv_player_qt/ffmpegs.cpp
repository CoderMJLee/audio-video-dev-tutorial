#include "ffmpegs.h"
#include <QDebug>
#include <QFile>

extern "C" {
#include <libswscale/swscale.h>
//#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#define ERR_BUF \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

#define END(func) \
    if (ret < 0) { \
        ERR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        goto end; \
    }

FFmpegs::FFmpegs() {

}

void FFmpegs::convertRawVideo(RawVideoFrame &in,
                              RawVideoFrame &out) {
    // 上下文
    SwsContext *ctx = nullptr;
    // 输入、输出缓冲区（指向每一个平面的数据）
    uint8_t *inData[4], *outData[4];
    // 每一个平面的大小
    int inStrides[4], outStrides[4];
    // 每一帧图片的大小
    int inFrameSize, outFrameSize;
    // 返回结果
    int ret = 0;

    // 创建上下文
    ctx = sws_getContext(in.width, in.height, in.format,
                         out.width, out.height, out.format,
                         SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!ctx) {
        qDebug() << "sws_getContext error";
        goto end;
    }

    // 输入缓冲区
    ret = av_image_alloc(inData, inStrides,
                         in.width, in.height, in.format, 1);
    END(av_image_alloc);

    // 输出缓冲区
    ret = av_image_alloc(outData, outStrides,
                         out.width, out.height, out.format, 1);
    END(av_image_alloc);

    // 计算每一帧图片的大小
    inFrameSize = av_image_get_buffer_size(in.format, in.width, in.height, 1);
    outFrameSize = av_image_get_buffer_size(out.format, out.width, out.height, 1);

    // 拷贝输入数据
    memcpy(inData[0], in.pixels, inFrameSize);

    // 转换
    sws_scale(ctx,
              inData, inStrides, 0, in.height,
              outData, outStrides);

    // 写到输出文件去
    out.pixels = (char *) malloc(outFrameSize);
    memcpy(out.pixels, outData[0], outFrameSize);

end:
    av_freep(&inData[0]);
    av_freep(&outData[0]);
    sws_freeContext(ctx);
}

// yuv420p
//    inData[0] = (uint8_t *) malloc(in.frameSize);
//    inData[1] = inData[0] + 所有Y的大小;
//    inData[2] = inData[0] + 所有Y的大小 + 所有U的大小;

//    inStrides[0] = 640; // Y
//    inStrides[1] = 320; // U
//    inStrides[2] = 320; // V
void FFmpegs::convertRawVideo(RawVideoFile &in,
                              RawVideoFile &out) {
    // yuv420p -> yuv444p

    // 上下文
    SwsContext *ctx = nullptr;
    // 输入、输出缓冲区（指向每一个平面的数据）
    uint8_t *inData[4], *outData[4];
    // 每一个平面一行的大小
    int inStrides[4], outStrides[4];
    // 每一帧图片的大小
    int inFrameSize, outFrameSize;
    // 返回结果
    int ret = 0;
    // 进行到了那一帧
    int frameIdx = 0;
    // 文件
    QFile inFile(in.filename), outFile(out.filename);

    // 创建上下文
    ctx = sws_getContext(in.width, in.height, in.format,
                         out.width, out.height, out.format,
                         SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!ctx) {
        qDebug() << "sws_getContext error";
        goto end;
    }

    // 输入缓冲区
    ret = av_image_alloc(inData, inStrides,
                         in.width, in.height, in.format, 1);
    END(av_image_alloc);

    // 输出缓冲区
    ret = av_image_alloc(outData, outStrides,
                         out.width, out.height, out.format, 1);
    END(av_image_alloc);

    // 打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error" << in.filename;
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error" << out.filename;
        goto end;
    }

    // 计算每一帧图片的大小
    inFrameSize = av_image_get_buffer_size(in.format, in.width, in.height, 1);
    outFrameSize = av_image_get_buffer_size(out.format, out.width, out.height, 1);

    // 进行每一帧的转换
    while (inFile.read((char *) inData[0], inFrameSize) == inFrameSize) {
        // 转换
        sws_scale(ctx,
                  inData, inStrides, 0, in.height,
                  outData, outStrides);
        // 写到输出文件去
        outFile.write((char *) outData[0], outFrameSize);
        qDebug() << "转换完第" << frameIdx++ << "帧";
    }

end:
    inFile.close();
    outFile.close();
    av_freep(&inData[0]);
    av_freep(&outData[0]);
    sws_freeContext(ctx);
}
