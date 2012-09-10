#ifndef SQLITE_H
#define SQLITE_H

#include "js32.h"
#include "sqlite3.h"
#include "JSGlobalClasses.h"

JSAPI_FUNC(my_sqlite_version);
JSAPI_FUNC(my_sqlite_memusage);

JSAPI_FUNC(sqlite_ctor);
JSAPI_FUNC(sqlite_execute);
JSAPI_FUNC(sqlite_query);
JSAPI_FUNC(sqlite_close);
JSAPI_FUNC(sqlite_open);
JSAPI_PROP(sqlite_getProperty);
void sqlite_finalize(JSFreeOp *fop, JSObject *obj);
JSBool sqlite_equal(JSContext *cx, JSObject *obj, jsval v, JSBool *bp);

CLASS_CTOR(sqlite_stmt);

JSAPI_FUNC(sqlite_stmt_getobject);
JSAPI_FUNC(sqlite_stmt_colcount);
JSAPI_FUNC(sqlite_stmt_colval);
JSAPI_FUNC(sqlite_stmt_colname);
JSAPI_FUNC(sqlite_stmt_execute);
JSAPI_FUNC(sqlite_stmt_next);
JSAPI_FUNC(sqlite_stmt_reset);
JSAPI_FUNC(sqlite_stmt_skip);
JSAPI_FUNC(sqlite_stmt_close);
JSAPI_FUNC(sqlite_stmt_bind);
JSAPI_PROP(sqlite_stmt_getProperty);
void sqlite_stmt_finalize(JSFreeOp *fop, JSObject *obj);

enum {
	SQLITE_PATH,
	SQLITE_STMTS,
	SQLITE_OPEN,
	SQLITE_LASTROWID,
	SQLITE_CHANGES
};

enum {
	SQLITE_STMT_SQL,
	SQLITE_STMT_READY
};


static JSFunctionSpec sqlite_methods[] = {
	{"execute",	sqlite_execute,	1},
	{"query",	sqlite_query,	1},
	{"open",	sqlite_open,	0},
	{"close",	sqlite_close,	0},
	{0}
};

static JSPropertySpec sqlite_props[] = {
	{"path",		SQLITE_PATH,		JSPROP_PERMANENT_VAR,	sqlite_getProperty},
	{"statements",	SQLITE_STMTS,		JSPROP_PERMANENT_VAR,	sqlite_getProperty},
	{"isOpen",		SQLITE_OPEN,		JSPROP_PERMANENT_VAR,	sqlite_getProperty},
	{"lastRowId",	SQLITE_LASTROWID,	JSPROP_PERMANENT_VAR,	sqlite_getProperty},
	{"changes",		SQLITE_CHANGES,		JSPROP_PERMANENT_VAR,	sqlite_getProperty},
	{0}
};

static JSFunctionSpec sqlite_stmt_methods[] = {
	{"getObject",		sqlite_stmt_getobject,	0},
	{"getColumnCount",	sqlite_stmt_colcount,	0},
	{"getColumnValue",	sqlite_stmt_colval,		1},
	{"getColumnName",	sqlite_stmt_colname,	1},
	{"go",				sqlite_stmt_execute,	0},
	{"next",			sqlite_stmt_next,		0},
	{"skip",			sqlite_stmt_skip,		1},
	{"reset",			sqlite_stmt_reset,		0},
	{"close",			sqlite_stmt_close,		0},
	{"bind",			sqlite_stmt_bind,		2},
	{0}
};

static JSPropertySpec sqlite_stmt_props[] = {
	{"sql",		SQLITE_STMT_SQL,	JSPROP_PERMANENT_VAR,	sqlite_stmt_getProperty},
	{"ready",	SQLITE_STMT_READY,	JSPROP_PERMANENT_VAR,	sqlite_stmt_getProperty},
	{0}
};

#endif SQLITE_H
