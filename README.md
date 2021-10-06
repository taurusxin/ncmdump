## 简介
该版本为最早的 C++ 版本，也是作者开发的市面上第一个支持 ncm 转换的程序

源码复刻自 anonymous5l/ncmdump，感谢前辈的付出！
做了 Windows 下的移植，修复了一些编译问题

## 便捷式传送门
2021年10月6日，原作者已经删库

## 依赖库及编译方法
#### 依赖库
* taglib

### Linux / macOS
```shell
# CentOS
yum install taglib-devel
make linux

# Ubuntu / Debian
apt install libtag1-dev
make linux

# macOS
brew install taglib
make macos
```

注意：Linux / macOS 为动态库支持，Linux 下静态编译需要手动编译 taglib 静态库，macOS 原生库不支持静态链接

### Windows

因为部分代码（例如常量引用等）在 msvc 下面编译不通过，本项目需要使用 MinGW 编译

下载 [mingw-w64](https://winlibs.com/) 的 Windows 版本，这里推荐从 winlibs 这个网站下载，编译器版本始终保持最新

将 mingw64/bin 添加到系统环境变量

taglib 在 Windows 下已经编译好放在项目内，直接执行

```shell
mingw32-make win32
```

生成的二进制程序为静态链接版本，可脱离运行库运行在裸机上

## 使用
1. 命令行下使用 `ncmdump [files]...`

2. 直接 ncm 拖拽文件到二进制文件上
