#ifndef FFMPEGS_H
#define FFMPEGS_H

extern "C" {
#include <libavutil/avutil.h>
}

typedef struct {
    char *pixels;
    int width;
    int height;
    AVPixelFormat format;
} RawVideoFrame;

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat format;
} RawVideoFile;

class FFmpegs {
public:
    FFmpegs();
    static void convertRawVideo(RawVideoFrame &in,
                                RawVideoFrame &out);
    static void convertRawVideo(RawVideoFile &in,
                                RawVideoFile &out);
};

#endif // FFMPEGS_H
