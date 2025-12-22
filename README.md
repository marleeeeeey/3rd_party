# 3rd_party

A script to download 3rd party libraries and make them installed as CMake targets.

### Motivation

I wanted to control the library build process to simplify cross-platform compilation.

### How to build 3rd party libraries (download and install)

```shell
cmake -P download_all.cmake
```

Rebuild existing libraries

```shell
cmake -DFORCE_REBUILD=TRUE -P download_all.cmake
```

Rebuild all examples even if they were already built

```shell
cmake -DREBUILD_EXAMPLE_PROJECTS=TRUE -P download_all.cmake
```

### How to use libraries in your project

Add the following line to your CMakeLists.txt (example for box2d):

```cmake
list(APPEND CMAKE_PREFIX_PATH "../../external_install")
find_package(box2d REQUIRED)
target_link_libraries(box2d_minimalProject PRIVATE box2d::box2d)
```

### List of CMake packages

- box2d
- EnTT
- glm
- GTest
- imgui
- implot
- magic_enum
- nlohmann_json
- OpenAL
- SDL3
- SDL3_image
- spdlog