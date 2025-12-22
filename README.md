# 3rd_party

A script to download 3rd party libraries and make them installed as CMake targets.

### Motivation

I wanted to control the library build process to simplify cross-platform compilation.

### How to use

Run `download_all.bat` and expect the libraries to be installed in `external_install` folder.

Cross-platform variant (alternative to `download_all.bat`):

```shell
cmake -P download_all.cmake
```

To use a library in your project, add the following line to your CMakeLists.txt (example for box2d):

```cmake
list(APPEND CMAKE_PREFIX_PATH "../../external_install")
find_package(box2d REQUIRED)
target_link_libraries(box2d_minimalProject PRIVATE box2d::box2d)
```

#### Rebuild existing libraries

```shell
cmake -DFORCE_REBUILD=TRUE -P download_all.cmake
```

### List of CMake packages

- box2d
- EnTT
- glm
- GTest
- imgui
- magic_enum
- nlohmann_json
- SDL3
- spdlog