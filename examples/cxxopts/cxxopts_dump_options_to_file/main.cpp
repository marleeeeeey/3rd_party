#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, const char* argv[]) {
  std::filesystem::path exePath = std::filesystem::absolute(argv[0]);
  std::filesystem::path logPath = exePath.parent_path() / "options.txt";
  std::ofstream ofs(logPath, std::ios::app);

  auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  ofs << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << " ";

  ofs << "Full launch string: ";
  for (int i = 0; i < argc; ++i) {
    ofs << "\"" << argv[i] << "\" ";
  }
  ofs << std::endl;

  return 0;
}
