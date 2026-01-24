/* https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

------------------------------
Calling JavaScript from C/C++
------------------------------

--- Var 1 ---

Use method `emscripten_run_script`

--- Var 2 ---

using EM_JS()

--- Var 3 ---

Using EM_ASM()

*/

#include <emscripten.h>

EM_JS(void, call_alert, (), {
  alert('hello world from C++! Var 02');
  // throw 'all done';
});

int main() {
  // --- Var 01 ---
  emscripten_run_script("alert('hello world from C++! Var 01')");

  // --- Var 02 ---
  call_alert();

  // --- Var 03 ---
  EM_ASM(
      alert('hello world from C++! Var 03');
      // throw 'all done';
  );
}