#ifndef __FILE_H__
#define __FILE_H__

#include "js32.h"

char* readLine(FILE* fptr, bool locking);
bool writeValue(FILE* fptr, JSContext* cx, jsval value, bool isBinary, bool locking);
FILE* fileOpenRelScript(const wchar_t* filename, const wchar_t* mode, JSContext* cx);
wchar_t* getPathRelScript(const wchar_t* filename, int bufLen, wchar_t* fullPath);
bool isValidPath(const wchar_t* name);

#endif
