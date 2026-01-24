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

#include <cstdio>

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

  // --------------

  // send values from C into JavaScript
  EM_ASM({ alert('I received: ' + $0); }, 100);

  // receive values back from JS to C++
  int x = EM_ASM_INT({
    console.log('I received: ' + $0);
    return $0 + 1; }, 100);
  std::printf("%d\n", x);  // print to browser console

  return 0;
}