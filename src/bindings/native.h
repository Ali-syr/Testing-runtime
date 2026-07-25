#ifndef NATIVE_H
#define NATIVE_H

#include "jsapi.h"

//implementation of the JavaScript print() function.
bool js_print(JSContext* cx, unsigned argc, JS::Value* vp);

bool js_readFile(JSContext* cx, unsigned argc, JS::Value* vp);

// List of native functions to be exposed to the global JavaScript scope.
extern const JSFunctionSpec global_functions[];

#endif
