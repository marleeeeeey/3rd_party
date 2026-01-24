/* https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

----------------------------------------------------------
Interacting with an API (MODULE) written in C/C++ from JS
----------------------------------------------------------

Start index.html in browser from output folder.

*/

#include <emscripten.h>
#include <stdio.h>

extern "C" {

EMSCRIPTEN_KEEPALIVE
void sayHi() {
  printf("Hi!\n");
}

EMSCRIPTEN_KEEPALIVE
int daysInWeek() {
  return 7;
}
}