#ifndef __FILE_H__
#define __FILE_H__

#include "js32.h"

const char* readLine(FILE* fptr, bool locking);
bool writeValue(FILE* fptr, JSContext* cx, jsval value, bool isBinary, bool locking);
bool isValidPath(const char* name);

#endif
