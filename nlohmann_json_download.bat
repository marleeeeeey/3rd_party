git clone https://github.com/nlohmann/json.git nlohmann_json
cd nlohmann_json
git checkout v3.12.0

if exist build rmdir /s /q build
if exist ..\nlohmann_json_installed rmdir /s /q ..\nlohmann_json_installed

cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../nlohmann_json_installed -DCMAKE_DEBUG_POSTFIX=d

cmake --build build --config Release
cmake --install build --config Release

cmake --build build --config Debug
cmake --install build --config Debug

