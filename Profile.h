#ifndef PROFILE_H
#define PROFILE_H

#include "D2BS.h"

enum ProfileType { PROFILETYPE_INVALID, PROFILETYPE_SINGLEPLAYER, PROFILETYPE_BATTLENET, PROFILETYPE_OPEN_BATTLENET, PROFILETYPE_TCPIP_HOST, PROFILETYPE_TCPIP_JOIN };

class Profile {
  protected:
    ProfileType type;
    union {
        wchar_t ip[16];
        wchar_t username[48];
    };
    wchar_t password[256];
    wchar_t gateway[256];
    wchar_t charname[24];
    wchar_t diff;
    unsigned int maxLoginTime;
    unsigned int maxCharTime;

    void init(const wchar_t* profileName);

    void init(ProfileType pt, const wchar_t* _ipUsername, const wchar_t* _password, const wchar_t* _charname, const wchar_t* _gateway, const wchar_t _diff,
              unsigned int _maxLoginTime,
              unsigned int _maxCharTime) {
        type = pt;
        wcscpy_s(username, wcslen(username), _ipUsername);
        wcscpy_s(password, wcslen(password), _password);
        wcscpy_s(gateway, wcslen(gateway), _gateway);
        wcscpy_s(charname, wcslen(charname), _charname);
        diff = _diff;
        maxLoginTime = _maxLoginTime;
        maxCharTime = _maxCharTime;
    }

  public:
    // Get the active profile
    Profile() {
        init(Vars.szProfile);
    }

    // Get profile by name
    Profile(const wchar_t* profileName) {
        init(profileName);
    }

    // Create single player or tcp/ip host profile
    Profile(ProfileType pt, const wchar_t* charName, const wchar_t _diff) {
        init(pt, L"", L"", charName, L"", _diff, 5000, 5000);
    }

    // Create battle.net or open battle.net profile
    Profile(ProfileType pt, const wchar_t* account, const wchar_t* _pass, const wchar_t* _charname, const wchar_t* _gateway) {
        init(pt, account, _pass, _charname, _gateway, 0, 5000, 5000);
    }

    // Create tcp/ip join profile
    Profile(ProfileType pt, const wchar_t* _charname, const wchar_t* ip) {
        init(pt, ip, L"", _charname, L"", 0, 5000, 5000);
    }

    friend JSAPI_PROP(profile_getProperty);

    DWORD login(char** error);

    static bool ProfileExists(const wchar_t* profile);
    static bool ProfileExists(const char* profile);
};

#endif