#pragma once
#ifndef __JSGAME_H__
#define __JSGAME_H__

#include "js32.h"

JSAPI_FUNC(my_rand);
JSAPI_FUNC(my_submitItem);
JSAPI_FUNC(my_copyUnit);
JSAPI_FUNC(my_clickMap);
JSAPI_FUNC(my_acceptTrade);
JSAPI_FUNC(my_tradeOk);
JSAPI_FUNC(my_getDialogLines);
JSAPI_FUNC(my_clickDialog);
JSAPI_FUNC(my_clickItem);
JSAPI_FUNC(my_gold);
JSAPI_FUNC(my_checkCollision);
JSAPI_FUNC(my_playSound);
JSAPI_FUNC(my_quit);
JSAPI_FUNC(my_quitGame);
JSAPI_FUNC(my_say);
JSAPI_FUNC(my_weaponSwitch);
JSAPI_FUNC(my_transmute);
JSAPI_FUNC(my_clickParty);
JSAPI_FUNC(my_useStatPoint);
JSAPI_FUNC(my_useSkillPoint);

JSAPI_FUNC(my_getInteractedNPC);
JSAPI_FUNC(my_getIsTalkingNPC);

JSAPI_FUNC(my_takeScreenshot);
JSAPI_FUNC(my_getMouseCoords);
JSAPI_FUNC(my_getDistance);
JSAPI_FUNC(my_getPath);
JSAPI_FUNC(my_getCollision);
JSAPI_FUNC(unit_getMercHP);
JSAPI_FUNC(my_getCursorType);
JSAPI_FUNC(my_getSkillByName);
JSAPI_FUNC(my_getSkillById);
JSAPI_FUNC(my_getLocaleString);
JSAPI_FUNC(my_getTextSize);
JSAPI_FUNC(my_getUIFlag);
JSAPI_FUNC(my_getTradeInfo);
JSAPI_FUNC(my_getWaypoint);
JSAPI_FUNC(my_getBaseStat);
JSAPI_FUNC(my_getPlayerFlag);
JSAPI_FUNC(my_moveNPC);
JSAPI_FUNC(my_revealLevel);
#endif
