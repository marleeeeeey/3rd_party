// Original example: https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

/*

Interacting with an API written in C/C++ from NodeJS

-------------------------------------
HOW TO USE in BROWSER via JavaScript:
-------------------------------------

Start index.html in browser from output folder.

 */

// api_example.c
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