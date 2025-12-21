git clone https://github.com/gabime/spdlog.git spdlog
cd spdlog
git checkout v1.16.0

if exist build rmdir /s /q build
if exist ..\spdlog_installed rmdir /s /q ..\spdlog_installed

cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../spdlog_installed -DCMAKE_DEBUG_POSTFIX=d

cmake --build build --config Release
cmake --install build --config Release

cmake --build build --config Debug
cmake --install build --config Debug