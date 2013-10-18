#pragma once

#include "js32.h"

JSAPI_FUNC(my_getThreadPriority);
JSAPI_FUNC(my_debugLog);
JSAPI_FUNC(my_print);
JSAPI_FUNC(my_setTimeout);
JSAPI_FUNC(my_setInterval);
JSAPI_FUNC(my_clearInterval);
JSAPI_FUNC(my_delay);
JSAPI_FUNC(my_load);
JSAPI_FUNC(my_include);
JSAPI_FUNC(my_isIncluded);
JSAPI_FUNC(my_stop);
JSAPI_FUNC(my_beep);
JSAPI_FUNC(my_copy);
JSAPI_FUNC(my_paste);
JSAPI_FUNC(my_sendCopyData);
JSAPI_FUNC(my_sendDDE);
JSAPI_FUNC(my_keystate);
JSAPI_FUNC(my_getTickCount);
JSAPI_FUNC(my_addEventListener);
JSAPI_FUNC(my_removeEventListener);
JSAPI_FUNC(my_clearEvent);
JSAPI_FUNC(my_clearAllEvents);
JSAPI_FUNC(my_js_strict);
JSAPI_FUNC(my_version);
JSAPI_FUNC(my_scriptBroadcast);
JSAPI_FUNC(my_showConsole);
JSAPI_FUNC(my_hideConsole);
JSAPI_FUNC(my_loadMpq);
JSAPI_FUNC(my_getPacket);
JSAPI_FUNC(my_sendPacket);
JSAPI_FUNC(my_getIP);
JSAPI_FUNC(my_sendClick);