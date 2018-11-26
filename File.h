#ifndef __FILE_H__
#define __FILE_H__

#include "js32.h"

const char* readLine(FILE* fptr, bool locking);
bool writeValue(FILE* fptr, JSContext* cx, jsval value, bool isBinary, bool locking);
FILE* fileOpenRelScript(const wchar_t* filename, const wchar_t* mode, JSContext* cx);
FILE* fileOpenRelScript(const char* filename, const char* mode, JSContext* cx);
wchar_t* getPathRelScript(const wchar_t* filename, int bufLen, wchar_t* fullPath);
char* getPathRelScript(const char* filename, int bufLen, char* fullPath);
bool isValidPath(const wchar_t* name);
bool isValidPath(const char* name);

#endif
