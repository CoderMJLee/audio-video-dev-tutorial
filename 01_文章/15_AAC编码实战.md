> 本文将分别通过命令行、编程2种方式进行AAC编码实战，使用的编码库是libfdk_aac。

## 要求

fdk-aac对输入的PCM数据是有[参数要求](https://wiki.hydrogenaud.io/index.php?title=Fraunhofer_FDK_AAC#Sample_Format)的，如果参数不对，就会出现以下错误：

```sh
[libfdk_aac @ 0x7fa3db033000] Unable to initialize the encoder: SBR library initialization error
Error initializing output stream 0:0 -- Error while opening encoder for output stream #0:0 - maybe incorrect parameters such as bit_rate, rate, width or height
Conversion failed!
```

### 采样格式

必须是16位整数PCM。

### 采样率

支持的采样率有（Hz）：

- 8000、11025、12000、16000、22050、24000、32000
- 44100、48000、64000、88200、96000

## 命令行

### 基本使用

最简单的用法如下所示：

```sh
ffmpeg -ar 44100 -ac 2 -f s16le -i in.pcm -codec:a libfdk_aac out.aac
```

- *-ar 44100 -ac 2 -f s16le*
	- PCM输入数据的参数

- *-c:a*
	- 设置音频编码器  
	- **c**表示codec（编解码器），**a**表示audio（音频）
	- 等价写法
		- *-codec:a*
		- *acodec*

默认生成的aac文件是LC规格的。

```sh
ffprobe out.aac

# 输出结果如下所示
Audio: aac (LC), 44100 Hz, stereo, fltp, 120 kb/s
```

### [常用参数](https://ffmpeg.org/ffmpeg-all.html#libfdk_005faac)

- *-b:a*
	- 设置输出比特率
	- 比如*-b:a 96k*

```sh
ffmpeg -ar 44100 -ac 2 -f s16le -i in.pcm -c:a libfdk_aac -b:a 96k out.aac
```

- *-profile:a*
	- 设置输出规格
	- 取值有：
		- **aac_low**：Low Complexity AAC (LC)，**默认值**
		- **aac_he**：High Efficiency AAC (HE-AAC)
		- **aac_he_v2**：High Efficiency AAC version 2 (HE-AACv2)
		- **aac_ld**：Low Delay AAC (LD)
		- **aac_eld**：Enhanced Low Delay AAC (ELD)
	- 一旦设置了输出规格，会自动设置一个合适的输出比特率
		- 也可以用过*-b:a*自行设置输出比特率

```sh
ffmpeg -ar 44100 -ac 2 -f s16le -i in.pcm -c:a libfdk_aac -profile:a aac_he_v2 -b:a 32k out.aac
```

- *-vbr*
	- 开启[VBR](https://wiki.hydrogenaud.io/index.php?title=Fraunhofer_FDK_AAC#Bitrate_Modes)模式（Variable Bit Rate，可变比特率）
	- 如果开启了VBR模式，*-b:a*选项将会被忽略，但*-profile:a*选项仍然有效
	- 取值范围是0 ~ 5
		- 0：**默认值**，关闭VBR模式，开启CBR模式（Constant Bit Rate，固定比特率）

| VBR  | kbps/channel | AOTs         |
| ---- | ------------ | ------------ |
| 1    | 20-32        | LC、HE、HEv2 |
| 2    | 32-40        | LC、HE、HEv2 |
| 3    | 48-56        | LC、HE、HEv2 |
| 4    | 64-72        | LC           |
| 5    | 96-112       | LC           |

AOT是Audio Object Type的简称。

```sh
ffmpeg -ar 44100 -ac 2 -f s16le -i in.pcm -c:a libfdk_aac -vbr 1 out.aac
```

## 编程

需要用到2个库：

```cpp
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));
```

