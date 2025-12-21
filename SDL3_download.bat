git clone https://github.com/libsdl-org/SDL.git SDL3
cd SDL3
git checkout release-3.2.28

if exist build rmdir /s /q build
if exist ..\SDL3_installed rmdir /s /q ..\SDL3_installed

cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../SDL3_installed -DCMAKE_DEBUG_POSTFIX=d

cmake --build build --config Release
cmake --install build --config Release

cmake --build build --config Debug
cmake --install build --config Debug