#include "JSMenu.h"
//#include "Control.h"
#include "JSControl.h"
#include "Constants.h"
#include "Helpers.h"
#include "D2BS.h"

#include "Profile.h"

JSAPI_FUNC(my_login) {
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    if (ClientState() != ClientStateMenu)
        return JS_TRUE;

    const wchar_t* profile = NULL;
    char* error;

    if (!JSVAL_IS_STRING(JS_ARGV(cx, vp)[0])) {
        if (Vars.szProfile != NULL) {
            profile = Vars.szProfile;
        } else
            THROW_ERROR(cx, "Invalid profile specified!");
    } else {
        profile = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        wcscpy_s(Vars.szProfile, 256, profile);
    }

    if (!profile)
        THROW_ERROR(cx, "Could not get profile!");
    if (!Profile::ProfileExists(profile))
        THROW_ERROR(cx, "Profile does not exist!");

    Profile* prof = new Profile(profile);
    if (prof->login(&error) != 0) {
        delete prof;
        THROW_ERROR(cx, error);
    }

    delete prof;
    return JS_TRUE;
}

JSAPI_FUNC(my_selectChar) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    if (argc != 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
        THROW_ERROR(cx, "Invalid parameters specified to selectCharacter");

    const wchar_t* profile = JS_GetStringCharsZ(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));

    if (!Profile::ProfileExists(profile))
        THROW_ERROR(cx, "Invalid profile specified");
    wchar_t charname[24], file[_MAX_FNAME + MAX_PATH];
    swprintf_s(file, _countof(file), L"%sd2bs.ini", Vars.szPath);
    GetPrivateProfileStringW(profile, L"character", L"ERROR", charname, _countof(file), file);

    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(OOG_SelectCharacter(charname)));

    return JS_TRUE;
}

JSAPI_FUNC(my_createGame) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    if (ClientState() != ClientStateMenu)
        return JS_TRUE;

    const wchar_t *name = NULL, *pass = NULL;
    jschar *jsname = NULL, *jspass = NULL;
    int32 diff = 3;
    if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "W/Wi", &jsname, &jspass, &diff)) {
        JS_ReportError(cx, "Invalid arguments specified to createGame");
        return JS_FALSE;
    }
    name = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
    pass = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));

    if (!pass)
        pass = L"";

    if (wcslen(name) > 15 || wcslen(pass) > 15)
        THROW_ERROR(cx, "Invalid game name or password length");

    if (!OOG_CreateGame(name, pass, diff))
        THROW_ERROR(cx, "createGame failed");

    return JS_TRUE;
}

JSAPI_FUNC(my_joinGame) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    if (ClientState() != ClientStateMenu)
        return JS_TRUE;
    jschar *jsname = NULL, *jspass = NULL;
    const wchar_t *name = NULL, *pass = NULL;
    if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "W/W", &jsname, &jspass)) {
        JS_ReportError(cx, "Invalid arguments specified to createGame");
        return JS_FALSE;
    }
    name = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
    pass = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
    if (!pass)
        pass = L"";

    if (wcslen(name) > 15 || wcslen(pass) > 15)
        THROW_ERROR(cx, "Invalid game name or password length");

    if (!OOG_JoinGame(name, pass))
        THROW_ERROR(cx, "joinGame failed");

    return JS_TRUE;
}

JSAPI_FUNC(my_addProfile) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    // validate the args...
    const wchar_t *profile, *mode, *gateway, *username, *password, *charname;
    profile = mode = gateway = username = password = charname = NULL;
    int spdifficulty = 3;
    if (argc < 6 || argc > 7)
        THROW_ERROR(cx, "Invalid arguments passed to addProfile");

    const wchar_t** args[] = {&profile, &mode, &gateway, &username, &password, &charname};
    for (uint i = 0; i < 6; i++) {
        if (!JSVAL_IS_STRING(JS_ARGV(cx, vp)[i])) {
            THROW_ERROR(cx, "Invalid argument passed to addProfile");
        } else
            *args[i] = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[i]));
    }

    for (int i = 0; i < 6; i++) {
        if (!(*args[i]))
            THROW_ERROR(cx, "Failed to convert string");
    }

    if (argc == 7)
        spdifficulty = JSVAL_TO_INT(JS_ARGV(cx, vp)[6]);

    if (spdifficulty > 3 || spdifficulty < 0)
        THROW_ERROR(cx, "Invalid argument passed to addProfile");

    wchar_t file[_MAX_FNAME + _MAX_PATH];

    swprintf_s(file, _countof(file), L"%sd2bs.ini", Vars.szPath);
    if (!Profile::ProfileExists(*args[0])) {
        wchar_t settings[600] = L"";
        swprintf_s(settings, _countof(settings), L"mode=%s\tgateway=%s\tusername=%s\tpassword=%s\tcharacter=%s\tspdifficulty=%d\t", mode, gateway, username, password,
                   charname, spdifficulty);

        StringReplace(settings, '\t', '\0', 600);
        WritePrivateProfileSectionW(*args[0], settings, file);
    }

    return JS_TRUE;
}

JSAPI_FUNC(my_getOOGLocation) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    if (ClientState() != ClientStateMenu)
        return JS_TRUE;

    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(OOG_GetLocation()));

    return JS_TRUE;
}

JSAPI_FUNC(my_createCharacter) {
    if (ClientState() != ClientStateMenu)
        return JS_TRUE;

    const wchar_t* name = NULL;
    jschar* jsname = NULL;
    int32 type = -1;
    JSBool hc = JS_FALSE, ladder = JS_FALSE;
    JS_BeginRequest(cx);
    if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "Wi/bb", &jsname, &type, &hc, &ladder)) {
        JS_EndRequest(cx);
        THROW_ERROR(cx, "Failed to Convert Args createCharacter");
    }
    JS_EndRequest(cx);
    name = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!OOG_CreateCharacter(name, type, !!hc, !!ladder)));
    return JS_TRUE;
}
