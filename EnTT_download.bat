git clone https://github.com/skypjack/entt.git EnTT
cd EnTT
git checkout v3.16.0
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../EnTT_installed -DENTT_INSTALL=ON
cmake --build build --config Release
cmake --install build