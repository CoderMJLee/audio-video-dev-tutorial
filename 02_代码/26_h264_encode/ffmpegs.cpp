#include "ffmpegs.h"
#include <QDebug>
#include <QFile>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

FFmpegs::FFmpegs() {

}

// 检查像素格式
static int check_pix_fmt(const AVCodec *codec,
                         enum AVPixelFormat pixFmt) {
    const enum AVPixelFormat *p = codec->pix_fmts;
    while (*p != AV_PIX_FMT_NONE) {
        if (*p == pixFmt) return 1;
        p++;
    }
    return 0;
}

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

    // 不断从编码器中取出编码后的数据
    while (true) {
        ret = avcodec_receive_packet(ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            // 继续读取数据到frame，然后送到编码器
            return 0;
        } else if (ret < 0) { // 其他错误
            return ret;
        }

        // 成功从编码器拿到编码后的数据
        // 将编码后的数据写入文件
        outFile.write((char *) pkt->data, pkt->size);

        // 释放pkt内部的资源
        av_packet_unref(pkt);
    }
}

void FFmpegs::h264Encode(VideoEncodeSpec &in,
                         const char *outFilename) {
    // 文件
    QFile inFile(in.filename);
    QFile outFile(outFilename);

    // 一帧图片的大小
    int imgSize = av_image_get_buffer_size(in.pixFmt, in.width, in.height, 1);

    // 返回结果
    int ret = 0;

    // 编码器
    AVCodec *codec = nullptr;

    // 编码上下文
    AVCodecContext *ctx = nullptr;

    // 存放编码前的数据（yuv）
    AVFrame *frame = nullptr;

    // 存放编码后的数据（h264）
    AVPacket *pkt = nullptr;

//    uint8_t *buf = nullptr;

    // 获取编码器
    codec = avcodec_find_encoder_by_name("libx264");
    if (!codec) {
        qDebug() << "encoder not found";
        return;
    }

    // 检查输入数据的采样格式
    if (!check_pix_fmt(codec, in.pixFmt)) {
        qDebug() << "unsupported pixel format"
                 << av_get_pix_fmt_name(in.pixFmt);
        return;
    }

    // 创建编码上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug() << "avcodec_alloc_context3 error";
        return;
    }

    // 设置yuv参数
    ctx->width = in.width;
    ctx->height = in.height;
    ctx->pix_fmt = in.pixFmt;
    // 设置帧率（1秒钟显示的帧数是in.fps）
    ctx->time_base = {1, in.fps};

    // 打开编码器
    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_open2 error" << errbuf;
        goto end;
    }

    // 创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }

    frame->width = ctx->width;
    frame->height = ctx->height;
    frame->format = ctx->pix_fmt;
    frame->pts = 0;

    // 利用width、height、format创建缓冲区
    ret = av_image_alloc(frame->data, frame->linesize,
                         in.width, in.height, in.pixFmt, 1);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "av_frame_get_buffer error" << errbuf;
        goto end;
    }

    // 创建输入缓冲区（方法2）
//    buf = (uint8_t *) av_malloc(imgSize);
//    ret = av_image_fill_arrays(frame->data, frame->linesize,
//                               buf,
//                               in.pixFmt, in.width, in.height, 1);
//    if (ret < 0) {
//        ERROR_BUF(ret);
//        qDebug() << "av_image_fill_arrays error" << errbuf;
//        goto end;
//    }
//    qDebug() << buf << frame->data[0];

    // 创建输入缓冲区（方法3）
//    ret = av_frame_get_buffer(frame, 0);
//    if (ret < 0) {
//        ERROR_BUF(ret);
//        qDebug() << "av_frame_get_buffer error" << errbuf;
//        goto end;
//    }

    // 创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug() << "av_packet_alloc error";
        goto end;
    }

    // 打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error" << in.filename;
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error" << outFilename;
        goto end;
    }

    // 读取数据到frame中
    while ((ret = inFile.read((char *) frame->data[0],
                              imgSize)) > 0) {
        // 进行编码
        if (encode(ctx, frame, pkt, outFile) < 0) {
            goto end;
        }

        // 设置帧的序号
        frame->pts++;
    }

    // 刷新缓冲区
    encode(ctx, nullptr, pkt, outFile);

end:
    // 关闭文件
    inFile.close();
    outFile.close();

//    av_freep(&buf);

    // 释放资源
    if (frame) {
        av_freep(&frame->data[0]);
//        av_free(frame->data[0]);
//        frame->data[0] = nullptr;
        av_frame_free(&frame);
    }
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);

    qDebug() << "线程正常结束";
}
