#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main() {
  // 1. Create a 3D vector (x=1, y=0, z=0)
  glm::vec3 position = glm::vec3(1.0f, 0.0f, 0.0f);

  // 2. Create a rotation matrix (90 degrees around the Z axis)
  // We use glm::radians to convert degrees to radians
  glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // 3. Transform the position vector by the rotation matrix
  // Note: GLM matrices use column-major order, so we multiply: matrix * vector
  glm::vec3 rotated_position = glm::vec3(rotation * glm::vec4(position, 1.0f));

  // 4. Print the results to verify calculation
  std::cout << "Original position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
  std::cout << "Rotated position:  (" << rotated_position.x << ", " << rotated_position.y << ", " << rotated_position.z << ")" << std::endl;

  return 0;
}