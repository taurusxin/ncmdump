# ncmdump

使用本程序可将下载的网易云音乐缓存文件（ncm）转换为 mp3 或 flac 格式

## 简介

该版本为最早的 C++ 版本，也是作者开发的市面上第一个支持 ncm 转换的程序

源码复刻自 anonymous5l/ncmdump，感谢前辈的付出！
做了 Windows 下的移植，修复了一些编译问题

## 传送门

2021年10月6日，原作者已经删库

## 使用

你可以使用 Homebrew 来安装 ncmdump

```shell
brew install ncmdump
```

或者从 [Release](https://github.com/taurusxin/ncmdump/releases) 下载最新版本的对应系统的已编译好的二进制文件

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
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -B build --config Release

# Linux / macOS
cmake -DCMAKE_BUILD_TYPE=Release -B build
```

编译项目

```shell
cmake --build build --target all -j 8
```
