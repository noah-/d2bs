#ifndef PARTY_H
#define PARTY_H

#include "js32.h"

CLASS_CTOR(party);

JSAPI_PROP(party_getProperty);

JSAPI_FUNC(party_getNext);

JSAPI_FUNC(my_getParty);

enum party_tinyid {
	PARTY_AREA,
	PARTY_X,
	PARTY_Y,
	PARTY_GID,
	PARTY_LIFE,
	PARTY_NAME,
	PARTY_FLAG,
	PARTY_ID,
	PARTY_CLASSID,
	PARTY_LEVEL
};


static JSPropertySpec party_props[] = {
	{"x",			PARTY_X,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"y",			PARTY_Y,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"area",		PARTY_AREA,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"gid",			PARTY_GID,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"life",		PARTY_LIFE,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"partyflag",	PARTY_FLAG,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"partyid",		PARTY_ID,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"name",		PARTY_NAME,			JSPROP_PERMANENT_VAR,	party_getProperty},
	{"classid",		PARTY_CLASSID,		JSPROP_PERMANENT_VAR,	party_getProperty},
	{"level",		PARTY_LEVEL,		JSPROP_PERMANENT_VAR,	party_getProperty},
	{0},
};

static JSFunctionSpec party_methods[] = {
	{"getNext",			party_getNext,		0},
	{0},
};

#endif