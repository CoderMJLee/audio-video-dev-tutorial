#include "demuxer.h"
#include <QDebug>

#define ERROR_BUF \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

#define END(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        goto end; \
    }

#define RET(func) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        return ret; \
    }

Demuxer::Demuxer() {

}

void Demuxer::demux(const char *inFilename,
                    AudioDecodeSpec &aOut,
                    VideoDecodeSpec &vOut) {
    // 保留参数
    _aOut = &aOut;
    _vOut = &vOut;

    // 返回结果
    int ret = 0;

    // 存放解码前的数据
    AVPacket pkt;

    // 创建解封装上下文、打开文件
    ret = avformat_open_input(&_fmtCtx, inFilename, nullptr, nullptr);
    END(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    END(avformat_find_stream_info);

    // 打印流信息到控制台
    av_dump_format(_fmtCtx, 0, inFilename, 0);
    fflush(stderr);

    // 初始化音频信息
    ret = initAudioInfo();
    if (ret < 0) {
        goto end;
    }

    // 初始化视频信息
    ret = initVideoInfo();
    if (ret < 0) {
        goto end;
    }

    // 初始化frame
    _frame = av_frame_alloc();
    if (!_frame) {
        qDebug() << "av_frame_alloc error";
        goto end;
    }

    // 初始化pkt
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    // 从输入文件中读取数据
    while (av_read_frame(_fmtCtx, &pkt) == 0) {
        if (pkt.stream_index == _aStreamIdx) { // 读取到的是音频数据
            ret = decode(_aDecodeCtx, &pkt);
        } else if (pkt.stream_index == _vStreamIdx) { // 读取到的是视频数据
            ret = decode(_vDecodeCtx, &pkt);
        }
        av_packet_unref(&pkt);

        if (ret < 0) {
            goto end;
        }
    }

    // 刷新缓冲区
    decode(_aDecodeCtx, nullptr);
    decode(_vDecodeCtx, nullptr);

end:
    _aOutFile.close();
    _vOutFile.close();
    avcodec_free_context(&_aDecodeCtx);
    avcodec_free_context(&_vDecodeCtx);
    avformat_close_input(&_fmtCtx);
    av_frame_free(&_frame);
}

// 初始化音频信息
int Demuxer::initAudioInfo() {
    // 初始化解码器
    int ret = initDecoder(&_aDecodeCtx, &_aStreamIdx, AVMEDIA_TYPE_AUDIO);
    if (ret < 0) {
        return ret;
    }

    // 打开文件
    _aOutFile.setFileName(_aOut->filename);
    if (!_aOutFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error" << _aOut->filename;
        return -1;
    }

    // 保存音频参数
    _aOut->sampleRate = _aDecodeCtx->sample_rate;
    _aOut->sampleFmt = _aDecodeCtx->sample_fmt;
    _aOut->chLayout = _aDecodeCtx->channel_layout;

    return 0;
}

// 初始化视频信息
int Demuxer::initVideoInfo() {
    // 初始化解码器
    int ret = initDecoder(&_vDecodeCtx, &_vStreamIdx, AVMEDIA_TYPE_VIDEO);
    if (ret < 0) {
        return ret;
    }

    // 打开文件
    _vOutFile.setFileName(_vOut->filename);
    if (!_vOutFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error" << _vOut->filename;
        return -1;
    }

    // 保存视频参数
    _vOut->width = _vDecodeCtx->width;
    _vOut->height = _vDecodeCtx->height;
    _vOut->fps = _vDecodeCtx->framerate.num;
    _vOut->pixFmt = _vDecodeCtx->pix_fmt;

    return 0;
}

// 初始化解码器
int Demuxer::initDecoder(AVCodecContext **decodeCtx,
                         int *streamIdx,
                         AVMediaType type) {
    // 根据type寻找最合适的流信息
    // 返回值是流索引
    int ret = av_find_best_stream(_fmtCtx, type, -1, -1, nullptr, 0);
    RET(av_find_best_stream);

    // 检验流
    *streamIdx = ret;
    AVStream *stream = _fmtCtx->streams[*streamIdx];
    if (!stream) {
        qDebug() << "stream is empty";
        return -1;
    }

    // 为当前流找到合适的解码器
    AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        qDebug() << "decoder not found" << stream->codecpar->codec_id;
        return -1;
    }

    // 初始化解码上下文
    *decodeCtx = avcodec_alloc_context3(decoder);
    if (!decodeCtx) {
        qDebug() << "avcodec_alloc_context3 error";
        return -1;
    }

    // 从流中拷贝参数到解码上下文中
    ret = avcodec_parameters_to_context(*decodeCtx, stream->codecpar);
    RET(avcodec_parameters_to_context);

    // 打开解码器
    ret = avcodec_open2(*decodeCtx, decoder, nullptr);
    RET(avcodec_open2);

    return 0;
}

int Demuxer::decode(AVCodecContext *decodeCtx, AVPacket *pkt) {
    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(decodeCtx, pkt);
    RET(avcodec_send_packet);

    while (true) {
        // 获取解码后的数据
        ret = avcodec_receive_frame(decodeCtx, _frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        }
        RET(avcodec_receive_frame);

        // 将frame的数据写入文件
//        if (pkt->stream_index == _vStreamIdx) {
        if (decodeCtx->codec->type == AVMEDIA_TYPE_VIDEO) {
            writeVideoFrame();
        } else {
            writeAudioFrame();
        }
    }
}

void Demuxer::writeVideoFrame() {
    // 写入Y平面
    _vOutFile.write((char *) _frame->data[0],
                    _frame->linesize[0] * _vOut->height);
    // 写入U平面
    _vOutFile.write((char *) _frame->data[1],
                    _frame->linesize[1] * _vOut->height >> 1);
    // 写入V平面
    _vOutFile.write((char *) _frame->data[2],
                    _frame->linesize[2] * _vOut->height >> 1);
}

void Demuxer::writeAudioFrame() {
    // 考虑好plannar
//    _frame
//    _aOutFile
}
