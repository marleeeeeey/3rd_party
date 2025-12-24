#include <iostream>
#include <magic_enum/magic_enum.hpp>

enum class Color { Red,
                   Green,
                   Blue };

int main() {
  Color c = Color::Green;

  // 1. Convert enum value to string
  auto color_name = magic_enum::enum_name(c);

  // 2. Convert string back to enum value
  auto color_opt = magic_enum::enum_cast<Color>("Blue");

  // 3. Print results
  std::cout << "Enum to string: " << color_name << std::endl;

  if (color_opt.has_value()) {
    std::cout << "String to enum: Success (value is " << static_cast<int>(color_opt.value()) << ")" << std::endl;
  }

  // 4. Get all enum names as a list
  std::cout << "All colors: ";
  for (auto name : magic_enum::enum_names<Color>()) {
    std::cout << name << " ";
  }
  std::cout << std::endl;

  return 0;
}
