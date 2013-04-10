#ifndef __JSFILE_H__
#define __JSFILE_H__

#include "js32.h"

//////////////////////////////////////////////////////////////////
// file stuff
//////////////////////////////////////////////////////////////////

CLASS_CTOR(file);
JSBool file_equality(JSContext *cx, JSObject *obj, jsval v, JSBool *bp);

JSAPI_PROP(file_getProperty);
JSAPI_STRICT_PROP(file_setProperty);

JSAPI_FUNC(file_open);
JSAPI_FUNC(file_close);
JSAPI_FUNC(file_reopen);
JSAPI_FUNC(file_read);
JSAPI_FUNC(file_readLine);
JSAPI_FUNC(file_readAllLines);
JSAPI_FUNC(file_readAll);
JSAPI_FUNC(file_write);
JSAPI_FUNC(file_seek);
JSAPI_FUNC(file_flush);
JSAPI_FUNC(file_reset);
JSAPI_FUNC(file_end);

void file_finalize(JSFreeOp *fop, JSObject *obj);

/**
File object:

File File.open(string path, mode [, bool binaryMode [, bool autoflush [, bool lockFile]]]) - static - open the specified file, return a File object, mode = one of the constants of the File object listed below, binaryMode = default setting of file.binaryMode, autoflush = default setting of file.autoflush, lockFile = if true lock the file on open and unlock it on close, so other threads/programs can't open it

File file.close() - object - close the current file
File file.reopen() - object - reopen the current file
string/int file.read(int num) - object - read num amount of characters or bytes from the file, if in non-binary mode this will be null terminated
string file.readLine() - object - read a single line from the file, assuming the line ends with \n or \r\n, throws an exception if the file is in binary mode
array file.readAllLines() - object - read the full contents of the file and split it up into an array of lines, and return the array of lines, throws an exception if the file is in binary mode
string/array file.readAll() - object - read the full contents of the file and return it as one big string or if in binary mode, array
File file.write(object contents, ...) - object - write the specified byte/strings/objects/array-of-bytes to a file, throws an exception if not all of the parameters could be written to disk
File file.seek(int bytes [, bool isLines [, bool fromStart]]) - object - seek the specified number of bytes, or optionally lines, in the file, optionally from the start of the file, stops at the end of the file
File file.flush() - object - flushes the remaining buffer to disk
File file.reset() - object - seek to the beginning of the file
File file.end() - object - seek to the end of the file
**/

static JSFunctionSpec file_methods[] = {
	JS_FN("close",		file_close,			0, FUNCTION_FLAGS),
	JS_FN("reopen",		file_reopen,		0, FUNCTION_FLAGS),
	JS_FN("read",		file_read,			1, FUNCTION_FLAGS),
	JS_FN("readLine",	file_readLine,		0, FUNCTION_FLAGS),
	JS_FN("readAllLines",file_readAllLines,	0, FUNCTION_FLAGS),
	JS_FN("readAll",		file_readAll,		0, FUNCTION_FLAGS),
	JS_FN("write",		file_write,			1, FUNCTION_FLAGS),
	JS_FN("seek",		file_seek,			1, FUNCTION_FLAGS),
	JS_FN("flush",		file_flush,			0, FUNCTION_FLAGS),
	JS_FN("reset",		file_reset,			0, FUNCTION_FLAGS),
	JS_FN("end",			file_end,			0, FUNCTION_FLAGS),
	JS_FS_END
};

static JSFunctionSpec file_s_methods[] = {
	JS_FN("open",		file_open,		2, FUNCTION_FLAGS),
	JS_FS_END
};

// ensure that read/write/append get the correct values
enum {
	FILE_READ = 0,
	FILE_WRITE = 1,
	FILE_APPEND = 2,
	FILE_READABLE,
	FILE_WRITEABLE,
	FILE_SEEKABLE,
	FILE_MODE,
	FILE_LENGTH,
	FILE_PATH,
	FILE_POSITION,
	FILE_EOF,
	FILE_AUTOFLUSH,
	FILE_BINARYMODE,
	FILE_ACCESSED,
	FILE_CREATED,
	FILE_MODIFIED,
};

/**
bool file.readable - object - can read from the file
bool file.writable - object - can write from the file
bool file.seekable - object - can seek within the file
int file.mode - object - the mode the file was opened in, one of the FILE_MODE constants of the file object
bool file.binaryMode - object - determines if the file is in binary mode for read operations (write operations happen based on the parameter specified)
bool file.autoflush - object - automatically flush the file buffer after each write, defaults to off
int file.length - object - the length of the file in bytes
string file.path - object - the name and path (relative to the scripts/ folder) of the file
int file.position - object - the current position in the file
bool file.eof - object - determines if the file is at end-of-file or not
**/

static JSPropertySpec file_props[] = {
	{"readable",	FILE_READABLE,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"writeable",	FILE_WRITEABLE,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"seekable",	FILE_SEEKABLE,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"mode",		FILE_MODE,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"binaryMode",	FILE_BINARYMODE,JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"length",		FILE_LENGTH,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"path",		FILE_PATH,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"position",	FILE_POSITION,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"eof",			FILE_EOF,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"accessed",	FILE_ACCESSED,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"created",		FILE_CREATED,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"modified",	FILE_MODIFIED,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(file_getProperty), JSOP_NULLWRAPPER },
	{"autoflush",	FILE_AUTOFLUSH,	JSPROP_STATIC_VAR,		JSOP_WRAPPER(file_getProperty),	JSOP_WRAPPER(file_setProperty)},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

#endif
