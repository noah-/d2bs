#pragma once

#include "Offset.h"
#include "D2Intercepts.h"
#include "D2Handlers.h"

PatchHook Patches[] = {
	{PatchCall,	GetDllOffset("D2Client.dll", 0xB2342),	(DWORD)GameInput_Intercept,				5},//1.13d
	{PatchJmp,	GetDllOffset("D2Client.dll", 0x1D7B4),	(DWORD)GameDraw_Intercept,				6},//1.13d
	{PatchCall,	GetDllOffset("D2Client.dll", 0x83301),	(DWORD)GamePacketReceived_Intercept,	5},//1.13d
	{PatchJmp,  GetDllOffset("D2Client.dll", 0xD13C),   (DWORD)GamePacketSent_Interception,		6},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x2B494),	(DWORD)GetSelectedUnit_Intercept,		5},//1.13d
	{PatchJmp,	GetDllOffset("D2Client.dll", 0x84417),	(DWORD)PlayerAssignment_Intercept,		5},//1.13d
	{PatchBytes,GetDllOffset("D2Client.dll", 0x14630),	(DWORD)0xc3,							1},//1.13d
	{PatchCall, GetDllOffset("D2Client.dll", 0x1B047),	(DWORD)GameActChange_Intercept,			5},//1.13d
	{PatchJmp,  GetDllOffset("D2Client.dll", 0x1B474),	(DWORD)GameActChange2_Intercept,		5},//1.13d
	{PatchCall, GetDllOffset("D2Client.dll", 0x461AD),	(DWORD)GameLeave_Intercept,				5},//1.13d
	{PatchCall,	GetDllOffset("D2Client.dll", 0x29560),	(DWORD)GameAttack_Intercept,			5},//1.13d

//	{PatchCall,	GetDllOffset("D2Client.dll", 0xA7364),	(DWORD)AddUnit_Intercept,				5},
//	{PatchCall,	GetDllOffset("D2Client.dll", 0xA6F25),	(DWORD)RemoveUnit_Intercept,			9},

	// this patch needs a constant in ChatPacketRecv_Interception updated also
	{PatchCall, GetDllOffset("BNCLIENT.DLL", 0xCEBC),   (DWORD)ChatPacketRecv_Interception,		12},

	{PatchCall,	GetDllOffset("D2Multi.dll", 0x142FC),	(DWORD)Whisper_Intercept,				7},//1.13d
	{PatchCall, GetDllOffset("D2Multi.dll", 0x11D63),	(DWORD)ChannelInput_Intercept,			5},//1.13d
	
	//{PatchCall,	GetDllOffset("D2Multi.dll", 0x14A9A),	(DWORD)ChannelWhisper_Intercept,		5},//1.13d //not used
	{PatchJmp,	GetDllOffset("D2Multi.dll", 0x14BE0),	(DWORD)ChannelChat_Intercept,			6},//1.13d
	{PatchJmp,	GetDllOffset("D2Multi.dll", 0x14850),	(DWORD)ChannelEmote_Intercept,			6},//1.13d
	
	{PatchCall,	GetDllOffset("D2Win.dll", 0xEC68),		(DWORD)GameDrawOOG_Intercept,			5},//1.13d

	{PatchCall,	GetDllOffset("D2CMP.dll", 0x14CD5),		(DWORD)GameCrashFix_Intercept,			10},//1.13d
	
	//{PatchCall,	GetDllOffset("D2MCPClient.dll", 0x6297), (DWORD)RealmPacketRecv_Interception, 5},
	
	{PatchCall, GetDllOffset("Fog.dll", 0xDE51),		(DWORD)LogMessageBoxA_Intercept,		6},
};

PatchHook Conditional[] = {
	{PatchJmp,	GetDllOffset("BNCLIENT.DLL",0x15EB3),	(DWORD)ClassicSTUB,		5}, 
	{PatchJmp,	GetDllOffset("BNCLIENT.DLL",0x161B8),	(DWORD)LodSTUB,			5} 
};