# ncmdump

使用本程序可将下载的网易云音乐缓存文件（ncm）转换为 mp3 或 flac 格式

## 简介

该版本为最早的 C++ 版本，也是作者开发的市面上第一个支持 ncm 转换的程序

源码复刻自 anonymous5l/ncmdump (2021年10月6日，原作者已经删库)

感谢前辈的付出！此版本做了全操作系统下的的跨平台编译移植，修复了一些内存溢出的问题。

1.3.0 版本更新说明：因为之前有较多 Issue 反馈无法解密带有特殊字符的文件名，例如中文、日文、韩文或者是表情符号等，在1.3.0以及之后的版本彻底修复了这个问题，所有的 UTF-8 字符都可以正常解密。并且还自带了 dll 的构建，可以供其他应用程序（C#、Python、Java等）调用。

## 传送门

2021年10月6日，原作者已经删库

## 使用

注意：网易云音乐 3.0 之后的某些版本，下载的 ncm 文件会出现不内置歌曲专辑的封面图片的情况，所需的封面图数据需要从网络获取，介于在一个小工具中嵌入庞大网络库的非必要性，可以移步我的另一个仓库 [ncmdump-go](https://git.taurusxin.com/taurusxin/ncmdump-go) 或者使用基于此项目开发的可视化 GUI 程序 [ncmdump-gui](https://git.taurusxin.com/taurusxin/ncmdump-gui)，其中后者基于前者，均完全使用 Golang 重写，并支持自动从元数据读取封面信息后从网络获取封面图并嵌入到目标音乐文件。

### 命令行工具

从 [Release](https://github.com/taurusxin/ncmdump/releases) 下载最新版本的对应系统的已编译好的二进制文件

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

或者，如果你想利用此项目进行二次开发，例如在你的 C#、Python、Java 等项目中调用，你可以使用 `libncmdump` 动态库，具体使用方法见仓库的 `example` 文件夹

请注意！如果你在 Windows 下开发，传递到库构造函数的文件名编码**必须为 UTF-8 编码**，否则会抛出运行时错误。

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

使用 CMake 配置项目。Windows 下若使用 GNU 套件，推荐使用 [msys2](https://www.msys2.org/) 或者 [winlibs](https://winlibs.com/)

```shell
# Windows MinGW
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -B build

# Windows MSVC
cmake -G "Visual Studio 17 2022" -A x64 -B build

# Linux / macOS
cmake -DCMAKE_BUILD_TYPE=Release -B build

# 如果需要在 macOS 下交叉编译，可以指定 `CMAKE_OSX_ARCHITECTURES` 变量来指明目标系统架构
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64 -B build  # arm64
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=x86_64 -B build  # Intel-based
```

编译项目

```shell
# Windows MSVC 需要在构建阶段指定 --config Release
cmake --build build -j 8 --config Release

# Windows MinGW / Linux / macOS
cmake --build build -j 8
```

你可以在 `build` 文件夹下找到编译好的二进制文件，以及一个可供其它项目使用的动态库(Windows Only)，使用方法见仓库的 `example` 文件夹

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=taurusxin/ncmdump&type=Date)](https://star-history.com/#taurusxin/ncmdump&Date)
