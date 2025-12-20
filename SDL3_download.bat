git clone https://github.com/libsdl-org/SDL.git SDL3
cd SDL3
git checkout release-3.2.28
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../SDL3_installed
cmake --build build --config Release
cmake --install build