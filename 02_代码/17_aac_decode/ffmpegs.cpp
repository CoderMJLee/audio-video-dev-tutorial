#include "ffmpegs.h"
#include <QDebug>
#include <QFile>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

// 输入缓冲区的大小
#define IN_DATA_SIZE 20480
// 需要再次读取输入文件数据的阈值
#define REFILL_THRESH 4096

FFmpegs::FFmpegs() {

}

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

//        for (int i = 0; i < frame->channels; i++) {
//            frame->data[i];
//        }

        // 将解码后的数据写入文件
        outFile.write((char *) frame->data[0], frame->linesize[0]);
    }
}

void FFmpegs::aacDecode(const char *inFilename,
                        AudioDecodeSpec &out) {
    // 返回结果
    int ret = 0;

    // 用来存放读取的输入文件数据（aac）
    // 加上AV_INPUT_BUFFER_PADDING_SIZE是为了防止某些优化过的reader一次性读取过多导致越界
    char inDataArray[IN_DATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    char *inData = inDataArray;

    // 每次从输入文件中读取的长度（aac）
    int inLen;
    // 是否已经读取到了输入文件的尾部
    int inEnd = 0;

    // 文件
    QFile inFile(inFilename);
    QFile outFile(out.filename);

    // 解码器
    AVCodec *codec = nullptr;
    // 上下文
    AVCodecContext *ctx = nullptr;
    // 解析器上下文
    AVCodecParserContext *parserCtx = nullptr;

    // 存放解码前的数据(aac)
    AVPacket *pkt = nullptr;
    // 存放解码后的数据(pcm)
    AVFrame *frame = nullptr;

    // 获取解码器
    codec = avcodec_find_decoder_by_name("libfdk_aac");
    if (!codec) {
        qDebug() << "decoder not found";
        return;
    }

    // 初始化解析器上下文
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        qDebug() << "av_parser_init error";
        return;
    }

    // 创建上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        qDebug() << "avcodec_alloc_context3 error";
        goto end;
    }

    // 创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug() << "av_packet_alloc error";
        goto end;
    }

    // 创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }

    // 打开解码器
    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        qDebug() << "avcodec_open2 error" << errbuf;
        goto end;
    }

    // 打开文件
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "file open error:" << inFilename;
        goto end;
    }
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error:" << out.filename;
        goto end;
    }

    while ((inLen = inFile.read(inDataArray, IN_DATA_SIZE)) > 0) {
        inData = inDataArray;

        while (inLen > 0) {
            // 经过解析器解析
            // 内部调用的核心逻辑是：ff_aac_ac3_parse
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
        }
    }

//    inLen = inFile.read(inData, IN_DATA_SIZE);
//    while (inLen > 0) {
//        // 经过解析器解析
//        // 内部调用的核心逻辑是：ff_aac_ac3_parse
//        ret = av_parser_parse2(parserCtx, ctx,
//                               &pkt->data, &pkt->size,
//                               (uint8_t *) inData, inLen,
//                               AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

//        /*
//         pkt->data = inData;
//         pkt->size = inLen;
//         */
////        qDebug() << pkt->data << (uint8_t *) inData;
////        qDebug() << pkt->size << inLen;

//        if (ret < 0) {
//            ERROR_BUF(ret);
//            qDebug() << "av_parser_parse2 error" << errbuf;
//            goto end;
//        }

//        // 跳过已经解析过的数据
//        inData += ret;
//        // 减去已经解析过的数据大小
//        inLen -= ret;

//        // 解码
//        if (pkt->size > 0 && decode(ctx, pkt, frame, outFile) < 0) {
//            goto end;
//        }

//        // 检查是否需要读取新的文件数据
//        if (inLen < REFILL_THRESH && !inEnd) {
//            // 剩余数据移动到缓冲区的最前面
//            memmove(inDataArray, inData, inLen);

//            // 重置inData
//            inData = inDataArray;

//            // 读取文件数据到inData + inLen位置
//            int len = inFile.read(inData + inLen, IN_DATA_SIZE - inLen);
//            if (len > 0) { // 有读取到文件数据
//                inLen += len;
//            } else { // 文件中已经没有任何数据
//                // 标记为已经读取到文件的尾部
//                inEnd = 1;
//            }
//        }
//    }

    // 刷新缓冲区
//    pkt->data = NULL;
//    pkt->size = 0;
//    decode(ctx, pkt, frame, outFile);
    decode(ctx, nullptr, frame, outFile);

    // 赋值输出参数
    out.sampleRate = ctx->sample_rate;
    out.sampleFmt = ctx->sample_fmt;
    out.chLayout = ctx->channel_layout;

end:
    inFile.close();
    outFile.close();
    av_packet_free(&pkt);
    av_frame_free(&frame);
    av_parser_close(parserCtx);
    avcodec_free_context(&ctx);
}
