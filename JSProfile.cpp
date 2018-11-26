#include "JSProfile.h"
#include "Profile.h"
#include "Helpers.h"

#include "D2BS.h"

// Profile() - get the active profile
// Profile(name) - get the named profile
//
// Create profiles:
//	Profile(ProfileType.singlePlayer, charname, diff)
//	Profile(ProfileType.battleNet, account, pass, charname, gateway)
//	Profile(ProfileType.openBattleNet, account, pass, charname, gateway)
//	Profile(ProfileType.tcpIpHost, charname, diff)
//	Profile(ProfileType.tcpIpJoin, charname, ip)
CLASS_CTOR(profile) {
    Profile* prof;
    ProfileType pt;
    char* str1;
    char* str2;
    char* str3;
    char* str4;
    unsigned int i;

    str1 = str2 = str3 = str4 = NULL;
    pt = PROFILETYPE_INVALID;

    try {
        wchar_t* str0;
        wchar_t* str1;
        wchar_t* str2;
        wchar_t* str3;
        wchar_t* str4;

        // Profile()
        if (argc == 0) {
            if (Vars.szProfile != NULL)
                prof = new Profile();
            else
                THROW_ERROR(cx, "No active profile!");
        }
        // Profile(name) - get the named profile
        else if (argc == 1 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0])) {
            str0 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0])));
            prof = new Profile(str0);
            free(str0);
        }
        // Profile(ProfileType.singlePlayer, charname, diff)
        else if (argc == 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_SINGLEPLAYER) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "isu", &pt, &i);  //removed s flag no longer supported
            str1 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1])));
            str2 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2])));
            prof = new Profile(PROFILETYPE_SINGLEPLAYER, str1, str2);
            free(str1);
            free(str2);
        }
        // Profile(ProfileType.battleNet, account, pass, charname, gateway)
        else if (argc == 5 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_BATTLENET) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "issss", &pt, &str1, &str2, &str3, &str4);
            str1 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1])));
            str2 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2])));
            str3 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[3])));
            str4 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[4])));
            prof = new Profile(PROFILETYPE_BATTLENET, str1, str2, str3, str4);
            free(str1);
            free(str2);
            free(str3);
            free(str4);
        }
        // Profile(ProfileType.openBattleNet, account, pass, charname, gateway)
        else if (argc == 5 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_OPEN_BATTLENET) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "issss", &pt, &str1, &str2, &str3, &str4);
            str1 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1])));
            str2 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2])));
            str3 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[3])));
            str4 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[4])));
            prof = new Profile(PROFILETYPE_OPEN_BATTLENET, str1, str2, str3, str4);
            free(str1);
            free(str2);
            free(str3);
            free(str4);
        }
        // Profile(ProfileType.tcpIpHost, charname, diff)
        else if (argc == 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_TCPIP_HOST) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "isu", &pt, &str1, &i);
            str1 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1])));
            str2 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2])));
            prof = new Profile(PROFILETYPE_TCPIP_HOST, str1, str2);
            free(str1);
            free(str2);
        }
        // Profile(ProfileType.tcpIpJoin, charname, ip)
        else if (argc == 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_TCPIP_JOIN) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "iss", &pt, &str1, &str2);
            str1 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1])));
            str2 = AnsiToUnicode(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2])));
            prof = new Profile(PROFILETYPE_TCPIP_JOIN, str1, str2);
            free(str1);
            free(str2);
        } else
            THROW_ERROR(cx, "Invalid parameters.");
    } catch (char* ex) {
        THROW_ERROR(cx, ex);
    }

    JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), prof);

    return JS_TRUE;
}

JSAPI_FUNC(profile_login) {
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    char* error;
    Profile* prof;

    prof = (Profile*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

    if (prof->login(&error) != 0)
        THROW_ERROR(cx, error);

    return JS_TRUE;
}

void profile_finalize(JSFreeOp* fop, JSObject* obj) {
    Profile* prof;

    prof = (Profile*)JS_GetPrivate(obj);

    if (prof != NULL)
        delete prof;

    JS_SetPrivate(obj, NULL);
}

JSAPI_PROP(profile_getProperty) {
    Profile* prof;

    prof = (Profile*)JS_GetPrivate(cx, obj);
    jsval ID;
    JS_IdToValue(cx, id, &ID);
    char* nStr;

    switch (JSVAL_TO_INT(ID)) {
    case PROFILE_TYPE:
        vp.setInt32(prof->type);
        break;
    case PROFILE_IP:
        nStr = UnicodeToAnsi(prof->ip);
        vp.setString(JS_NewStringCopyZ(cx, nStr));
        free(nStr);
        break;
    case PROFILE_USERNAME:
        nStr = UnicodeToAnsi(prof->username);
        vp.setString(JS_NewStringCopyZ(cx, nStr));
        free(nStr);
        break;
    case PROFILE_GATEWAY:
        nStr = UnicodeToAnsi(prof->gateway);
        vp.setString(JS_NewStringCopyZ(cx, nStr));
        free(nStr);
        break;
    case PROFILE_CHARACTER:
        nStr = UnicodeToAnsi(prof->charname);
        vp.setString(JS_NewStringCopyZ(cx, nStr));
        free(nStr);
        break;
    case PROFILE_DIFFICULTY:
        vp.setInt32(prof->diff);
        break;
    case PROFILE_MAXLOGINTIME:
        vp.setInt32(prof->maxLoginTime);
        break;
    case PROFILE_MAXCHARSELTIME:
        vp.setInt32(prof->maxCharTime);
        break;
    }

    return JS_TRUE;
}

EMPTY_CTOR(profileType);

JSAPI_PROP(profileType_getProperty) {
    jsval ID;
    JS_IdToValue(cx, id, &ID);
    vp.set(ID);

    return JS_TRUE;
}
