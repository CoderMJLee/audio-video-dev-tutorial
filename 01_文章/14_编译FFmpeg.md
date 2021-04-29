> 本文来详细讲解一下：如何在Mac、Windows环境下成功编译FFmpeg。

## 目标

这里先提前说明一下，最后希望达到的效果：

- 编译出**ffmpeg**、**ffprobe**、**ffplay**三个命令行工具
- 只产生动态库，不产生静态库
- 将**fdk-aac**、**x264**、**x265**集成到FFmpeg中
  - x264、x265会在以后讲解的视频模块中用到

## 下载源码

下载源码[ffmpeg-4.3.2.tar.xz](https://ffmpeg.org/releases/ffmpeg-4.3.2.tar.xz)，然后解压。

![FFmpeg源码结构](https://img2020.cnblogs.com/blog/497279/202104/497279-20210410211902005-744008601.png)

## Mac编译

### 依赖项

- *brew install yasm*
  - ffmpeg的编译过程依赖yasm
  - 若未安装yasm会出现错误：nasm/yasm not found or too old. Use --disable-x86asm for a crippled build.
- *brew install sdl2*
	- ffplay依赖于sdl2
	- 如果缺少sdl2，就无法编译出ffplay 
- *brew install fdk-aac*
  - 不然会出现错误：ERROR: libfdk_aac not found 
- *brew install x264*
  - 不然会出现错误：ERROR: libx264 not found 
- *brew install x265*
  - 不然会出现错误：ERROR: libx265 not found 

其实**x264**、**x265**、**sdl2**都在曾经执行*brew install ffmpeg*的时候安装过了。

- 可以通过*brew list*的结果查看是否安装过
	- *brew list | grep fdk*
	- *brew list | grep x26*
	- *brew list | grep -E 'fdk|x26'*
- 如果已经安装过，可以不用再执行*brew install*

### configure

首先进入源码目录。

```sh
# 我的源码放在了Downloads目录下
cd ~/Downloads/ffmpeg-4.3.2
```

然后执行源码目录下的*configure*脚本，设置一些编译参数，做一些编译前的准备工作。

```sh
./configure --prefix=/usr/local/ffmpeg --enable-shared --disable-static --enable-gpl  --enable-nonfree --enable-libfdk-aac --enable-libx264 --enable-libx265
```

- *--prefix*
	- 用以指定编译好的FFmpeg安装到哪个目录
	- 一般放到**/usr/local/ffmpeg**中即可
	
- *--enable-shared*
	- 生成动态库

- *--disable-static*
	- 不生成静态库

- *--enable-libfdk-aac*
	- 将fdk-aac内置到FFmpeg中

- *--enable-libx264*
	- 将x264内置到FFmpeg中

- *--enable-libx265*
	- 将x265内置到FFmpeg中

- *--enable-gpl*
	- x264、x265要求开启[GPL License](https://www.gnu.org/licenses/gpl-3.0.html)

- *--enable-nonfree*
	- [fdk-aac与GPL不兼容](https://github.com/FFmpeg/FFmpeg/blob/master/LICENSE.md)，需要通过开启nonfree进行配置 

你可以通过*configure --help*命令查看每一个配置项的作用。

```sh
./configure --help | grep static

# 结果如下所示
--disable-static         do not build static libraries [no]
```

### 编译

接下来开始解析源代码目录中的Makefile文件，进行编译。*-j8*表示允许同时执行8个编译任务。

```sh
make -j8
```

对于经常在类Unix系统下接触C/C++开发的小伙伴来说，Makefile必然是不陌生的。这里给不了解Makefile的小伙伴简单科普一下：

- Makefile描述了整个项目的编译和链接等规则
  - 比如哪些文件需要编译？哪些文件不需要编译？哪些文件需要先编译？哪些文件需要后编译？等等
- Makefile可以使项目的编译变得自动化，不需要每次都手动输入一堆源文件和参数
  - 比如原来需要这么写：*gcc test1.c test2.c test3.c -o test* 

### 安装

将编译好的库安装到指定的位置：**/usr/local/ffmpeg**。

```sh
make install
```

安装完毕后，**/usr/local/ffmpeg**的目录结构如下所示。

![FFmpeg目录结构](https://img2020.cnblogs.com/blog/497279/202104/497279-20210410215351652-254888592.png)

### 配置PATH

为了让bin目录中的**ffmpeg**、**ffprobe**、**ffplay**在任意位置都能够使用，需要先将bin目录配置到环境变量PATH中。

```sh
# 编辑.zprofile
vim ~/.zprofile

# .zprofile文件中写入以下内容
export PATH=/usr/local/ffmpeg/bin:$PATH

# 让.zprofile生效
source ~/.zprofile
```

如果你用的是bash，而不是zsh，只需要将上面的**.zprofile**换成**.bash_profile**。

### 验证

接下来，在命令行上进行验证。

```sh
ffmpeg -version

# 结果如下所示
ffmpeg version 4.3.2 Copyright (c) 2000-2021 the FFmpeg developers
built with Apple clang version 12.0.0 (clang-1200.0.32.29)
configuration: --prefix=/usr/local/ffmpeg --enable-shared --disable-static --enable-gpl --enable-nonfree --enable-libfdk-aac --enable-libx264 --enable-libx265
libavutil      56. 51.100 / 56. 51.100
libavcodec     58. 91.100 / 58. 91.100
libavformat    58. 45.100 / 58. 45.100
libavdevice    58. 10.100 / 58. 10.100
libavfilter     7. 85.100 /  7. 85.100
libswscale      5.  7.100 /  5.  7.100
libswresample   3.  7.100 /  3.  7.100
libpostproc    55.  7.100 / 55.  7.100
```

此时，你完全可以通过*brew uninstall ffmpeg*卸载以前安装的FFmpeg。

## Windows编译

**configure**、**Makefile**这一套工具是用在类Unix系统上的（Linux、Mac等），默认无法直接用在Windows上。

这里介绍其中一种[可行的解决方案](https://trac.ffmpeg.org/wiki/CompilationGuide/MinGW)：
- 使用[MSYS2](https://www.msys2.org/)软件在Windows上模拟出Linux环境
- 结合使用MinGW对FFmpeg进行编译

### 下载安装MSYS2

进入[MSYS2官网](https://www.msys2.org/)下载安装包（我这边下载的是：[msys2-x86_64-20210228.exe](https://repo.msys2.org/distrib/x86_64/msys2-x86_64-20210228.exe)），然后进行安装。

安装完毕后打开命令行工具**mingw64.exe**。

![mingw64.exe](https://img2020.cnblogs.com/blog/497279/202104/497279-20210416170808700-281534809.png)

### 安装依赖

pacman（Package Manager）是一个包管理工具。

- *pacman -Sl*：搜索有哪些包可以安装
- *pacman -S*：安装
- *pacman -R*：卸载

```sh
# 查看是否有fdk、SDL2相关包（E表示跟一个正则表达式，i表示不区分大小写）
pacman -Sl | grep -Ei 'fdk|sdl2'

# 结果如下所示
mingw32 mingw-w64-i686-SDL2 2.0.14-2
mingw32 mingw-w64-i686-SDL2_gfx 1.0.4-1
mingw32 mingw-w64-i686-SDL2_image 2.0.5-1
mingw32 mingw-w64-i686-SDL2_mixer 2.0.4-2
mingw32 mingw-w64-i686-SDL2_net 2.0.1-1
mingw32 mingw-w64-i686-SDL2_ttf 2.0.15-1
mingw32 mingw-w64-i686-fdk-aac 2.0.1-1
mingw64 mingw-w64-x86_64-SDL2 2.0.14-2
mingw64 mingw-w64-x86_64-SDL2_gfx 1.0.4-1
mingw64 mingw-w64-x86_64-SDL2_image 2.0.5-1
mingw64 mingw-w64-x86_64-SDL2_mixer 2.0.4-2
mingw64 mingw-w64-x86_64-SDL2_net 2.0.1-1
mingw64 mingw-w64-x86_64-SDL2_ttf 2.0.15-1
mingw64 mingw-w64-x86_64-fdk-aac 2.0.1-1
```

接下来，安装各种依赖包。

```sh
# 编译工具链
pacman -S mingw-w64-x86_64-toolchain

pacman -S mingw-w64-x86_64-yasm

pacman -S mingw-w64-x86_64-SDL2

pacman -S mingw-w64-x86_64-fdk-aac

pacman -S mingw-w64-x86_64-x264

pacman -S mingw-w64-x86_64-x265

# 需要单独安装make
pacman -S make
```

关于软件包相关的默认路径：
- 下载目录：**%MSYS2_HOME%/var/cache/pacman/pkg**
- 安装目录：**%MSYS2_HOME%/mingw64**
- **%MSYS2_HOME%**是指MSYS2的安装目录

### configure

我的源码是放在**F:/Dev/ffmpeg-4.3.1**，输入*cd /f/dev/ffmpeg-4.3.1*即可进入源码目录。然后执行**configure**。

```sh
./configure --prefix=/usr/local/ffmpeg --enable-shared --disable-static --enable-gpl  --enable-nonfree --enable-libfdk-aac --enable-libx264 --enable-libx265
```

### 编译、安装

```sh
make -j8 && make install
```

FFmpeg最终会被安装到**%MSYS2_HOME%/usr/local/ffmpeg**目录中。

![FFmpeg的安装目录](https://img2020.cnblogs.com/blog/497279/202104/497279-20210417101940164-2143679821.png)

### bin

此时bin目录中的ffmpeg、ffprobe、ffplay还是没法使用的，因为缺少相关的**dll**，需要从**%MSYS2_HOME%/mingw64/bin**中拷贝，或者将**%MSYS2_HOME%/mingw64/bin**配置到环境变量Path中。

需要拷贝的dll有：**libwinpthread-1**、**SDL2**、**zlib1.dll**、**liblzma-5**、**libbz2-1**、**libiconv-2**、**libgcc_s_seh-1**、**libstdc++-6**、**libx265**、**libx264-159**、**libfdk-aac-2**。

![FFmpeg的bin目录](https://img2020.cnblogs.com/blog/497279/202104/497279-20210419191615801-1704788279.png)

### Path

最后建议将**%FFMPEG_HOME%/bin**目录配置到环境变量Path中。

在命令行输入*ffmpeg -version*，一切大功告成！

![查看版本](https://img2020.cnblogs.com/blog/497279/202104/497279-20210417102933478-118610548.png)