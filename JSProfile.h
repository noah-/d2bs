#ifndef JSPROFILE_H
#define JSPROFILE_H

#include <windows.h>
#include "js32.h"
#include "Profile.h"

CLASS_CTOR(profile);

JSAPI_FUNC(profile_login);

void profile_finalize(JSFreeOp *fop, JSObject *obj);

JSAPI_PROP(profile_getProperty);

struct jsProfilePrivate
{
	char* password;
};

enum jsProfileProperty_ids
{
	PROFILE_TYPE,
	PROFILE_IP,
	PROFILE_USERNAME,
	PROFILE_GATEWAY,
	PROFILE_CHARACTER,
	PROFILE_DIFFICULTY,
	PROFILE_MAXLOGINTIME,
	PROFILE_MAXCHARSELTIME
};

static JSPropertySpec profile_props[] = {
	{"type",			PROFILE_TYPE,		JSPROP_PERMANENT_VAR,	profile_getProperty},
	{"ip",				PROFILE_IP,			JSPROP_PERMANENT_VAR,	profile_getProperty},
	{"username",		PROFILE_USERNAME,	JSPROP_PERMANENT_VAR,	profile_getProperty},
	{"gateway",			PROFILE_GATEWAY,	JSPROP_PERMANENT_VAR,	profile_getProperty},
	{"character",		PROFILE_CHARACTER,	JSPROP_PERMANENT_VAR,	profile_getProperty},
	{"difficulty",		PROFILE_DIFFICULTY,	JSPROP_PERMANENT_VAR,	profile_getProperty},
	{"maxLoginTime",	PROFILE_MAXLOGINTIME,
											JSPROP_PERMANENT_VAR,	profile_getProperty},
	{"maxCharacterSelectTime",
						PROFILE_MAXCHARSELTIME,
											JSPROP_PERMANENT_VAR,	profile_getProperty},
	{0}
};

static JSFunctionSpec profile_methods[] = {
	{"login",			profile_login,		0},
	{0}
};


CLASS_CTOR(profileType);

JSAPI_PROP(profileType_getProperty);

static JSPropertySpec profileType_props[] = {
	{"singlePlayer",	PROFILETYPE_SINGLEPLAYER,	JSPROP_PERMANENT_VAR,	profileType_getProperty},
	{"battleNet",		PROFILETYPE_BATTLENET,		JSPROP_PERMANENT_VAR,	profileType_getProperty},
	{"openBattleNet",	PROFILETYPE_OPEN_BATTLENET,	JSPROP_PERMANENT_VAR,	profileType_getProperty},
	{"tcpIpHost",		PROFILETYPE_TCPIP_HOST,		JSPROP_PERMANENT_VAR,	profileType_getProperty},
	{"tcpIpJoin",		PROFILETYPE_TCPIP_JOIN,		JSPROP_PERMANENT_VAR,	profileType_getProperty},
	{0}
};

#endif