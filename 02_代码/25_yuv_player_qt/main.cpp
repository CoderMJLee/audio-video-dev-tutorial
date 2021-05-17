#include "mainwindow.h"

#include <QApplication>
//#include "ffmpegs.h"

#undef main

// Fault tolerant heap shim applied to current process.
// This is due to previous crash.
/*
解决方案：
1、打开注册表regedit
2、找到HKEY_LOCAL_MACHINE\Software\Microsoft\Windows  NT\CurrentVersion\AppCompatFlags\Layers
3、删掉exe对应的key
*/

int main(int argc, char *argv[]) {
//    RawVideoFile in = {
//        "F:/res/in.yuv",
//        512, 512, AV_PIX_FMT_YUV420P
//    };
//    RawVideoFile out = {
//        "F:/res/dragon_ball.rgb",
//        512, 512, AV_PIX_FMT_RGB24
//    };
//    FFmpegs::convertRawVideo(in, out);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/*
640*480，yuv420p

---- 640个Y -----
YY............YY |
YY............YY |
YY............YY |
YY............YY
................ 480行
YY............YY
YY............YY |
YY............YY |
YY............YY |
YY............YY |

---- 320个U -----
UU............UU |
UU............UU |
UU............UU |
UU............UU
................ 240行
UU............UU
UU............UU |
UU............UU |
UU............UU |
UU............UU |

---- 320个V -----
VV............VV |
VV............VV |
VV............VV |
VV............VV
................ 240行
VV............VV
VV............VV |
VV............VV |
VV............VV |
VV............VV |

600*600，rgb24

-------  600个RGB ------
RGB RGB .... RGB RGB  |
RGB RGB .... RGB RGB  |
RGB RGB .... RGB RGB
RGB RGB .... RGB RGB 600行
RGB RGB .... RGB RGB
RGB RGB .... RGB RGB  |
RGB RGB .... RGB RGB  |
RGB RGB .... RGB RGB  |

6 * 4，yuv420p

YYYYYY
YYYYYY
YYYYYY
YYYYYY

UUU
UUU

VVV
VVV
*/
