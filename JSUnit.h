#pragma once

#include <windows.h>
#include "js32.h"

CLASS_CTOR(unit);

JSAPI_FUNC(unit_getUnit);
JSAPI_FUNC(unit_getNext);
JSAPI_FUNC(unit_cancel);
JSAPI_FUNC(unit_repair);
JSAPI_FUNC(unit_useMenu);
JSAPI_FUNC(unit_interact);
JSAPI_FUNC(unit_getStat);
JSAPI_FUNC(unit_getState);
JSAPI_FUNC(unit_getItems);
JSAPI_FUNC(unit_getSkill);
JSAPI_FUNC(unit_getParent);
JSAPI_FUNC(unit_setskill);
JSAPI_FUNC(unit_getMerc);
JSAPI_FUNC(unit_getMercHP);
JSAPI_FUNC(unit_getItem);
JSAPI_FUNC(unit_move);
JSAPI_FUNC(item_getFlag);
JSAPI_FUNC(item_getFlags);
//JSAPI_FUNC(item_getPrice);
JSAPI_FUNC(item_shop);
JSAPI_FUNC(my_overhead);
JSAPI_FUNC(my_revive);
JSAPI_FUNC(unit_getEnchant);
JSAPI_FUNC(unit_getQuest);
JSAPI_FUNC(unit_getMinionCount);
JSAPI_FUNC(me_getRepairCost);
JSAPI_FUNC(item_getItemCost);

void unit_finalize(JSContext *cx, JSObject *obj);
JSAPI_PROP(unit_getProperty);
JSAPI_STRICT_PROP(unit_setProperty);
JSBool unit_equal(JSContext *cx, JSObject *obj, jsval v, JSBool *bp);

struct myUnit
{
	DWORD _dwPrivateType;
	DWORD dwUnitId;
	DWORD dwClassId;
	DWORD dwType;
	DWORD dwMode;
	char szName[128];
};

struct invUnit
{
	DWORD _dwPrivateType;
	DWORD dwUnitId;
	DWORD dwClassId;
	DWORD dwType;
	DWORD dwMode;
	char szName[128];
	DWORD dwOwnerId;
	DWORD dwOwnerType;
};

enum unit_tinyid
{
	UNIT_TYPE, UNIT_CLASSID, UNIT_ID, UNIT_MODE, UNIT_NAME, UNIT_BUSY, 
	UNIT_ACT, UNIT_XPOS, UNIT_YPOS, ME_WSWITCH, UNIT_AREA, UNIT_HP, 
	UNIT_HPMAX, UNIT_MP, UNIT_MPMAX, UNIT_STAMINA, UNIT_STAMINAMAX, 
	UNIT_CHARLVL, ME_RUNWALK, ITEM_CODE, ITEM_PREFIX, ITEM_SUFFIX, ITEM_FNAME, 
	ITEM_QUALITY, ITEM_NODE, UNIT_SELECTABLE, ITEM_LOC, ITEM_SIZEX, 
	ITEM_SIZEY, ITEM_TYPE, MISSILE_DIR, MISSILE_VEL, ITEM_CLASS, 
	UNIT_SPECTYPE, ITEM_DESC, ITEM_BODYLOCATION, UNIT_ITEMCOUNT, ITEM_LEVELREQ,
	UNIT_OWNER, UNIT_OWNERTYPE, UNIT_UNIQUEID, ITEM_LEVEL, UNIT_DIRECTION, 
	ITEM_SUFFIXNUM, ITEM_PREFIXNUM, ITEM_PREFIXES, ITEM_SUFFIXES, ITEM_SUFFIXNUMS, ITEM_PREFIXNUMS, ITEM_GFX,
	OBJECT_TYPE, OBJECT_LOCKED
};

enum me_tinyid {
	ME_ACCOUNT = 90,
	ME_CHARNAME,
	ME_CHICKENHP,
	ME_CHICKENMP,
	ME_DIFF,
	ME_MAXDIFF,
	ME_GAMENAME,
	ME_GAMEPASSWORD,
	ME_GAMESERVERIP,
	ME_GAMESTARTTIME,
	ME_GAMETYPE,
	ME_ITEMONCURSOR,
	ME_LADDER,
	ME_PING,
	ME_FPS,
	ME_PLAYERTYPE,
	ME_QUITONHOSTILE,
	ME_REALM,
	ME_REALMSHORT,
	ME_MERCREVIVECOST,
	ME_MAPID,
	OOG_WINDOWTITLE,
	OOG_SCREENSIZE,
	OOG_INGAME,
	OOG_QUITONERROR,
	OOG_MAXGAMETIME,
	ME_BLOCKKEYS,
	ME_BLOCKMOUSE,
	ME_GAMEREADY,
	ME_PROFILE,
	ME_NOPICKUP,
	ME_PID
};

static JSPropertySpec me_props[] = {
	{"account",			ME_ACCOUNT,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"charname",		ME_CHARNAME,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"diff",			ME_DIFF,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"maxdiff",			ME_MAXDIFF,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gamename",		ME_GAMENAME,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gamepassword",	ME_GAMEPASSWORD,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gameserverip",	ME_GAMESERVERIP,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gamestarttime",	ME_GAMESTARTTIME,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gametype",		ME_GAMETYPE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"itemoncursor",	ME_ITEMONCURSOR,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"ladder",			ME_LADDER,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"ping",			ME_PING,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"fps",				ME_FPS,				JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"playertype",		ME_PLAYERTYPE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"realm",			ME_REALM,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"realmshort",		ME_REALMSHORT,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mercrevivecost",	ME_MERCREVIVECOST,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"runwalk",			ME_RUNWALK,			JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"weaponswitch",	ME_WSWITCH,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"chickenhp",		ME_CHICKENHP,		JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"chickenmp",		ME_CHICKENMP,		JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"quitonhostile",	ME_QUITONHOSTILE,	JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"blockKeys",		ME_BLOCKKEYS,		JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"blockMouse",		ME_BLOCKMOUSE,		JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"gameReady",		ME_GAMEREADY,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"profile",			ME_PROFILE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"nopickup",		ME_NOPICKUP,		JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"pid",				ME_PID,				JSPROP_PERMANENT_VAR,	unit_getProperty},

	{"screensize",		OOG_SCREENSIZE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"windowtitle",		OOG_WINDOWTITLE,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"ingame",			OOG_INGAME,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"quitonerror",		OOG_QUITONERROR,	JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},
	{"maxgametime",		OOG_MAXGAMETIME,	JSPROP_STATIC_VAR,		unit_getProperty,	unit_setProperty},

	{"type",			UNIT_TYPE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"classid",			UNIT_CLASSID,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mode",			UNIT_MODE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"name",			UNIT_NAME,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mapid",			ME_MAPID,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"act",				UNIT_ACT,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gid",				UNIT_ID,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"x",				UNIT_XPOS,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"y",				UNIT_YPOS,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"area",			UNIT_AREA,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"hp",				UNIT_HP,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"hpmax",			UNIT_HPMAX,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mp",				UNIT_MP,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mpmax",			UNIT_MPMAX,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"stamina",			UNIT_STAMINA,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"staminamax",		UNIT_STAMINAMAX,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"charlvl",			UNIT_CHARLVL,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"itemcount",		UNIT_ITEMCOUNT,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"owner",			UNIT_OWNER,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"ownertype",		UNIT_OWNERTYPE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"spectype",		UNIT_SPECTYPE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"direction",		UNIT_DIRECTION,		JSPROP_PERMANENT_VAR,	unit_getProperty},

	{"code",			ITEM_CODE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefix",			ITEM_PREFIX,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffix",			ITEM_SUFFIX,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefixes",		ITEM_PREFIXES,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffixes",		ITEM_SUFFIXES,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefixnum",		ITEM_PREFIXNUM,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffixnum",		ITEM_SUFFIXNUM,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefixnums",		ITEM_PREFIXNUMS,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffixnums",		ITEM_SUFFIXNUMS,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"fname",			ITEM_FNAME,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"quality",			ITEM_QUALITY,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"node",			ITEM_NODE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"location",		ITEM_LOC,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"sizex",			ITEM_SIZEX,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"sizey",			ITEM_SIZEY,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"itemType",		ITEM_TYPE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"description",		ITEM_DESC,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"bodylocation",	ITEM_BODYLOCATION,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"ilvl",			ITEM_LEVEL,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{0},
};

static JSPropertySpec unit_props[] = {
	{"type",		UNIT_TYPE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"classid",		UNIT_CLASSID,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mode",		UNIT_MODE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"name",		UNIT_NAME,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"act",			UNIT_ACT,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gid",			UNIT_ID,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"x",			UNIT_XPOS,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"y",			UNIT_YPOS,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"area",		UNIT_AREA,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"hp",			UNIT_HP,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"hpmax",		UNIT_HPMAX,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mp",			UNIT_MP,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"mpmax",		UNIT_MPMAX,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"stamina",		UNIT_STAMINA,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"staminamax",	UNIT_STAMINAMAX,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"charlvl",		UNIT_CHARLVL,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"itemcount",	UNIT_ITEMCOUNT,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"owner",		UNIT_OWNER,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"ownertype",	UNIT_OWNERTYPE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"spectype",	UNIT_SPECTYPE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"direction",	UNIT_DIRECTION,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"uniqueid",	UNIT_UNIQUEID,		JSPROP_PERMANENT_VAR,	unit_getProperty},

	{"code",		ITEM_CODE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefix",		ITEM_PREFIX,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffix",		ITEM_SUFFIX,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefixes",	ITEM_PREFIXES,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffixes",	ITEM_SUFFIXES,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefixnum",	ITEM_PREFIXNUM,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffixnum",	ITEM_SUFFIXNUM,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"prefixnums",	ITEM_PREFIXNUMS,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"suffixnums",	ITEM_SUFFIXNUMS,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"fname",		ITEM_FNAME,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"quality",		ITEM_QUALITY,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"node",		ITEM_NODE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"location",	ITEM_LOC,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"sizex",		ITEM_SIZEX,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"sizey",		ITEM_SIZEY,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"itemType",	ITEM_TYPE,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"description", ITEM_DESC,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"bodylocation",ITEM_BODYLOCATION,	JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"ilvl",		ITEM_LEVEL,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"lvlreq",		ITEM_LEVELREQ,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"gfx",			ITEM_GFX,			JSPROP_PERMANENT_VAR,	unit_getProperty},

	{"runwalk",		ME_RUNWALK,			JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"weaponswitch",ME_WSWITCH,			JSPROP_PERMANENT_VAR,	unit_getProperty},

	{"objtype",		OBJECT_TYPE,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{"islocked",	OBJECT_LOCKED,		JSPROP_PERMANENT_VAR,	unit_getProperty},
	{0},
};

static JSFunctionSpec unit_methods[] = {
	{"getNext",			unit_getNext,		0},
	{"cancel",			unit_cancel,		0},
	{"repair",			unit_repair,		0},
	{"useMenu",			unit_useMenu,		0},
	{"interact",		unit_interact,		0},
	{"getItem",			unit_getItem,		3},
	{"getItems",		unit_getItems,		0},
	{"getMerc",			unit_getMerc,		0},
	{"getMercHP",		unit_getMercHP,		0},
	{"getSkill",		unit_getSkill,		0},
	{"getParent",		unit_getParent,		0},
	{"overhead",		my_overhead,		0},
	{"revive",			my_revive,			0},
	{"getFlags",		item_getFlags,		1},
	{"getFlag",			item_getFlag,		1},
	{"getStat",			unit_getStat,		1},
	{"getState",		unit_getState,		1},
//	{"getPrice",		item_getPrice,		1},
	{"getEnchant",		unit_getEnchant,	1},
	{"shop",			item_shop,			2},
	{"setSkill",		unit_setskill,		2},
	{"move",			unit_move,			2},
	{"getQuest",		unit_getQuest,		2},
	{"getMinionCount",	unit_getMinionCount, 1},
	{"getRepairCost",	me_getRepairCost,	1},
	{"getItemCost",		item_getItemCost,	1},
	{0},
};
