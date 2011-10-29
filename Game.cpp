#include "Game.h"
#include "D2Ptrs.h"
#include "Constants.h"
#include "D2Helpers.h"

// TODO: Refactor a lot of the JSGame functions here

void SendGold(int nGold, int nMode)
{
	*p_D2CLIENT_GoldDialogAmount = nGold;
	*p_D2CLIENT_GoldDialogAction = nMode;
	D2CLIENT_PerformGoldDialogAction();
}

void __fastcall UseStatPoint(WORD stat, unsigned int count)
{
	if(D2COMMON_GetUnitStat(D2CLIENT_GetPlayerUnit(), STAT_STATPOINTSLEFT, 0) < count)
		return;

	BYTE packet[3] = {0x3A};
	*(WORD*)&packet[1] = stat;

	for(unsigned int i = 0; i < count; i++)
	{
		D2CLIENT_SendGamePacket(3, packet);
		if(i != count-1) Sleep(500);
	}
}

void __fastcall UseSkillPoint(WORD skill, unsigned int count)
{
	if(D2COMMON_GetUnitStat(D2CLIENT_GetPlayerUnit(), STAT_SKILLPOINTSLEFT, 0) < count)
		return;

	BYTE packet[3] = {0x3B};
	*(WORD*)&packet[1] = skill;

	for(unsigned int i = 0; i < count; i++)
	{
		D2CLIENT_SendGamePacket(3, packet);
		if(i != count-1) Sleep(500);
	}
}
