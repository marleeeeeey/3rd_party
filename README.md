# 3rd_party

A bunch of scripts to download 3rd party libraries and make them installed as CMake targets.

### Motivation

I don't want to use any package manager.

### How to use

Every `<library>_download.bat` script downloads a library and installs it to a folder named `<library>_installed`.

To use a library in your project, add the following line to your CMakeLists.txt (example for box2d):

```cmake
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../box2d_installed")
find_package(box2d REQUIRED)
```

### List of libraries

- box2d
- EnTT
- imgui
- SDL3
- spdlog

Every library has a `<library>_minimalProject` folder that contains a minimal CMake project that uses the library.

### How to add a new library

Copy and update `box2d_download.bat` because it contains common structure for all modern libraries that are using CMake. 