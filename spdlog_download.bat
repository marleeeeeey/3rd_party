git clone https://github.com/gabime/spdlog.git spdlog
cd spdlog
git checkout v1.16.0

if exist build rmdir /s /q build
if exist ..\spdlog_installed rmdir /s /q ..\spdlog_installed

REM Building Release...
cmake -S . -B build/release -DCMAKE_INSTALL_PREFIX=../spdlog_installed -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --config Release
cmake --install build/release --config Release

REM Building Debug...
cmake -S . -B build/debug -DCMAKE_INSTALL_PREFIX=../spdlog_installed -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --config Debug
cmake --install build/debug --config Debug