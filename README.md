# ncmdump-android

使用本程序可将下载的网易云音乐缓存文件（ncm）转换为 mp3 或 flac 格式

## 简介

本分支是ncmdump在Android平台上的移植，taglib的android调整改自[taglib-ndk](https://github.com/tumugin/taglib-ndk)

下为原简介：

该版本为最早的 C++ 版本，也是作者开发的市面上第一个支持 ncm 转换的程序

源码复刻自 anonymous5l/ncmdump (2021年10月6日，原作者已经删库)

感谢前辈的付出！此版本做了全操作系统下的的跨平台编译移植，修复了一些内存溢出的问题。

1.3.0 版本更新说明：因为之前有较多 Issue 反馈无法解密带有特殊字符的文件名，例如中文、日文、韩文或者是表情符号等，在1.3.0以及之后的版本彻底修复了这个问题，所有的 UTF-8 字符都可以正常解密。并且还自带了 dll 的构建，可以供其他应用程序（C#、Python、Java等）调用。

## 使用

注意：网易云音乐 3.0 之后的某些版本，下载的 ncm 文件会出现不内置歌曲专辑的封面图片的情况，所需的封面图数据需要从网络获取，介于在一个小工具中嵌入庞大网络库的非必要性，可以移步我的另一个仓库 [ncmdump-go](https://git.taurusxin.com/taurusxin/ncmdump-go) 或者使用基于此项目开发的可视化 GUI 程序 [ncmdump-gui](https://git.taurusxin.com/taurusxin/ncmdump-gui)，其中后者基于前者，均完全使用 Golang 重写，并支持自动从元数据读取封面信息后从网络获取封面图并嵌入到目标音乐文件。

### 命令行工具

用法与原版相同

使用 `-h` 或 `--help` 参数来打印帮助

```shell
ncmdump -h
```

使用 `-v` 或 `--version` 参数来打印版本信息

```shell
ncmdump -v
```

处理单个或多个文件

```shell
ncmdump 1.ncm 2.ncm...
```

使用 `-d` 参数来指定一个文件夹，对文件夹下的所有以 ncm 为扩展名的文件进行批量处理

```shell
ncmdump -d source_dir
```

使用 `-r` 配合 `-d` 参数来递归处理文件夹下的所有以 ncm 为扩展名的文件

```shell
ncmdump -d source_dir -r
```

使用 `-o` 参数来指定输出目录，将转换后的文件输出到指定目录，该参数支持与 `-r` 参数一起使用

```shell
# 处理单个或多个文件并输出到指定目录
ncmdump 1.ncm 2.ncm -o output_dir

# 处理文件夹下的所有以 ncm 为扩展名并输出到指定目录，不包含子文件夹
ncmdump -d source_dir -o output_dir

# 递归处理文件夹并输出到指定目录，并保留目录结构
ncmdump -d source_dir -o output_dir -r
```

### 动态库

或者，如果你想利用此项目进行二次开发，例如在你的Android应用中调用，你可以使用 `libncmdump` 动态库，具体使用方法如下

## 编译项目

克隆本仓库

```shell
git clone https://github.com/taurusxin/ncmdump.git
```

更新子模块

```shell
cd ncmdump
git submodule update --init --recursive
```

调整CMakeLists.txt

# Android Studio
置于你项目的cpp文件夹下

编译Android项目时自动编译，生成numdump和libncmdump.so

# 使用
```kotlin
init {
        System.loadLibrary("ncmdump") // 你的库名
    }

external fun DumpFileTo(inputFilePath: String, outputCachePath: String): Int
```

# DumpFileTo用法
inputFilePath：输入文件路径
outputCachePath:输出文件路径
返回：
-1（失败）
0（成功）

# 注意
在 Android 11 以上的版本，Android对文件读取和写入有较严格的规定，因此上述目录应为应用私有的目录（比如cache）

[![Powered by DartNode]( https://dartnode.com/branding/DN-Open-Source-sm.png)]( https://dartnode.com "Powered by DartNode - Free VPS for Open Source")

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=taurusxin/ncmdump&type=Date)](https://star-history.com/#taurusxin/ncmdump&Date)
