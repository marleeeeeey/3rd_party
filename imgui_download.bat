git clone https://github.com/ocornut/imgui.git imgui
cd imgui
git checkout v1.92.5
copy ..\imgui_CMakeConfig.txt .\CMakeLists.txt
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=../imgui_installed
cmake --build build --config Release
cmake --install build