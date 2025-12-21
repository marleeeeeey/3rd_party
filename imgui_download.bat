if not exist SDL3_installed (
    echo Error: SDL3_installed not found. Please run SDL3_download.bat first.
    exit /b 1
)

git clone https://github.com/ocornut/imgui.git imgui
cd imgui
git checkout v1.92.5

if exist build rmdir /s /q build
if exist ..\imgui_installed rmdir /s /q ..\imgui_installed

copy ..\imgui_CMakeConfig.txt .\CMakeLists.txt

cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../imgui_installed
cmake --build build --config Release
cmake --install build

del CMakeLists.txt