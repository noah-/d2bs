#ifndef __JSDIRECTORY_H__
#define __JSDIRECTORY_H__

// TODO: Rewrite this mess of crap too

#include "js32.h"
#include <cstdlib>
#include <cstring>

CLASS_CTOR(dir);

JSAPI_FUNC(dir_getFiles);
JSAPI_FUNC(dir_getFolders);
JSAPI_FUNC(dir_create);
JSAPI_FUNC(dir_delete);
JSAPI_FUNC(my_openDir);

JSAPI_PROP(dir_getProperty);

void dir_finalize(JSContext *cx, JSObject *obj);


//////////////////////////////////////////////////////////////////
// directory stuff
//////////////////////////////////////////////////////////////////

enum {DIR_NAME};

static JSPropertySpec dir_props[] = {
	{"name",	DIR_NAME,	JSPROP_PERMANENT_VAR,	dir_getProperty},
	{0}
};

static JSFunctionSpec dir_methods[] = {
	{"create",			dir_create,			1},
	{"remove",			dir_delete,			1},
	{"getFiles",		dir_getFiles,		1},
	{"getFolders",		dir_getFolders,		1},
	{0}
};

class DirData {
public:
	char name[_MAX_FNAME];
	DirData(char* newname)
	{
		strcpy_s(name, _MAX_FNAME, newname);
	}
};

#endif
