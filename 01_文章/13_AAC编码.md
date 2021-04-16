> AAC（**A**dvanced **A**udio **C**oding，译为：高级音频编码），是由Fraunhofer IIS、杜比实验室、AT&T、Sony、Nokia等公司共同开发的**有损**音频编码和文件格式。

## 对比MP3

AAC被设计为MP3格式的后继产品，通常在相同的比特率下可以获得比MP3更高的声音质量，是iPhone、iPod、iPad、iTunes的标准音频格式。

AAC相较于MP3的改进包含：

- 更多的采样率选择：8kHz ~ 96kHz，MP3为16kHz ~ 48kHz
- 更高的声道数上限：48个，MP3在MPEG-1模式下为最多双声道，MPEG-2模式下[5.1声道](https://en.wikipedia.org/wiki/5.1_surround_sound)
- 改进的压缩功能：以较小的文件大小提供更高的质量
- 改进的解码效率：需要较少的处理能力进行解码
- ......

## 规格

AAC是一个庞大家族，为了适应不同场合的需要，它有很多种规格可供选择。下面列举其中的9种规格（Profile）：

- MPEG-2 AAC LC：低复杂度规格（Low Complexity）
- MPEG-2 AAC Main：主规格
- MPEG-2 AAC SSR：可变采样率规格（Scaleable Sample Rate）
- MPEG-4 AAC LC：低复杂度规格（Low Complexity）
   - 现在的手机比较常见的MP4文件中的音频部分使用了该规格
- MPEG-4 AAC Main：主规格
- MPEG-4 AAC SSR：可变采样率规格（Scaleable Sample Rate）
- MPEG-4 AAC LTP：长时期预测规格（Long Term Predicition）
- MPEG-4 AAC LD：低延迟规格（Low Delay）
- MPEG-4 AAC HE：高效率规格（High Efficiency）

最早是基于MPEG-2标准，称为：MPEG-2 AAC。后来MPEG-4标准在原来基础上增加了一些新技术，称为：MPEG-4 AAC。

## LC和HE

虽然上面列举了9种规格，但我们目前只需要把注意力放在常用的LC和HE上。下图很好的展示了从LC到HE的发展历程。

![从LC到HE](https://img2020.cnblogs.com/blog/497279/202104/497279-20210408163956985-629555896.png)

![从LC到HE](https://img2020.cnblogs.com/blog/497279/202104/497279-20210408164225812-2026598970.png)

### LC

LC适合中等比特率，比如96kbps ~ 192kbps之间。

**MPEG-4 AAC LC**等价于：

- **MPEG-2 AAC LC** + **PNS**

PNS（[Perceptual Noise Substitution](https://wiki.hydrogenaud.io/index.php?title=Perceptual_Noise_Substitution)）译为：感知噪声替代。
- PNS可以提高AAC的编码效率

### HE

HE有v1和v2两个版本，适合低比特率：
- v1：适合48kbps ~ 64kbps
- v2：适合低于32kbps，可在低至32kbps的比特率下提供接近CD品质的声音

#### v1

**MPEG-4 AAC HE v1**的别名：

- aacPlus v1
- eAAC
- AAC+
- CT-aacPlus（Coding Technologies）
    - Coding Technologies是瑞典是一家技术公司，率先在AAC中使用了SBR技术
    - 在2007年，被杜比实验室（Dolby Laboratories）以2.5亿美元收购

**MPEG-4 AAC HE v1**等价于：

- **MPEG-4 AAC LC** + **SBR**

![SBR](https://img2020.cnblogs.com/blog/497279/202104/497279-20210408172638109-1831642019.jpg)

SBR（[Spectral Band Replication](https://en.wikipedia.org/wiki/Spectral_band_replication)）译为：频段复制。

- 是一种增强的压缩技术
- 可以将高频信号存储在少量的SBR data中
- 解码器可以根据SBR data恢复出高频信号

#### v2

**MPEG-4 AAC HE v2**的别名：

- aacPlus v2
- AAC++
- eAAC+、Enhanced AAC+

**MPEG-4 AAC HE v2**等价于：

- **MPEG-4 AAC HE v1** + **PS**

![PS](https://img2020.cnblogs.com/blog/497279/202104/497279-20210408172640837-1465111111.jpg)

PS（[Parametric Stereo](https://en.wikipedia.org/wiki/Parametric_Stereo)）译为：参数立体声。

- 是一种有损的音频压缩算法，可以进一步提高压缩率
- 可以将左右声道信号组合成单声道信号，声道之间的差异信息存储到少量的PS data中（大概占2 ~ 3kbps）
- 解码器可以根据PS data中恢复出立体声信号

## 编解码器

如果想对PCM数据进行AAC编码压缩，那么就要用到AAC**编**码器（encoder）。
如果想将AAC编码后的数据解压出PCM数据，那么就要用到AAC**解**码器（decoder）。

这里只列举几款常用的AAC**编解**码器：

- [Nero AAC](https://en.wikipedia.org/wiki/Nero_AAC_Codec)
  - 支持LC/HE规格
  - 目前已经停止开发维护

- FFmpeg AAC
  - 支持LC规格
  - FFmpeg官方内置的AAC编解码器，在libavcodec库中
    - 编解码器名字叫做aac
    - 在开发过程中通过这个名字找到编解码器

- [FAAC（Freeware Advanced Audio Coder）](https://en.wikipedia.org/wiki/FAAC)
  - 支持LC规格
  - 可以集成到FFmpeg的libavcodec中
    - 编解码器名字叫做libfaac
    - 在开发过程中通过这个名字找到编解码器，最后调用FAAC库的功能
  - 从2016年开始，FFmpeg已经[移除](https://ffmpeg.org/pipermail/ffmpeg-devel/2016-August/197790.html)了对FAAC的支持
  
- [Fraunhofer FDK AAC](https://en.wikipedia.org/wiki/Fraunhofer_FDK_AAC)
  - 支持LC/HE规格
  - [**目前质量最高**](https://trac.ffmpeg.org/wiki/Encode/AAC)的AAC编解码器
  - 可以集成到FFmpeg的libavcodec中
    - 编解码器名字叫做libfdk_aac
    - 在开发过程中通过这个名字找到编解码器，最后调用FDK AAC库的功能

编码质量排名：Fraunhofer FDK AAC > FFmpeg AAC > FAAC。

## FDK AAC

在网上下载的编译版FFmpeg，通常都是没有集成libfdk_aac的。可以通过命令行查看FFmpeg目前集成的AAC编解码器。

```sh
# windows
ffmpeg -codecs | findstr aac

# mac
ffmpeg -codecs | grep aac
```

我这边的输出结果是：

```sh
DEAIL. aac                  AAC (Advanced Audio Coding) (decoders: aac aac_fixed )
D.AIL. aac_latm             AAC LATM (Advanced Audio Coding LATM syntax)
```

很显然，并没有包含libfdk_aac。

这里给出1个比较推荐的方案：自己[手动编译FFmpeg源码](https://trac.ffmpeg.org/wiki/CompilationGuide)，将libfdk_aac集成到FFmpeg中。
- 自己手动编译的话，想集成啥就集成啥
- 可以把你想要的东西都塞到FFmpeg中，不想要的就删掉
- 也就是根据自己的需要对FFmpeg进行裁剪