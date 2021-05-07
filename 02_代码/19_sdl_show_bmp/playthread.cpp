#include "playthread.h"

#include <SDL2/SDL.h>
#include <QDebug>
#include <QFile>

#define END(judge, func) \
    if (judge) { \
        qDebug() << #func << "error" << SDL_GetError(); \
        goto end; \
    }

PlayThread::PlayThread(QObject *parent) : QThread(parent) {
    connect(this, &PlayThread::finished,
            this, &PlayThread::deleteLater);

}

PlayThread::~PlayThread() {
    disconnect();
    requestInterruption();
    quit();
    wait();

    qDebug() << this << "析构了";
}

void PlayThread::run() {
    // 像素数据
    SDL_Surface *surface = nullptr;

    // 窗口
    SDL_Window *window = nullptr;

    // 渲染上下文
    SDL_Renderer *renderer = nullptr;

    // 纹理（直接跟特定驱动程序相关的像素数据）
    SDL_Texture *texture = nullptr;

    // 矩形框
    SDL_Rect srcRect = {0, 0, 512, 512};
    SDL_Rect dstRect = {200, 200, 100, 100};
    SDL_Rect rect;

    // 初始化子系统
    END(SDL_Init(SDL_INIT_VIDEO), SDL_Init);

    // 加载BMP
    surface = SDL_LoadBMP("F:/res/in.bmp");
    END(!surface, SDL_LoadBMP);

    // 创建窗口
    window = SDL_CreateWindow(
                 // 标题
                 "SDL显示BMP图片",
                 // x
                 SDL_WINDOWPOS_UNDEFINED,
                 // y
                 SDL_WINDOWPOS_UNDEFINED,
                 // w
                 surface->w,
                 // h
                 surface->h,
                 SDL_WINDOW_SHOWN
             );
    END(!window, SDL_CreateWindow);

    // 创建渲染上下文
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, 0);
        END(!renderer, SDL_CreateRenderer);
    }

    // 创建纹理
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    END(!texture, SDL_CreateTextureFromSurface);

    // 画一个红色的矩形框
    END(SDL_SetRenderDrawColor(renderer,
                               255, 0, 0, SDL_ALPHA_OPAQUE),
        SDL_SetRenderDrawColor);
    rect = {0, 0, 50, 50};
    END(SDL_RenderFillRect(renderer, &rect),
        SDL_RenderFillRect);

    // 设置绘制颜色（画笔颜色）
    END(SDL_SetRenderDrawColor(renderer,
                               255, 255, 0, SDL_ALPHA_OPAQUE),
        SDL_SetRenderDrawColor);

    // 用绘制颜色（画笔颜色）清除渲染目标
    END(SDL_RenderClear(renderer),
        SDL_RenderClear);

    // 拷贝纹理数据到渲染目标（默认是window）
    END(SDL_RenderCopy(renderer, texture, &srcRect, &dstRect),
        SDL_RenderCopy);

    // 更新所有的渲染操作到屏幕上
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

end:
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
