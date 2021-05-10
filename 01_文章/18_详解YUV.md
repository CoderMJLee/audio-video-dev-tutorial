> 本文的主角是多媒体领域非常重要的一个概念：YUV。

## 简介

YUV，是一种颜色编码方法，跟[RGB](https://www.cnblogs.com/mjios/p/14661561.html#toc_title_1)是同一个级别的概念，广泛应用于多媒体领域中。

也就是说，图像中每1个像素的颜色信息，除了可以用RGB的方式表示，也可以用YUV的方式表示。

## vs RGB

对比RGB，YUV有哪些不同和优势呢？

### 体积更小

- 如果使用RGB
	- 比如RGB888（R、G、B每个分量都是8bit）
	- 1个像素占用24bit（3字节）
- 如果使用YUV
	- 1个像素可以减小至平均只占用12bit（1.5字节）
	- 体积为RGB888的一半

### 组成

RGB数据由R、G、B三个分量组成。

YUV数据由Y、U、V三个分量组成，现在通常说的YUV指的是**YCbCr**。

- **Y**：表示亮度（Luminance、Luma），占8bit（1字节）
- **Cb**、**Cr**：表示色度（Chrominance、Chroma）
	- **Cb**（U）：蓝色色度分量，占8bit（1字节）
	- **Cr**（V）：红色色度分量，占8bit（1字节）

![YCbCr](https://img2020.cnblogs.com/blog/497279/202104/497279-20210425100003167-1841615734.gif)

![当Y等于0.5时](https://img2020.cnblogs.com/blog/497279/202104/497279-20210425100109029-57393111.png)

### 兼容性

![原始图像](https://img2020.cnblogs.com/blog/497279/202104/497279-20210427170430773-901459411.png)

![RGB](https://img2020.cnblogs.com/blog/497279/202104/497279-20210427171443529-78372621.png)

![YCbCr](https://img2020.cnblogs.com/blog/497279/202104/497279-20210427171448515-273966348.png)

根据上面的图片，不难看出：
- Y分量对呈现出清晰的图像有着很大的贡献
- Cb、Cr分量的内容不太容易识别清楚

此外，你是否感觉：Y分量的内容看着有点眼熟？其实以前黑白电视的画面就是长这样子的。

YUV的发明处在彩色电视与黑白电视的过渡时期。

- YUV将亮度信息（Y）与色度信息（UV）分离，没有UV信息一样可以显示完整的图像，只不过是黑白的
- 这样的设计很好地解决了彩色电视与黑白电视的兼容性问题，使黑白电视也能够接收彩色电视信号，只不过它只显示了Y分量
- 彩色电视有Y、U、V分量，如果去掉UV分量，剩下的Y分量和黑白电视相同

## 转换

### 公式1

```Text
Y = 0.257R + 0.504G + 0.098B + 16
U = -0.148R - 0.291G + 0.439B + 128
V = 0.439R - 0.368G - 0.071B + 128

R = 1.164(Y - 16) + 2.018(U - 128)
G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
B = 1.164(Y - 16) + 1.596(V - 128)
```

- RGB的取值范围是[0,255]
- Y的取值范围是[16,235]
- UV的取值范围是[16,239]

### 公式2

```Text
Y = 0.299R + 0.587G + 0.114B
U = 0.564(B - Y) = -0.169R - 0.331G + 0.500B
V = 0.713(R - Y) = 0.500R - 0.419G - 0.081B

R = Y + 1.403V
G = Y - 0.344U - 0.714V
B = Y + 1.770U
```

- RGB的取值范围是[0, 1]
- Y的取值范围是[0, 1]
- UV的取值范围是[-0.5, 0.5]

### 公式3

```Text
Y = 0.299R + 0.587G + 0.114B
U = -0.169R - 0.331G + 0.500B + 128
V = 0.500R - 0.419G - 0.081B + 128

R = Y + 1.403(V - 128)
G = Y - 0.343(U - 128) - 0.714(V - 128)
B = Y + 1.770(U - 128)
```

- RGB的取值范围是[0, 255]
- YUV的取值范围是[0, 255]

## 色度二次采样

### 原理

人眼的视网膜上，分布着两种感光细胞：**视杆细胞**和**视锥细胞**。

![视网膜](https://img2020.cnblogs.com/blog/497279/202104/497279-20210426155047283-1582687227.png)

- **视杆细胞**
	- 感知**光线的强弱**
	- 没有色彩识别功能
	- 负责夜间非彩色视觉

- **视锥细胞**
	- 感知**颜色**
	- 负责白天彩色视觉
	- 如果你的视锥细胞发育不正常，数量太少，那感知颜色就会受阻，可能会导致你色弱

![视杆细胞和视锥细胞](https://img2020.cnblogs.com/blog/497279/202104/497279-20210426155049335-298714345.png)

人眼中有上亿个感光细胞，其中视杆细胞占了95%，而视锥细胞仅占5%。

因此，人眼对**亮度**的敏感程度要高于对**色度**的敏感程度，人眼对于亮度的分辨要比对颜色的分辨精细一些。 

如果把图像的色度分量减少一些，人眼也丝毫感觉不到变化和差异。

### 概念

如果在色度分量上进行（相对亮度分量）较低分辨率的采样，也就是存储较多的亮度细节、较少的色度细节，这样就可以在不明显降低画面质量的同时减小图像的体积。上述过程称为：**色度二次采样**（Chroma Subsampling）。

### 采样格式

采样格式通常用A:B:C的形式来表示，比如4:4:4、4:2:2、4:2:0等，其中我们最需要关注的是**4:2:0**。

- **A**：一块A*2个像素的概念区域，一般都是4
- **B**：第1行的色度采样数目
- **C**：第2行的色度采样数目
	- C的值一般要么等于B，要么等于0 

![色度二次采样](https://img2020.cnblogs.com/blog/497279/202104/497279-20210426100922898-174787607.png)

![色度二次采样](https://img2020.cnblogs.com/blog/497279/202104/497279-20210428110749441-1238461596.png)

上图中，不管是哪种采样格式，Y分量都是全水平、全垂直分辨率采样的，每一个像素都有自己独立的Y分量。

#### 4:4:4

- 第1行采集4组CbCr分量，第2行采集4组CbCr分量
- 每1个像素都有自己独立的1组CbCr分量
  - Y分量与CbCr分量的水平方向比例是1:1（每1列都有1组CbCr分量）
  - Y分量与CbCr分量的垂直方向比例是1:1（每1行都有1组CbCr分量）
  - Y分量与CbCr分量的总比例是1:1
- 1个像素占用24bit（3字节），跟RGB888的体积一样
  - 24bpp（bits per pixel）
- 这种格式是没有进行色度二次采样的

![4:4:4](https://img2020.cnblogs.com/blog/497279/202104/497279-20210426104306964-2082053712.gif)

叉叉代表：亮度。

圆圈代表：色度。

#### 4:2:2

- 第1行采集2组CbCr分量，第2行采集2组CbCr分量
- 水平方向相邻的2个像素（1行2列）共用1组CbCr分量
  - Y分量与CbCr分量的水平方向比例是2:1（每2列就有1组CbCr分量）
  - Y分量与CbCr分量的垂直方向比例是1:1（每1行都有1组CbCr分量）
  - Y分量与CbCr分量的总比例是2:1
- 1个像素平均占用16bit（2字节）
  - 16bpp
  - 因为2个像素共占用32bit（4字节 = 2个Y分量 + 1个Cb分量 + 1个Cr分量）

![4:2:2](https://img2020.cnblogs.com/blog/497279/202104/497279-20210426104309834-1923088815.gif)

#### 4:2:0

- 第1行采集2组CbCr分量，第2行共享第1行的CbCr分量
- 相邻的4个像素（2行2列）共用1组CbCr分量
  - Y分量与CbCr分量的水平方向比例是2:1（每2列就有1组CbCr分量）
  - Y分量与CbCr分量的垂直方向比例是2:1（每2行就有1组CbCr分量）
  - Y分量与CbCr分量的总比例是4:1
- 1个像素平均占用12bit（1.5字节）
  - 12bpp
  - 因为4个像素共占用48bit（6字节 = 4个Y分量 + 1个Cb分量 + 1个Cr分量）

![4:2:0 MPEG-1](https://img2020.cnblogs.com/blog/497279/202104/497279-20210426104312115-357762309.gif)

![4:2:0 MPEG-2](https://img2020.cnblogs.com/blog/497279/202104/497279-20210426110953281-1335685401.gif)

## 存储格式

存储格式，决定了YUV数据是如何排列和存储的。本文只介绍一些常见的存储格式。

### 分类

YUV的存储格式可以分为3大类：

- **Planar**（平面）
	- Y、U、V分量分开单独存储
	- 名称通常以字母p结尾
- **Semi-Planar**（半平面）
  - Y分量单独存储，U、V分量交错存储
  - 名称通常以字母sp结尾
- **Packed**（紧凑）
	- 或者叫**Interleaved** （交错）
	- Y、U、V分量交错存储

### 4:4:4

![4:4:4](https://img2020.cnblogs.com/blog/497279/202104/497279-20210427160725877-441224954.png)

#### Planar

- I444

```Text
Y Y Y Y
Y Y Y Y
U U U U
U U U U
V V V V
V V V V
```

- YV24

```Text
Y Y Y Y
Y Y Y Y
V V V V
V V V V
U U U U
U U U U
```

#### Semi-Planar

- NV24

```Text
Y Y Y Y
Y Y Y Y
U V U V U V U V
U V U V U V U V
```

- NV42

```Text
Y Y Y Y
Y Y Y Y
V U V U V U V U
V U V U V U V U
```

### 4:2:2

![4:2:2](https://img2020.cnblogs.com/blog/497279/202104/497279-20210427160727280-1159726486.png)

#### Planar

- I422

```Text
Y Y Y Y
Y Y Y Y
U U
U U
V V
V V
```

- YV16

```Text
Y Y Y Y
Y Y Y Y
V V
V V
U U
U U
```

#### Semi-Planar

- NV16

```Text
Y Y Y Y
Y Y Y Y
U V U V
U V U V
```

- NV61

```Text
Y Y Y Y
Y Y Y Y
V U V U
V U V U
```

#### Packed

- UYVY

```Text
U Y V Y U Y V Y
U Y V Y U Y V Y
```

- YUYV

```Text
Y U Y V Y U Y V 
Y U Y V Y U Y V 
```

- YVYU

```Text
Y V Y U Y V Y U
Y V Y U Y V Y U
```

### 4:2:0

![4:2:0](https://img2020.cnblogs.com/blog/497279/202104/497279-20210427160729037-800619598.png)

#### Planar

- I420
	- 大多数视频解码器以I420格式输出原始图片

```Text
Y Y Y Y
Y Y Y Y
U U
V V
```

![I420](https://img2020.cnblogs.com/blog/497279/202104/497279-20210428111046175-1802363278.png)

- YV12

```Text
Y Y Y Y
Y Y Y Y
V V
U U
```

#### Semi-Planar

- NV12

```Text
Y Y Y Y
Y Y Y Y
U V U V
```

- NV21

```Text
Y Y Y Y
Y Y Y Y
V U V U
```

![4:2:0](https://img2020.cnblogs.com/blog/497279/202104/497279-20210428111059181-1969527422.png)

## 格式转换

### 其他图片格式转YUV

```shell
ffmpeg -i in.png -s 512x512 -pix_fmt yuv420p out.yuv
```

上述命令生成的yuv文件大小是：393216字节 = 512 * 512 * 1.5字节。

- *-s*
  - 设置图片的尺寸
  - 可以用一些[固定字符串](https://ffmpeg.org/ffmpeg-all.html#Video-size)表示尺寸，比如**hd720**表示**1280x720**
  - 如果不设置此选项，默认会跟随输入图片的尺寸
- *-pix_fmt*
  - 设置像素格式
  - 可以通过*ffmpeg -pix_fmts*查看FFmpeg支持的像素格式
  - 如果不设置此选项，默认会跟随输入图片的像素格式
    - 比如可能是**rgb24**、**rgba8**、**pal8**等
    - 可以通过*ffprobe*查看某图片的像素格式，比如*ffprobe in.png*

### YUV转其他图片格式

```shell
ffmpeg -s 512x512 -pix_fmt yuv420p -i in.yuv out.jpg
```

- 这里必须得设置YUV的尺寸（*-s*）、像素格式（*-pix_fmt*）
- 这就类似于：对pcm进行编码时，必须得设置采样率（*-ar*）、声道数（*-ac*）、采样格式（*-f*）

## 显示YUV

### 完整的YUV

可以通过*ffplay*显示YUV数据。

- YUV中直接存储的是所有像素的颜色信息（可以理解为是图像的一种原始数据）

- 必须得设置YUV的尺寸（*-s*）、像素格式（*-pix_fmt*）才能正常显示
- 这就类似于：播放pcm时，必须得设置采样率（*-ar*）、声道数（*-ac*）、采样格式（*-f*）

```shell
ffplay -s 512x512 -pix_fmt yuv420p in.yuv

# 在ffplay中
# -s已经过期，建议改为：-video_size
# -pix_fmt已经过期，建议改为：-pixel_format
ffplay -video_size 512x512 -pixel_format yuv420p in.yuv
```

### 单个分量

可以使用过滤器（filter）显示其中的单个分量（r、g、b、y、u、v）。

```sh
# 只显示r分量
ffplay -vf extractplanes=r in.png

# 只显示g分量
ffplay -vf extractplanes=g in.png

# 只显示b分量
ffplay -vf extractplanes=b in.png

# 只显示y分量
ffplay -video_size 512x512 -pixel_format yuv420p -vf extractplanes=y in.yuv
# 只显示u分量
ffplay -video_size 512x512 -pixel_format yuv420p -vf extractplanes=u in.yuv
# 只显示v分量
ffplay -video_size 512x512 -pixel_format yuv420p -vf extractplanes=v in.yuv
```

- *-vf*
  - 设置视频过滤器
  - 等价写法：*-filter:v*
- [extractplanes](https://ffmpeg.org/ffmpeg-filters.html#extractplanes)
  - 抽取单个分量的内容到灰度视频流中