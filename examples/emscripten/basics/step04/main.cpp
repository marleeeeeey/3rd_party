/* https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html

--------------------------------------------------------------
Embind is used to bind C++ functions and classes to JavaScript
--------------------------------------------------------------

1. CODE
-------

extern "C" is not needed for embind

2. LINK
-------

Option --bind is required for embind.

target_link_options(emscripten_basic_step04_minimalProject PRIVATE
        "--bind"
)

3. RUN IN JS
------------

Open index.html in a browser to see the output.
*/

#include <emscripten/bind.h>

using namespace emscripten;

// Exposing methods to JavaScript
// ------------------------------

float lerp(float a, float b, float t) {
  return (1 - t) * a + t * b;
}

EMSCRIPTEN_BINDINGS(my_module) {
  function("lerp", &lerp);
}

// Exposing classes to JavaScript
// ------------------------------

class MyClass {
 public:
  MyClass(int x, std::string y)
      : x(x), y(y) {}

  void incrementX() {
    ++x;
  }

  int getX() const { return x; }

  void setX(int x_) { x = x_; }

  static std::string getStringFromInstance(const MyClass& instance) {
    return instance.y;
  }

 private:
  int x;
  std::string y;
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<MyClass>("MyClass")
      .constructor<int, std::string>()
      .function("incrementX", &MyClass::incrementX)
      .property("x", &MyClass::getX, &MyClass::setX)
      .property("x_readonly", &MyClass::getX)
      .class_function("getStringFromInstance", &MyClass::getStringFromInstance);
}