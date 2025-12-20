# 3rd_party

Banch of scripts to download 3rd party libraries and make them installed as CMake targets.

### Motivation

I don't want to use any package manager.

### How to use

Every `download_*.bat` script downloads a library and installs it to a folder named `<>_installed`.

To use a library in your project, add the following line to your CMakeLists.txt (example for imgui):

```cmake
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../imgui_installed")
find_package(imgui REQUIRED)
```

### List of libraries

- box2d
- EnTT
- imgui
- SDL3

### List of minimal projects using these libraries

See these folders in this repository:

- box2d_minimalProject
- EnTT_minimalProject
- imgui_minimalProject
- SDL3_minimalProject
