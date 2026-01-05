# 3rd_party

A script to download 3rd party libraries and make them installed as CMake targets.

### Motivation

I wanted to control the library build process to simplify cross-platform compilation.

### How to build 3rd party libraries (download and install)

Download and install OpenSSL to the system using any method to make it available to CMake.
For example, for Windows machine I download and install MSI package from https://slproweb.com/products/Win32OpenSSL.html.

```shell
cmake -P download_all.cmake
```

Rebuild existing libraries

```shell
cmake -DFORCE_REBUILD=TRUE -P download_all.cmake
```

Build libraries and examples

```shell
cmake -DBUILD_EXAMPLES=TRUE -P download_all.cmake
```

### How to use libraries in your project

Add the following line to your CMakeLists.txt (example for box2d):

```cmake
list(APPEND CMAKE_PREFIX_PATH "../../external_install")
find_package(box2d REQUIRED)
target_link_libraries(box2d_minimalProject PRIVATE box2d::box2d)
```

### List of CMake packages

- asio
- box2d
- cxxopts
- enet
- EnTT
- flatbuffers
- glm
- GTest
- imgui
- implot
- magic_enum
- miniaudio
- nlohmann_json
- OpenAL
- openssl
- SDL3
- SDL3_image
- spdlog
- Tracy