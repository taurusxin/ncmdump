# ncmdump

使用本程序可将下载的网易云音乐缓存文件（ncm）转换为 mp3 或 flac 格式

## 简介

该版本为最早的 C++ 版本，也是作者开发的市面上第一个支持 ncm 转换的程序

源码复刻自 anonymous5l/ncmdump，感谢前辈的付出！
做了 Windows 下的移植，修复了一些编译问题

## 传送门

2021年10月6日，原作者已经删库

## 使用

从 [Release](https://github.com/taurusxin/ncmdump/releases) 下载最新版本的对应系统的已编译好的二进制文件

使用 `-h` 或 `--help` 参数来打印帮助

```shell
ncmdump -h
```

命令行下输入一个或多个文件

```shell
ncmdump file1 file2...
```

你可以使用 `-d` 参数来指定一个文件夹，对文件夹下的所有文件批量处理

```shell
ncmdump -d folder
```

## 依赖库及不同平台编译方法

### 依赖库

* taglib

### Linux / macOS

```shell
# CentOS
yum install taglib-devel
make linux

# Ubuntu / Debian
apt install libtag1-dev
make linux

# macOS with Intel chip
brew install taglib
make macos-intel

# macOS with Apple Silicon (M1 / M2)
brew install taglib
make macos-arm64
```

注意：Linux / macOS 为动态库支持，Linux 下静态编译需要手动编译 taglib 静态库，macOS 原生库不支持静态链接

### Windows

#### MinGW

~~因为部分代码（例如常量引用等）在 msvc 下面编译不通过，本项目需要使用 MinGW 编译~~
已经修改部分代码使得支持 msvc 编译，详见下面的 Visual Studio 部分

下载 mingw-w64 的 Windows 版本，这里推荐从 [winlibs](https://winlibs.com/) 这个网站下载，编译器版本始终跟随上游保持最新

将 mingw64/bin 添加到系统环境变量

taglib 在 Windows 下已经编译好放在项目内，直接执行

```shell
mingw32-make win32
```

生成的二进制程序为静态链接版本，可脱离运行库运行在裸机上

#### msys2

```shell
pacman -Syu
git clone https://github.com/taurusxin/ncmdump && cd ncmdump
make win32
```

#### Visual Studio

注意**VS的源代码还未上传，请先使用 MinGW 编译**

使用 git 切换到 msvc 分支，使用 Visual Studio 打开 ncmdump.sln，编译即可
