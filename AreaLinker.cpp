#include <windows.h>

//#include "Constants.h"
#include "D2Structs.h"
#include "D2Helpers.h"
//#include "D2Ptrs.h"
//#include "Core.h"
#include "AreaLinker.h"

AreaNode* GetAreaNode(int nAreaId)
{
	for(int i = 0; i < ArraySize(AreaNodes); i++)
		for(int x = 0; x < AreaNodes[i].nCount; x++)
			if(AreaNodes[i].nAreas[x] == nAreaId)
				return &AreaNodes[i];

	return NULL;
}

int GetAreas(DWORD nArray[], int nSize, int nStartAreaId, WORD wX, WORD wY)
{
	int nDest = -1;
	int nCount = 0;

	AreaNode* pNode = GetAreaNode(nStartAreaId);

	if(pNode)
	{
		for(int i = 0; i < pNode->nCount; i++)
		{
			Level* pLevel = GetLevel(pNode->nAreas[i]);

			if(pLevel && pLevel->dwPosX * 5 <= wX && pLevel->dwPosY * 5 <= wY && pLevel->dwPosX * 5 + pLevel->dwSizeX * 5 >= wX && pLevel->dwPosY * 5 + pLevel->dwSizeY * 5 >= wY)
			{
				nDest = pLevel->dwLevelNo;
				break;
			}
		}

		nArray[nCount++] = nStartAreaId;

		if(nDest == -1 || nDest == nStartAreaId)
			return nCount;

		if(nStartAreaId > nDest)
		{
			for(int i = nStartAreaId - 1; i >= nDest; i--)
				nArray[nCount++] = i;
			
			if(nCount >= nSize)
				return 0;
		}
		else if (nStartAreaId < nDest)
		{
			for(int i = nStartAreaId + 1; i <= nDest; i++)
			{
				nArray[nCount++] = i;

				if(nCount >= nSize)
					return 0;
			}
		}
	}

	return nCount;
}
