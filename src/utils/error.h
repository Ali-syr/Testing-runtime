#ifndef ERROR_H
#define ERROR_H

#include "jsapi.h"

void reportPendingException(JSContext* cx);

#endif
