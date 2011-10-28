#pragma once

#include <windows.h>

DWORD ReassignPlayerHandler(BYTE* pPacket, DWORD dwSize);
DWORD HPMPUpdateHandler(BYTE* pPacket, DWORD dwSize);
DWORD ChatEventHandler(BYTE* pPacket, DWORD dwSize);
DWORD NPCTransactionHandler(BYTE* pPacket, DWORD dwSize);
DWORD EventMessagesHandler(BYTE* pPacket, DWORD dwSize);
DWORD ItemActionHandler(BYTE* pPacket, DWORD dwSize);
DWORD DelayedStateHandler(BYTE* pPacket, DWORD dwSize);
