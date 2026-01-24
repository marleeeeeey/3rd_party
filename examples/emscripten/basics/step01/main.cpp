// Original example: https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

/*

-------------
--- BUILD ---
-------------

target_link_options(emscripten_basic_minimalProject PRIVATE
        "-sEXPORTED_FUNCTIONS=['_int_sqrt']"
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
)

IMPORTANT: Note that you need _ at the beginning of the function names in the EXPORTED_FUNCTIONS list.



-------------------------------------
HOW TO USE in BROWSER via JavaScript:
-------------------------------------

--- VAR 1 ---

int_sqrt = Module.cwrap('int_sqrt', 'number', ['number'])
int_sqrt(12)
int_sqrt(28)

--- VAR 2 ---

// Call C from JavaScript
var result = Module.ccall('int_sqrt', // name of C function
  'number', // return type
  ['number'], // argument types
  [28]); // arguments

 */

#include <math.h>

extern "C" {

int int_sqrt(int x) {
  return sqrt(x);
}
}