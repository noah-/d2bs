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
    const wchar_t* str1;
    const wchar_t* str2;
    const wchar_t* str3;
    const wchar_t* str4;
    unsigned int i;

    str1 = str2 = str3 = str4 = NULL;
    pt = PROFILETYPE_INVALID;

    try {
        // Profile()
        if (argc == 0) {
            if (Vars.szProfile != NULL)
                prof = new Profile();
            else
                THROW_ERROR(cx, "No active profile!");
        }
        // Profile(name) - get the named profile
        else if (argc == 1 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0])) {
            str1 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
            prof = new Profile(str1);
        }
        // Profile(ProfileType.singlePlayer, charname, diff)
        else if (argc == 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_SINGLEPLAYER) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "isu", &pt, &i);  //removed s flag no longer supported
            str1 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
            prof = new Profile(PROFILETYPE_SINGLEPLAYER, str1, (char)(JSVAL_TO_INT(JS_ARGV(cx, vp)[2])));
        }
        // Profile(ProfileType.battleNet, account, pass, charname, gateway)
        else if (argc == 5 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_BATTLENET) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "issss", &pt, &str1, &str2, &str3, &str4);
            str1 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
            str2 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2]));
            str3 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[3]));
            str4 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[4]));
            prof = new Profile(PROFILETYPE_BATTLENET, str1, str2, str3, str4);
        }
        // Profile(ProfileType.openBattleNet, account, pass, charname, gateway)
        else if (argc == 5 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_OPEN_BATTLENET) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "issss", &pt, &str1, &str2, &str3, &str4);
            str1 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
            str2 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2]));
            str3 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[3]));
            str4 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[4]));
            prof = new Profile(PROFILETYPE_OPEN_BATTLENET, str1, str2, str3, str4);
        }
        // Profile(ProfileType.tcpIpHost, charname, diff)
        else if (argc == 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_TCPIP_HOST) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "isu", &pt, &str1, &i);
            str1 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
            prof = new Profile(PROFILETYPE_TCPIP_HOST, str1, (char)(JSVAL_TO_INT(JS_ARGV(cx, vp)[2])));
        }
        // Profile(ProfileType.tcpIpJoin, charname, ip)
        else if (argc == 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == PROFILETYPE_TCPIP_JOIN) {
            // JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "iss", &pt, &str1, &str2);
            str1 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
            str2 = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2]));
            prof = new Profile(PROFILETYPE_TCPIP_JOIN, str1, str2);
        } else
            THROW_ERROR(cx, "Invalid parameters.");
    } catch (char* ex) {
        THROW_ERROR(cx, ex);
    }

	JSObject* obj = BuildObject(cx, &profile_class, profile_methods, profile_props);
    if (!obj)
        THROW_ERROR(cx, "Failed to create profile object");
    JS_SetPrivate(cx, obj, prof);
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

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
        vp.setString(JS_NewUCStringCopyZ(cx, prof->ip));
        break;
    case PROFILE_USERNAME:
        vp.setString(JS_NewUCStringCopyZ(cx, prof->username));
        break;
    case PROFILE_GATEWAY:
        vp.setString(JS_NewUCStringCopyZ(cx, prof->gateway));
        break;
    case PROFILE_CHARACTER:
        vp.setString(JS_NewUCStringCopyZ(cx, prof->charname));
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
