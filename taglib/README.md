# taglib-ndk
Android NDK(Java) wrapper for [taglib](http://taglib.github.com/).

THIS IS EXPERIMENTAL PROJECT!!!! Not all APIs are wrapped and might be unstable.

## How to use in your App
You must have these dependencies installed on `Android SDK` before you start.

- Android NDK
- CMake

Also `Swig` is required to generate wrapper files.
- Swig(install this from apt or brew...or whatever you use)

### git clone this repository
`git clone` under `src/main/cpp/`.

### Generate Java/C++ wrapper files
```sh
cd taglib/swig
swig -java -c++ -package com.yourapp.taglib -outdir /(path_to_your_project_root)/app/src/main/java/com/yourapp/taglib -o swigout/swig_taglib_wrap.cpp taglib.i
```

### Write build settings
Open your `build.gradle` and add this.
```gradle
android {
    defaultConfig {
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++14 -frtti -fexceptions"
            }
        }
    }
    externalNativeBuild {
        cmake {
            path "src/main/cpp/taglib/CMakeLists.txt"
        }
    }
}
```

Hit `🔨build` on your Android Studio and make sure it builds successfully.

### Use taglib from Java/Kotlin code
#### Load libs before using
```kotlin
// make sure... it's not "libtag"(it will load liblibtag)
System.loadLibrary("tag")
```
### Load some Flac or MP3 and get tag
```kotlin
val fileRef = FileRef("/sdcard/01_girls_in_the_frontier (M@STER VERSION).flac")
val tag = fileRef.tag()

// get title and artist
val title = tag.title()
val artist = tag.artist()

// get additional tags
// check if the tag exists(accessing non exist item will SIGSEGV crash)
val comment = if (tag.properties().contains("COMMENT")) {
    tag.properties()["COMMENT"].front()
} else {
    ""
}

// get cover art(will return empty array when cover art does not exist)
val imageArray = fileRef.GetCoverArt()
val bitmap = BitmapFactory.decodeByteArray(imageArray, 0, imageArray.size)
```
