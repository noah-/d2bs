#include "D2Handlers.h"
#include "D2Ptrs.h"
#include "D2BS.h"

void __declspec(naked) GamePacketReceived_Intercept()
{
	__asm
	{
		pop ebp;
		pushad;

		call GamePacketReceived;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

OldCode:
		call D2NET_ReceivePacket_I;

		push ebp;
		ret;
	}
}

void __declspec(naked) GameDraw_Intercept()
{
	__asm
	{
		call GameDraw;

		POP ESI
		POP EBX
		POP ECX
		RETN 4
	}
}

void __declspec(naked) GameInput_Intercept()
{
	__asm {
		pushad
		mov ecx, ebx
		call GameInput
		cmp eax, -1
		popad
		je BlockIt
		call D2CLIENT_InputCall_I
		ret

BlockIt:

		xor eax,eax
		ret
	}
}

UnitAny* GetSelectedUnit_Intercept(void)
{
	if(Vars.bClickAction)
	{
		if(Vars.dwSelectedUnitId)
		{
			UnitAny* pUnit = D2CLIENT_FindUnit(Vars.dwSelectedUnitId, Vars.dwSelectedUnitType);

			return pUnit;
		}

		return NULL;
	}

	return D2CLIENT_GetSelectedUnit();
}

void __declspec(naked) Whisper_Intercept()
{
	__asm
	{
		MOV EBP,DWORD PTR SS:[ESP+0x1FC+4]
		pushad
		mov ecx, edx
		mov edx, ebx
		call WhisperHandler
		popad
		//jmp D2MULTI_WhisperIntercept_Jump
		retn
	}
}

void __declspec(naked) GameAttack_Intercept()
{
	__asm 
	{
		push ecx
		mov ecx, [esp+0xC]
		call GameAttack
		pop ecx

		cmp eax, -1
		je OldCode

		call D2CLIENT_GetSelectedUnit
		
		cmp eax, 0
		je OldCode

		mov [esp+0x0C], 1

OldCode:
		mov eax, [p_D2CLIENT_ScreenSizeY]
		mov eax, [eax]
		retn
	}
}

void __declspec(naked) PlayerAssignment_Intercept()
{
	__asm
	{
		FNOP
		CALL D2CLIENT_AssignPlayer_I
		MOV ECX, EAX
		CALL GamePlayerAssignment
		RETN
	}
}

void __declspec(naked) GameCrashFix_Intercept()
{
	__asm
	{
		CMP ECX, 0
		JE Skip
		MOV DWORD PTR DS:[ECX+0x10],EDX
Skip:
		MOV DWORD PTR DS:[EAX+0xC],0
		RETN
	}
}

void GameDrawOOG_Intercept(void)
{
	GameDrawOOG();
}

void __declspec(naked) GameActChange_Intercept(void)
{
	__asm
	{
		POP EAX
		PUSH EDI
		XOR EDI, EDI
		CMP [Vars.bChangedAct], 0
		MOV [Vars.bChangedAct], 0
		JMP EAX
	}
}

void __declspec(naked) GameActChange2_Intercept(void)
{
	__asm
	{
		MOV [Vars.bChangedAct], 1
		retn 4
	}
}

void __declspec(naked) GameLeave_Intercept(void)
{
	__asm
	{
		call GameLeave
		jmp D2CLIENT_GameLeave_I
	}
}

void __declspec(naked) ChannelInput_Intercept(void)
{
	__asm
	{
		push ecx
		mov ecx, esi

		call ChannelInput

		test eax, eax
		pop ecx

		jz SkipInput
		mov eax, dword ptr[esp+4]
		push eax
		call D2MULTI_ChannelInput_I

SkipInput:
		ret 4
	}
}

void __declspec(naked) ChannelWhisper_Intercept(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, edi
		mov edx, ebx

		call ChannelWhisperHandler

		//test eax, eax
		pop edx
		pop ecx

		//jz SkipWhisper
		jmp D2MULTI_ChannelWhisper_I

//SkipWhisper:
		ret 4
	}
}

void __declspec(naked) ChannelChat_Intercept(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, dword ptr ss:[esp+0xC]
		mov edx, dword ptr ss:[esp+0x10]

		call ChannelChatHandler

		//test eax, eax
		pop edx
		pop ecx

		//jz SkipChat
		sub esp, 0x308
		jmp D2MULTI_ChannelChat_I

//SkipChat:
		ret 8
	}
}

void __declspec(naked) ChannelEmote_Intercept(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, dword ptr ss:[esp+0xC]
		mov edx, dword ptr ss:[esp+0x10]

		call ChannelChatHandler

		//test eax, eax
		pop edx
		pop ecx

		//jz SkipChat
		sub esp, 0x4F8
		jmp D2MULTI_ChannelEmote_I

//SkipChat:
		ret 8
	}
}

void __declspec(naked) AddUnit_Intercept(UnitAny* lpUnit)
{
	__asm
	{
		call [D2CLIENT_GameAddUnit_I]
		pushad
		push esi
		call AddUnit
		popad
		retn
	}
}

void __declspec(naked) RemoveUnit_Intercept(UnitAny* lpUnit)
{
	__asm
	{
		pushad
		push dword ptr ds:[esi+edx*4]
		call RemoveUnit
		popad
		mov eax,dword ptr ds:[ecx+0xE4]
		mov DWORD PTR ds:[esi+edx*4], eax
		retn
	}
}

DWORD DClass = 0x6FF35EB8; 
DWORD DLod = 0x6FF361BD;

VOID __declspec(naked) __fastcall ClassicSTUB()
{
	__asm{
		lea eax,dword ptr ss:[Vars.szClassic]
		mov dword ptr ds:[0x6FF3E928],eax
		jmp DClass;
	}
	
}

VOID __declspec(naked) __fastcall LodSTUB()
{ 
	__asm{
		lea eax,dword ptr ss:[Vars.szLod]
		mov dword ptr ds:[0x6FF3E930],eax
		jmp DLod;
	}
	
}