git clone https://github.com/erincatto/box2d.git box2d
cd box2d
git checkout v3.1.1
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../box2d_installed
cmake --build build --config Release
cmake --install build