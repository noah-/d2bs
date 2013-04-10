#pragma once

#include "js32.h"
#include "JSCore.h"
#include "JSGame.h"
#include "JSMenu.h"
#include "JSHash.h"

#include "JSFile.h"
#include "JSFileTools.h"
#include "JSDirectory.h"
#include "JSSQLite.h"
#include "JSSandbox.h"
#include "JSScreenHook.h"
#include "JSParty.h"
#include "JSArea.h"
#include "JSPresetUnit.h"
#include "JSUnit.h"
#include "JSScript.h"
#include "JSRoom.h"

static JSFunctionSpec global_funcs[] = {
	// "get" functions
	JS_FN("getUnit",			unit_getUnit,			1, FUNCTION_FLAGS),
	JS_FN("getPath",			my_getPath,				0, FUNCTION_FLAGS),
	JS_FN("getCollision",		my_getCollision,		0, FUNCTION_FLAGS),
	JS_FN("getMercHP",			unit_getMercHP,			0, FUNCTION_FLAGS),
	JS_FN("getCursorType",		my_getCursorType,		0, FUNCTION_FLAGS),
	JS_FN("getSkillByName",		my_getSkillByName,		1, FUNCTION_FLAGS),
	JS_FN("getSkillById",		my_getSkillById,		1, FUNCTION_FLAGS),
	JS_FN("getLocaleString",	my_getLocaleString,		1, FUNCTION_FLAGS),
	JS_FN("getTextSize",		my_getTextSize,			2, FUNCTION_FLAGS),
	JS_FN("getThreadPriority",	my_getThreadPriority,	0, FUNCTION_FLAGS),
	JS_FN("getUIFlag",			my_getUIFlag,			1, FUNCTION_FLAGS),
	JS_FN("getTradeInfo",		my_getTradeInfo,		0, FUNCTION_FLAGS),
	JS_FN("getWaypoint",		my_getWaypoint,			1, FUNCTION_FLAGS),
	JS_FN("getScript",			my_getScript,			0, FUNCTION_FLAGS),
	JS_FN("getRoom",			my_getRoom,				0, FUNCTION_FLAGS),
	JS_FN("getParty",			my_getParty,			0, FUNCTION_FLAGS),
	JS_FN("getPresetUnit",		my_getPresetUnit,		0, FUNCTION_FLAGS),
	JS_FN("getPresetUnits",		my_getPresetUnits,		0, FUNCTION_FLAGS),
	JS_FN("getArea",			my_getArea,				0, FUNCTION_FLAGS),
	JS_FN("getBaseStat",		my_getBaseStat,			0, FUNCTION_FLAGS),
	JS_FN("getControl",			my_getControl,			0, FUNCTION_FLAGS),
	JS_FN("getPlayerFlag",		my_getPlayerFlag,		2, FUNCTION_FLAGS),
	JS_FN("getTickCount",		my_getTickCount,		0, FUNCTION_FLAGS),
	JS_FN("getInteractedNPC",	my_getInteractedNPC,	0, FUNCTION_FLAGS),
	JS_FN("getIsTalkingNPC",	my_getIsTalkingNPC,		0, FUNCTION_FLAGS),
	JS_FN("getDialogLines",		my_getDialogLines,		0, FUNCTION_FLAGS),

	// utility functions that don't have anything to do with the game
	JS_FN("print",				my_print,				1, FUNCTION_FLAGS),
	JS_FN("setTimeout",			my_setTimeout,			0, FUNCTION_FLAGS),
	JS_FN("setInterval",		my_setInterval,			0, FUNCTION_FLAGS),
	JS_FN("clearInterval",		my_clearInterval,			1, FUNCTION_FLAGS),
	JS_FN("delay",				my_delay,				1, FUNCTION_FLAGS),
	JS_FN("load",				my_load,				1, FUNCTION_FLAGS),
	JS_FN("isIncluded",			my_isIncluded,			1, FUNCTION_FLAGS),
	JS_FN("include",			my_include,				1, FUNCTION_FLAGS),
	JS_FN("stop",				my_stop,				0, FUNCTION_FLAGS),
	JS_FN("rand",				my_rand,				0, FUNCTION_FLAGS),
	JS_FN("copy",				my_copy,				0, FUNCTION_FLAGS),
	JS_FN("paste",				my_paste,				0, FUNCTION_FLAGS),
	JS_FN("sendCopyData",		my_sendCopyData,		4, FUNCTION_FLAGS),
	JS_FN("sendDDE",			my_sendDDE,				0, FUNCTION_FLAGS),
	JS_FN("keystate",			my_keystate,			0, FUNCTION_FLAGS),
	JS_FN("addEventListener",	my_addEventListener,	2, FUNCTION_FLAGS),
	JS_FN("removeEventListener",	my_removeEventListener,	2, FUNCTION_FLAGS),
	JS_FN("clearEvent",			my_clearEvent,			1, FUNCTION_FLAGS),
	JS_FN("clearAllEvents",		my_clearAllEvents,		0, FUNCTION_FLAGS),
	JS_FN("js_strict",			my_js_strict,			0, FUNCTION_FLAGS),
	JS_FN("version",			my_version,				0, FUNCTION_FLAGS),
	JS_FN("scriptBroadcast",	my_scriptBroadcast,		1, FUNCTION_FLAGS),
	JS_FN("sqlite_version",		my_sqlite_version,		0, FUNCTION_FLAGS),
	JS_FN("sqlite_memusage",	my_sqlite_memusage,		0, FUNCTION_FLAGS),
	JS_FN("dopen",				my_openDir,				1, FUNCTION_FLAGS),
	JS_FN("debugLog",			my_debugLog,			1, FUNCTION_FLAGS),
	JS_FN("showConsole",		my_showConsole,			0, FUNCTION_FLAGS),
	JS_FN("hideConsole",		my_hideConsole,			0, FUNCTION_FLAGS),

	// out of game functions
	JS_FN("login",				my_login,				1, FUNCTION_FLAGS),
//	JS_FN("createCharacter",		my_createCharacter,		4, FUNCTION_FLAGS), // this function is not finished
	JS_FN("selectCharacter",	my_selectChar,			1, FUNCTION_FLAGS),
	JS_FN("createGame",			my_createGame,			3, FUNCTION_FLAGS),
	JS_FN("joinGame",			my_joinGame,			2, FUNCTION_FLAGS),
	JS_FN("addProfile",			my_addProfile,			6, FUNCTION_FLAGS),
	JS_FN("getLocation",		my_getOOGLocation,		0, FUNCTION_FLAGS),
	JS_FN("loadMpq",			my_loadMpq,				1, FUNCTION_FLAGS),

	// game functions that don't have anything to do with gathering data
	JS_FN("submitItem",			my_submitItem,			0, FUNCTION_FLAGS),
	JS_FN("getMouseCoords",		my_getMouseCoords,		1, FUNCTION_FLAGS),
	JS_FN("copyUnit",			my_copyUnit,			1, FUNCTION_FLAGS),
	JS_FN("clickMap",			my_clickMap,			3, FUNCTION_FLAGS),
	JS_FN("acceptTrade",		my_acceptTrade,			0, FUNCTION_FLAGS),
	JS_FN("tradeOk",			my_tradeOk,				0, FUNCTION_FLAGS),
	JS_FN("beep",				my_beep,				0, FUNCTION_FLAGS),
	JS_FN("clickItem",			my_clickItem,			0, FUNCTION_FLAGS),
	JS_FN("getDistance",		my_getDistance,			2, FUNCTION_FLAGS),
	JS_FN("gold",				my_gold,				1, FUNCTION_FLAGS),
	JS_FN("checkCollision",		my_checkCollision,		2, FUNCTION_FLAGS),
	JS_FN("playSound",			my_playSound,			1, FUNCTION_FLAGS),
	JS_FN("quit",				my_quit,				0, FUNCTION_FLAGS),
	JS_FN("quitGame",			my_quitGame,			0, FUNCTION_FLAGS),
	JS_FN("say",				my_say,					1, FUNCTION_FLAGS),
	JS_FN("clickParty",			my_clickParty,			1, FUNCTION_FLAGS),
	JS_FN("weaponSwitch",		my_weaponSwitch,		0, FUNCTION_FLAGS),
	JS_FN("transmute",			my_transmute,			0, FUNCTION_FLAGS),
	JS_FN("useStatPoint",		my_useStatPoint,		1, FUNCTION_FLAGS),
	JS_FN("useSkillPoint",		my_useSkillPoint,		1, FUNCTION_FLAGS),
	JS_FN("takeScreenshot",		my_takeScreenshot,		0, FUNCTION_FLAGS),
	JS_FN("moveNPC",            my_moveNPC,             0, FUNCTION_FLAGS),
	JS_FN("getPacket",		    my_getPacket,	     	0, FUNCTION_FLAGS),
	JS_FN("sendPacket",		    my_sendPacket,   		0, FUNCTION_FLAGS),
	JS_FN("getIP",				my_getIP,				0, FUNCTION_FLAGS),
	JS_FN("revealLevel",        my_revealLevel,         0, FUNCTION_FLAGS),
	// drawing functions
	JS_FN("screenToAutomap",	screenToAutomap,		1, FUNCTION_FLAGS),
	JS_FN("automapToScreen",	automapToScreen,		1, FUNCTION_FLAGS),
	
	// hash functions
	JS_FN("md5",				my_md5,					1, FUNCTION_FLAGS),
	JS_FN("sha1",				my_sha1,				1, FUNCTION_FLAGS),
	JS_FN("sha256",				my_sha256,				1, FUNCTION_FLAGS),
	JS_FN("sha384",				my_sha384,				1, FUNCTION_FLAGS),
	JS_FN("sha512",				my_sha512,				1, FUNCTION_FLAGS),
	JS_FN("md5_file",			my_md5_file,			1, FUNCTION_FLAGS),
	JS_FN("sha1_file",			my_sha1_file,			1, FUNCTION_FLAGS),
	JS_FN("sha256_file",		my_sha256_file,			1, FUNCTION_FLAGS),
	JS_FN("sha384_file",		my_sha384_file,			1, FUNCTION_FLAGS),
	JS_FN("sha512_file",		my_sha512_file,			1, FUNCTION_FLAGS),
	JS_FS_END
};
