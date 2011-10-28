#ifndef __FILETOOLS_H__
#define __FILETOOLS_H__

#include "js32.h"

/**
These are non-atomic operations:

FileTools.remove(string name) - static - remove a file based on name
FileTools.rename(string oldName, string newName) - static - rename a file
FileTools.copy(string original, string copy) - static - copy file 'original' to 'copy'
FileTools.exists(string path) - static - determines if a file exists or not

These are atomic operations:

string FileTools.readText(string path) - static - open a file in read mode, read the full contents, return it as a string
bool FileTools.writeText(string path, object contents, ...) - static - open a file in write mode, write the content parameters into the file, and close it, true if the write succeeded, false if not
bool FileTools.appendText(string path, string contents) - static - open a file in append mode, write contents into the file, close it, true if the write succeeded, false if not
**/

CLASS_CTOR(filetools);

JSAPI_FUNC(filetools_remove);
JSAPI_FUNC(filetools_rename);
JSAPI_FUNC(filetools_copy);
JSAPI_FUNC(filetools_exists);
JSAPI_FUNC(filetools_readText);
JSAPI_FUNC(filetools_writeText);
JSAPI_FUNC(filetools_appendText);

static JSFunctionSpec filetools_s_methods[] = {
	{"remove",		filetools_remove,		1},
	{"rename",		filetools_rename,		2},
	{"copy",		filetools_copy,			2},
	{"exists",		filetools_exists,		1},
	{"readText",	filetools_readText,		1},
	{"writeText",	filetools_writeText,	2},
	{"appendText",	filetools_appendText,	2},
	{0}
};

#endif