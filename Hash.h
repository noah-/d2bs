#pragma once
#pragma comment (lib, "Crypt32")

#include <windows.h>
#include <wincrypt.h>

char* HashString(char* string, ALG_ID algo);
char* HashFile(char* file, ALG_ID algo);
char* md5(char* string);
char* sha1(char* string);
char* sha256(char* string);
char* sha384(char* string);
char* sha512(char* string);
char* md5_file(char* string);
char* sha1_file(char* string);
char* sha256_file(char* string);
char* sha384_file(char* string);
char* sha512_file(char* string);
