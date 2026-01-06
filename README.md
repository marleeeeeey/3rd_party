# 3rd_party

A script to download 3rd party libraries and make them installed as CMake targets.

### Motivation

My desire is to control 3rd party library versions and library build process.

### How to build 3rd party libraries

The project is designed to be configured as a standard CMake project.
When you configure the project (e.g., by running `cmake -B build`), it will automatically download, build, and install all dependencies into
the `external_install` folder (hardcoded). `-DCMAKE_BUILD_TYPE=Release` uses by default.

```shell
cmake -B cmake-build-release
```

#### Result

Command will generate the file structure mentioned below. Every folder like "asio", "box2d" is CMake package.

```
3rd_party/external_install/Release
  /asio
  /box2d
  /cxxopts
  /enet
  /EnTT
  /flatbuffers
  /glm
  /GTest
  /imgui
  /implot
  /magic_enum
  /miniaudio
  /nlohmann_json
  /OpenAL
  /openssl
  /SDL3
  /SDL3_image
  /spdlog
  /Tracy
  /uSockets
  /uWebSockets
```

### How to use libraries in your project

To use these libraries in your own CMake project, you need to point `CMAKE_PREFIX_PATH` to the configuration-specific installation folder.

```cmake

# Add paths to CMAKE_PREFIX_PATH so find_package can locate them
list(APPEND CMAKE_PREFIX_PATH "/you/path/to/3rd_party/external_install/Release")
list(APPEND CMAKE_PREFIX_PATH "/you/path/to/3rd_party/external_install/Release/openssl/x64") # (Optional hack for openssl on Windows)
list(APPEND CMAKE_PREFIX_PATH "/you/path/to/3rd_party/external_install/Release/OpenAL")      # (Optional hack for OpenAL on Windows)

# Common way to use libraries
find_package(box2d REQUIRED)
target_link_libraries(box2d_minimalProject PRIVATE box2d::box2d)

# Example of how to use header only libraries
set(MINIAUDIO_INCLUDE_DIR "../../external_install/${CMAKE_BUILD_TYPE}/miniaudio/include")
target_include_directories(miniaudio_minimalProject PRIVATE ${MINIAUDIO_INCLUDE_DIR})
```

### Technical Details

For every 3rd party library an example project is present. Building of example projects is enabled by default (`-DBUILD_EXAMPLES=TRUE`) 
but it may be optional. A successful build of example projects indicates that the library is properly integrated and functional.

Please see every example project for more details about the library usage.

### All Specific Build Options

- `-DFORCE_REBUILD=TRUE`: Wipe `external_build` and `external_install` and rebuild everything from scratch.
- `-DBUILD_EXAMPLES=TRUE`: Build smoke tests/examples located in the `examples` folder.
