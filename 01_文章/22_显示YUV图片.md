> 文本的主要内容是：使用[SDL](https://www.cnblogs.com/mjios/p/14581738.html#toc_title_1)显示一张YUV图片，整体过程跟《[显示BMP图片](https://www.cnblogs.com/mjios/p/14733502.html)》比较像。

## 宏定义

```cpp
#include <SDL2/SDL.h>
#include <QDebug>

#define END(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }

#define FILENAME "F:/res/in.yuv"
#define PIXEL_FORMAT SDL_PIXELFORMAT_IYUV
#define IMG_W 512
#define IMG_H 512
```

## 变量定义

```cpp
// 窗口
SDL_Window *window = nullptr;

// 渲染上下文
SDL_Renderer *renderer = nullptr;

// 纹理（直接跟特定驱动程序相关的像素数据）
SDL_Texture *texture = nullptr;

// 文件
QFile file(FILENAME);
```

## 初始化子系统

```cpp
// 初始化Video子系统
END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);
```

## 创建窗口

```cpp
// 创建窗口
window = SDL_CreateWindow(
             // 窗口标题
             "SDL显示YUV图片",
             // 窗口X（未定义）
             SDL_WINDOWPOS_UNDEFINED,
             // 窗口Y（未定义）
             SDL_WINDOWPOS_UNDEFINED,
             // 窗口宽度（跟图片宽度一样）
             surface->w,
             // 窗口高度（跟图片高度一样）
             surface->h,
             // 显示窗口
             SDL_WINDOW_SHOWN
         );
END(!window, SDL_CreateWindow);
```

## 创建渲染上下文

```cpp
// 创建渲染上下文（默认的渲染目标是window）
renderer = SDL_CreateRenderer(window, -1,
                              SDL_RENDERER_ACCELERATED |
                              SDL_RENDERER_PRESENTVSYNC);
if (!renderer) { // 说明开启硬件加速失败
    renderer = SDL_CreateRenderer(window, -1, 0);
}
END(!renderer, SDL_CreateRenderer);
```

## 创建纹理

```cpp
// 创建纹理
texture = SDL_CreateTexture(renderer,
                            PIXEL_FORMAT,
                            SDL_TEXTUREACCESS_STREAMING,
                            IMG_W, IMG_H);
END(!texture, SDL_CreateTexture);
```

## 打开文件

```cpp
// 打开文件
if (!file.open(QFile::ReadOnly)) {
    qDebug() << "file open error" << FILENAME;
    goto end;
}
```

## 渲染

```cpp
// 将YUV的像素数据填充到texture
END(SDL_UpdateTexture(texture, nullptr, file.readAll().data(), IMG_W),
    SDL_UpdateTexture);

// 设置绘制颜色（画笔颜色）
END(SDL_SetRenderDrawColor(renderer,
                           0, 0, 0, SDL_ALPHA_OPAQUE),
    SDL_SetRenderDrawColor);

// 用绘制颜色（画笔颜色）清除渲染目标
END(SDL_RenderClear(renderer),
    SDL_RenderClear);

// 拷贝纹理数据到渲染目标（默认是window）
END(SDL_RenderCopy(renderer, texture, nullptr, nullptr),
    SDL_RenderCopy);

// 更新所有的渲染操作到屏幕上
SDL_RenderPresent(renderer);
```

## 延迟退出

```cpp
// 延迟3秒退出
SDL_Delay(3000);
```

## 释放资源

```cpp
end:
    file.close();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
```