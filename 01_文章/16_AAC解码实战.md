> 本文主要讲解：如何将AAC编码后的数据解码成PCM。

## 命令行

用法非常简单：

```sh
ffmpeg -c:a libfdk_aac -i in.aac -f s16le out.pcm
```

- *-c:a libfdk_aac*
  - 使用fdk-aac解码器
  - 需要注意的是：这个参数要写在aac文件那边，也就是属于**输入**参数

- *-f s16le*
  - 设置PCM文件最终的采样格式

