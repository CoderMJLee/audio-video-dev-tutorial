#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
#include <libavutil/avutil.h>
}

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat pixFmt;
    int fps;
} VideoDecodeSpec;

class FFmpegs {
public:
    FFmpegs();

    static void h264Decode(const char *inFilename,
                           VideoDecodeSpec &out);
};

#endif // FFMPEGS_H
