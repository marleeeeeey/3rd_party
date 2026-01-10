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

### SSL/TLS Certificates which are used in the example projects

The project uses self-signed certificate for testing purposes. Passphrase is `123Qwe!`.

```shell
./examples/server.crt
./examples/server.key
```

Certificate is generated in the `examples` folder by CMake configure step. See method `generate_test_certificates` in
`ExternalDependencies.cmake`.

Later this certificate is used in the example projects by copying them to the output folder.

Equivalent command to generate certificates on Windows (powershell):

```shell
$env:OPENSSL_CONF = "$PWD/external_install/Debug/openssl/ssl/openssl.cnf"
& "external_install/Debug/openssl/x64/bin/openssl.exe" req -x509 -newkey rsa:2048 -keyout "examples/server.key" -out "examples/server.crt" -days 365 -subj "/CN=localhost"
```

To copy certificates to the `examples` build folder for a specific project, update `CMakeLists.txt`:

```shell
add_custom_command(TARGET YOUR_PROJECT_NAME_HERE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../server.crt"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../server.key"
        $<TARGET_FILE_DIR:YOUR_PROJECT_NAME_HERE>
        COMMENT "WARNING: Copying tests SSL certificates to output directory"
)
```

Please make shure that you use a relative path from the `CMAKE_CURRENT_SOURCE_DIR` variable.

### Debug SSL/TLS Handshake

```shell
./external_install/Debug/openssl/x64/bin/openssl.exe s_client -connect 127.0.0.1:12345 -debug -state
```