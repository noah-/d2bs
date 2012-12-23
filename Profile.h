#ifndef PROFILE_H
#define PROFILE_H

#include "D2BS.h"

enum ProfileType
{
	PROFILETYPE_INVALID,
	PROFILETYPE_SINGLEPLAYER,
	PROFILETYPE_BATTLENET,
	PROFILETYPE_OPEN_BATTLENET,
	PROFILETYPE_TCPIP_HOST,
	PROFILETYPE_TCPIP_JOIN
};

class Profile
{
	protected:
		ProfileType type;
		union
		{
			char ip[16];
			char username[48];
		};
		char password[256];
		char gateway[256];
		char charname[24];
		char diff;
		unsigned int maxLoginTime;
		unsigned int maxCharTime;

		void init(const char* profileName);

		void init(ProfileType pt, char* _ipUsername, char* _password,
			char* _charname, char* _gateway, char _diff,
			unsigned int _maxLoginTime, unsigned int _maxCharTime)
		{
			type = pt;
			strcpy_s(username, sizeof(username), _ipUsername);
			strcpy_s(password, sizeof(password), _password);
			strcpy_s(gateway, sizeof(gateway), _gateway);
			strcpy_s(charname, sizeof(charname), _charname);
			diff = _diff;
			maxLoginTime = _maxLoginTime;
			maxCharTime = _maxCharTime;
		}

	public:
		// Get the active profile
		Profile()
		{
			init(Vars.szProfile);
		}

		// Get profile by name
		Profile(char* profileName)
		{
			init(profileName);
		}

		// Create single player or tcp/ip host profile
		Profile(ProfileType pt, char* charName, char _diff)
		{
			init(pt, "", "", charName, "", _diff, 5000, 5000);
		}

		// Create battle.net or open battle.net profile
		Profile(ProfileType pt, char* account, char* _pass, char* _charname, char* _gateway)
		{
			init(pt, account, _pass, _charname, _gateway, 0, 5000, 5000);
		}

		// Create tcp/ip join profile
		Profile(ProfileType pt, char* _charname, char* ip)
		{
			init(pt, ip, "", _charname, "", 0, 5000, 5000);
		}

		friend JSAPI_PROP(profile_getProperty);

		DWORD login(char** error);

		static bool ProfileExists(const char* profile);
};

#endif