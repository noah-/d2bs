// CollisionMap.cpp: implementation of the CCollisionMap class.
//
//////////////////////////////////////////////////////////////////////

#include "CollisionMap.h"
#include "D2Helpers.h"
#include "D2Ptrs.h"
#include "Constants.h"
#include "CriticalSections.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollisionMap::CCollisionMap()
{
	m_iCurMap = 0x00;
	::memset(&m_ptLevelOrigin, 0, sizeof(POINT));
}

CCollisionMap::~CCollisionMap()
{
}

void CCollisionMap::AddCollisionData(const CollMap* pCol)
{
	if (pCol == NULL)
		return;

	int x = pCol->dwPosGameX - m_ptLevelOrigin.x;
	int y = pCol->dwPosGameY - m_ptLevelOrigin.y;
	int cx = pCol->dwSizeGameX;
	int cy = pCol->dwSizeGameY;

	if (!m_map.IsValidIndex(x, y))
	{
		return;
	}	
	
	int nLimitX = x + cx;
	int nLimitY = y + cy;
	
	WORD* p = pCol->pMapStart;
	for (int j = y; j < nLimitY; j++)		
	{
		for (int i = x; i < nLimitX; i++)
		{
			m_map[i][j] = *p;
			if (m_map[i][j] == 1024)
				m_map[i][j] = MAP_DATA_AVOID;

			if (m_aCollisionTypes.Find(*p) == -1)
			{
				m_aCollisionTypes.Add(*p);
				m_aCollisionTypes.Sort();
			}

			p++;
		}
	}
}

BOOL CCollisionMap::IsValidAbsLocation(long x, long y) const
{
	if (!m_map.IsCreated())
		return FALSE;
	
	x -= m_ptLevelOrigin.x;
	y -= m_ptLevelOrigin.y;

	return m_map.IsValidIndex(x, y);
}

WORD CCollisionMap::GetMapData(long x, long y, BOOL bAbs) const
{
	if (!m_map.IsCreated())
		return (WORD)MAP_DATA_INVALID;
	
	if (bAbs)
	{
		x -= m_ptLevelOrigin.x;
		y -= m_ptLevelOrigin.y;
	}

	//m_map.Lock();
	WORD wVal = (WORD)MAP_DATA_INVALID;

	if (m_map.IsValidIndex(x, y))
		wVal = m_map[x][y];

	//m_map.Unlock();
	return wVal;
}

BOOL CCollisionMap::BuildMapData(DWORD AreaIds[], int nSize)
{
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();

	if (m_map.IsCreated())
		return TRUE;

	if(!pUnit)
		return FALSE;


	//Get the most left-top level for the base and the size of the entire wanted map
	Level* pBestLevel = GetLevel(AreaIds[0]);
	DWORD dwXSize = 0;
	DWORD dwYSize = 0;
	m_ptLevelOrigin.x = pBestLevel->dwPosX * 5;
	m_ptLevelOrigin.y = pBestLevel->dwPosY * 5;
	dwLevelId = AreaIds[0];

	//Loop all the given areas
	for (int n = 0; n < nSize; n++) {
		//Get the level struct for given id
		Level* pLevel = GetLevel(AreaIds[n]);
	
		//Make sure we have pLevel
		if (!pLevel)
			continue;

		//Check if this level is even more top-left then the current one
		if((m_ptLevelOrigin.x / 5) > (int)pLevel->dwPosX)
			m_ptLevelOrigin.x = pLevel->dwPosX * 5;

		if((m_ptLevelOrigin.y / 5) > (int)pLevel->dwPosY)
			m_ptLevelOrigin.y = pLevel->dwPosY * 5;

		//Add the size of the levels.
		dwXSize += pLevel->dwSizeX * 5;
		dwYSize += pLevel->dwSizeY * 5;
	}

	if (!m_map.Create(dwXSize, dwYSize, (WORD)MAP_DATA_INVALID))
		return FALSE;

	DwordArray aSkip;
	for (int n = 0; n < nSize; n++) {
		Level* pLevel = GetLevel(AreaIds[n]);
		if (!pLevel)
			continue;

		Search(pLevel->pRoom2First, pUnit, aSkip, AreaIds[n]);
	}
		
	FillGaps();
	FillGaps();

	return TRUE;
}

void CCollisionMap::Search(Room2 *ro, UnitAny* pPlayer, DwordArray &aSkip, DWORD dwScanArea)
{
	if (!ro || ro->pLevel->dwLevelNo != dwScanArea || aSkip.Find((DWORD)ro) != -1 || pPlayer == NULL)
		return;

	BOOL add_room=FALSE;
	if(!ro->pRoom1)
	{
		add_room=TRUE;
		D2COMMON_AddRoomData(pPlayer->pAct, ro->pLevel->dwLevelNo, ro->dwPosX, ro->dwPosY, pPlayer->pPath->pRoom1);
	}

	aSkip.Add((DWORD)ro);
	aSkip.Sort();

	if (ro->pRoom1)
	{
		AddCollisionData(ro->pRoom1->Coll);
	}
	
	Room2 **n = ro->pRoom2Near;
	for(UINT i=0; i < ro->dwRoomsNear; i++)
	{
		Search(n[i], pPlayer, aSkip, dwScanArea);
	}
	
	if(add_room)
	{
		D2COMMON_RemoveRoomData(pPlayer->pAct,ro->pLevel->dwLevelNo, ro->dwPosX, ro->dwPosY, pPlayer->pPath->pRoom1);
	}
}

BOOL CCollisionMap::CreateMap(DWORD AreaId[], int nSize)
{
	
	BOOL bOK = BuildMapData(AreaId, nSize);

	return bOK;
}
BOOL CCollisionMap::CreateMap(DWORD AreaId)
{
	DWORD dwAreas[] = {AreaId};
	return BuildMapData(dwAreas, 1);
}
POINT CCollisionMap::GetMapOrigin() const
{
	return m_ptLevelOrigin;
}

void CCollisionMap::AbsToRelative(POINT &pt) const
{
	pt.x -= m_ptLevelOrigin.x;
	pt.y -= m_ptLevelOrigin.y;
}

void CCollisionMap::RelativeToAbs(POINT &pt) const
{
	pt.x += m_ptLevelOrigin.x;
	pt.y += m_ptLevelOrigin.y;
}

BOOL CCollisionMap::DumpMap(LPCSTR lpszFilePath, const LPPOINT lpPath, DWORD dwCount) const
{
	if (lpszFilePath == NULL)
		return FALSE;

	FILE *fp = NULL;
	fopen_s(&fp, lpszFilePath, "w+");
	if(fp == NULL )
		return FALSE;	
	
	if (!m_map.IsCreated())
		return FALSE;

	//m_map.Lock();
	char szMapName[256] = "";

	fprintf(fp, "%s (Size: %d * %d)\nKnown Collision Types: ", szMapName, m_map.GetCX(), m_map.GetCY());
	for (int i = 0; i < m_aCollisionTypes.GetSize(); i++)
	{
		fprintf(fp, "%d, ", m_aCollisionTypes[i]);
	}

	fprintf(fp, "\n\n");

	POINT* pPath = NULL;
	if (lpPath && dwCount)
	{
		pPath = new POINT[dwCount];
		::memcpy(pPath, lpPath, dwCount * sizeof(POINT));
		for (DWORD i = 0; i < dwCount; i++)
			AbsToRelative(pPath[i]);
	}	

	UnitAny* Me = D2CLIENT_GetPlayerUnit();
	POINT ptPlayer = {Me->pPath->xPos,Me->pPath->yPos};
	AbsToRelative(ptPlayer);

	const int CX = m_map.GetCX();
	const int CY = m_map.GetCY();

	for (int y = 0; y < CY; y++)
	{		
		for (int x = 0; x < CX; x++)
		{
			char ch = IsMarkPoint(ptPlayer, x, y, pPath, dwCount);			

			if (!ch)
				ch = (m_map[x][y] % 2) ? 'X' : ' ';

			fprintf(fp, "%C", ch); // X - unreachable
		}

		fprintf(fp, "%c", '\n');
	}

	delete [] pPath;

	//m_map.Unlock();	
	fclose(fp);

	return TRUE;
}

BOOL CCollisionMap::CheckCollision(int x, int y) {
	if(!m_map.IsCreated())
		return FALSE;
	if(x > m_map.GetCX() || y > m_map.GetCY())
		return FALSE;
	BOOL Works = FALSE;
	Works = (m_map[x][y] % 2) ? FALSE : TRUE;
return Works;
}

char CCollisionMap::IsMarkPoint(const POINT& ptPlayer, int x, int y, const LPPOINT lpPath, DWORD dwCount) const
{	
	char ch = 0;
	if (x == ptPlayer.x && y == ptPlayer.y)
		ch = 'P';

	if (lpPath == NULL || dwCount == 0)
		return ch;

	for (DWORD i = 0; i < dwCount; i++)
	{
		if (lpPath[i].x == x && lpPath[i].y == y)
		{
			if (i == 0)
				return 'S'; // start
			else if (i == dwCount - 1)
				return 'E'; // end
			else
				return '*'; // nodes
		}
	}

	return ch;
}

SIZE CCollisionMap::GetMapSize() const
{
	SIZE cz = {0};
	if (m_map.IsCreated())
	{
		cz.cx = m_map.GetCX();
		cz.cy = m_map.GetCY();
	}
	return cz;
}

SIZE CCollisionMap::CopyMapData(WORD** ppBuffer, int cx, int cy) const
{
	SIZE copied = {0};
	//m_map.Lock();
	copied = m_map.ExportData(ppBuffer, cx, cy);
	//m_map.Unlock();
	return copied;
}

BOOL CCollisionMap::IsGap(int x, int y) 
{
	if (m_map[x][y] % 2)
		return FALSE;

	int nSpaces = 0;
	int i = 0;

	// Horizontal check
	for (i = x - 2; i <= x + 2 && nSpaces < 3; i++)
	{
		if ( i < 0 || i >= m_map.GetCX() || (m_map[i][y] % 2))
			nSpaces = 0;
		else
			nSpaces++;
	}

	if (nSpaces < 3)
		return TRUE;

	// Vertical check
	nSpaces = 0;
	for (i = y - 2; i <= y + 2 && nSpaces < 3; i++)
	{
		if ( i < 0 || i >= m_map.GetCY() || (m_map[x][i] % 2))
			nSpaces = 0;
		else
			nSpaces++;
	}

	return nSpaces < 3;
}

void CCollisionMap::FillGaps()
{
	if (!m_map.IsCreated())
		return;

	//m_map.Lock();

	const int CX = m_map.GetCX();
	const int CY = m_map.GetCY();

	for (int x = 0; x <CX; x++)
	{
		for (int y = 0; y < CY; y++)
		{
			if (IsGap(x, y))
				m_map[x][y] = MAP_DATA_FILLED;
		}
	}

	//m_map.Unlock();
}

void CCollisionMap::MakeBlank(WordMatrix& rMatrix, POINT pos)
{
	if (!rMatrix.IsCreated())
		return;

	for (int i = pos.x - 1; i <= pos.x + 1; i++)
	{
		for (int j = pos.y - 1; j <= pos.y + 1; j++)
		{
			if (rMatrix.IsValidIndex(i, j))
				rMatrix[i][j] = MAP_DATA_CLEANED;
		}
	}
}

BOOL CCollisionMap::ThickenWalls(WordMatrix& rMatrix, int nThickenBy)
{
	if (!rMatrix.IsCreated() || nThickenBy <= 0)
		return FALSE;

	const int CX = rMatrix.GetCX();
	const int CY = rMatrix.GetCY();
	
	for (int i = 0; i < CX; i++)
	{
		for (int j = 0; j < CY; j++)
		{
			if ((rMatrix[i][j] % 2) == 0 || rMatrix[i][j] == MAP_DATA_THICKENED)
				continue;

			for (int x = i - nThickenBy; x <= i + nThickenBy; x++)
			{
				for (int y = j - nThickenBy; y <= j + nThickenBy; y++)
				{
					if (!rMatrix.IsValidIndex(x, y))
						continue;

					if ((rMatrix[x][y] % 2) == 0)
						rMatrix[x][y] = MAP_DATA_THICKENED;
				}
			}
		}
	}

	return TRUE;
}

void CCollisionMap::DestroyMap()
{
	//m_map.Lock();
	m_map.Destroy();
	//m_map.Unlock();
	m_iCurMap = 0x00;
	m_aCollisionTypes.RemoveAll();
	::memset(&m_ptLevelOrigin, 0, sizeof(POINT));
}

BOOL CCollisionMap::CopyMapData(WordMatrix& rBuffer) const
{
	//m_map.Lock();
	m_map.ExportData(rBuffer);
	//m_map.Unlock();
	return rBuffer.IsCreated();
}

BOOL CCollisionMap::ReportCollisionType(POINT ptOrigin, long lRadius) const
{
	if (!m_map.IsCreated() || lRadius < 1)
		return FALSE;

	//m_map.Lock();
	WordArray aList;
	AbsToRelative(ptOrigin);
	for (int i = ptOrigin.x - lRadius; i <= ptOrigin.x + lRadius; i++)
	{
		for (int j = ptOrigin.y - lRadius; j <= ptOrigin.y + lRadius; j++)
		{
			if (!m_map.IsValidIndex(i, j))
				continue;

			if (aList.Find(m_map[i][j]) == -1)
				aList.Add(m_map[i][j]);
		}
	}

	//m_map.Unlock();
	aList.Sort();

	char sz[256] = "";
	for (int n = 0; n < aList.GetSize(); n++)
	{
		char buf[32] = "";
		_itoa_s(aList[n], buf, sizeof(buf), 10);
		strcat_s(sz, sizeof(sz), buf);
		strcat_s(sz, sizeof(sz), ", ");
	}

	
	return aList.GetSize();
}

int CCollisionMap::GetLevelExits(LPLevelExit* lpLevel)
{
	POINT ptExitPoints[0x40][2];
	int nTotalPoints = 0, nCurrentExit = 0;
	int nMaxExits = 0x40;
	UnitAny* Me = D2CLIENT_GetPlayerUnit();

	CriticalRoom myRoom;
	myRoom.EnterSection();

	for(int i = 0; i < m_map.GetCX(); i++)
	{
		if(!(m_map[i][0] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = i;
			ptExitPoints[nTotalPoints][0].y = 0;

			for(i++; i < m_map.GetCX(); i++)
			{
				if(m_map[i][0] % 2)
				{
					ptExitPoints[nTotalPoints][1].x = i - 1;
					ptExitPoints[nTotalPoints][1].y = 0;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	for(int i = 0; i < m_map.GetCX(); i++)
	{
		if(!(m_map[i][m_map.GetCY() - 1] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = i;
			ptExitPoints[nTotalPoints][0].y = m_map.GetCY() - 1;

			for(i++; i < m_map.GetCX(); i++)
			{
				if((m_map[i][m_map.GetCY() - 1] % 2))
				{
					ptExitPoints[nTotalPoints][1].x = i - 1;
					ptExitPoints[nTotalPoints][1].y = m_map.GetCY() - 1;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	for(int i = 0; i < m_map.GetCY(); i++)
	{
		if(!(m_map[0][i] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = 0;
			ptExitPoints[nTotalPoints][0].y = i;

			for(i++; i < m_map.GetCY(); i++)
			{
				if((m_map[0][i] % 2))
				{
					ptExitPoints[nTotalPoints][1].x = 0;
					ptExitPoints[nTotalPoints][1].y = i - 1;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	for(int i = 0; i < m_map.GetCY(); i++)
	{
		if(!(m_map[m_map.GetCX() - 1][i] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = m_map.GetCX() - 1;
			ptExitPoints[nTotalPoints][0].y = i;

			for(i++; i < m_map.GetCY(); i++)
			{
				if((m_map[m_map.GetCX() - 1][i] % 2))
				{
					ptExitPoints[nTotalPoints][1].x = m_map.GetCX() - 1;
					ptExitPoints[nTotalPoints][1].y = i - 1;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	LPPOINT ptCenters = new POINT[nTotalPoints];
	for(int i = 0; i < nTotalPoints; i++)
	{
		int nXDiff = ptExitPoints[i][1].x - ptExitPoints[i][0].x;
		int nYDiff = ptExitPoints[i][1].y - ptExitPoints[i][0].y;
		int nXCenter = 0, nYCenter = 0;

		if(nXDiff > 0)
		{
			if(nXDiff % 2)
				nXCenter = ptExitPoints[i][0].x + ((nXDiff - (nXDiff % 2)) / 2);
			else
				nXCenter = ptExitPoints[i][0].x + (nXDiff / 2);
		}

		if(nYDiff > 0)
		{
			if(nYDiff % 2)
				nYCenter = ptExitPoints[i][0].y + ((nYDiff - (nYDiff % 2)) / 2);
			else
				nYCenter = ptExitPoints[i][0].y + (nYDiff / 2);
		}

		ptCenters[i].x = nXCenter ? nXCenter : ptExitPoints[i][0].x;
		ptCenters[i].y = nYCenter ? nYCenter : ptExitPoints[i][0].y;
	}

	

	for(Room2* pRoom = GetLevel(dwLevelId)->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
	{
		Room2* *pNear = pRoom->pRoom2Near;
		for(DWORD i = 0; i < pRoom->dwRoomsNear; i++)
		{
			if(pNear[i]->pLevel->dwLevelNo != dwLevelId)
			{
				int nRoomX = pRoom->dwPosX * 5;
				int nRoomY = pRoom->dwPosY * 5;

				for(int j = 0; j < nTotalPoints; j++)
				{
					if((ptCenters[j].x + m_ptLevelOrigin.x) >= (WORD)nRoomX && (ptCenters[j].x + m_ptLevelOrigin.x) <= (WORD)(nRoomX + (pRoom->dwSizeX * 5)))
					{
						if((ptCenters[j].y + m_ptLevelOrigin.y) >= (WORD)nRoomY && (ptCenters[j].y + m_ptLevelOrigin.y) <= (WORD)(nRoomY + (pRoom->dwSizeY * 5)))
						{
							if(nCurrentExit >= nMaxExits)
							{
//								LeaveCriticalSection(&CriticalSection);
								return FALSE;
							}
							
							lpLevel[nCurrentExit] = new LevelExit;
							lpLevel[nCurrentExit]->dwTargetLevel = pNear[i]->pLevel->dwLevelNo;
							lpLevel[nCurrentExit]->ptPos.x = ptCenters[j].x + m_ptLevelOrigin.x;
							lpLevel[nCurrentExit]->ptPos.y = ptCenters[j].y + m_ptLevelOrigin.y;
							lpLevel[nCurrentExit]->dwType = EXIT_LEVEL;
							lpLevel[nCurrentExit]->dwId = 0;
							nCurrentExit++;
						}
					}
				}

				break;
			}
		}

		BOOL bAdded = FALSE;

		if(!pRoom->pRoom1)
		{
			D2COMMON_AddRoomData(Me->pAct, pRoom->pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, Me->pPath->pRoom1);
			bAdded = TRUE;
		}
			
			for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
			{

				if(nCurrentExit >= nMaxExits)
				{
					if(bAdded)
						D2COMMON_RemoveRoomData(Me->pAct, pRoom->pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, Me->pPath->pRoom1);
//					LeaveCriticalSection(&CriticalSection);
					return FALSE;
				}

				if(pUnit->dwType == UNIT_TILE)
				{
					DWORD dwTargetLevel = GetTileLevelNo(pRoom, pUnit->dwTxtFileNo);
					
					if(dwTargetLevel)
					{
						BOOL bExists = FALSE;

						for(int i = 0; i < nCurrentExit; i++)
							{
								if(((DWORD)lpLevel[i]->ptPos.x == (pRoom->dwPosX * 5) + pUnit->dwPosX) && 
									((DWORD)lpLevel[i]->ptPos.y == (pRoom->dwPosY * 5) + pUnit->dwPosY))
									bExists = TRUE;
							}

						if(!bExists)
						{
							lpLevel[nCurrentExit] = new LevelExit;
							lpLevel[nCurrentExit]->dwTargetLevel = dwTargetLevel;
							lpLevel[nCurrentExit]->ptPos.x = (pRoom->dwPosX * 5) + pUnit->dwPosX;
							lpLevel[nCurrentExit]->ptPos.y = (pRoom->dwPosY * 5) + pUnit->dwPosY;
							lpLevel[nCurrentExit]->dwType = EXIT_TILE;
							lpLevel[nCurrentExit]->dwId = pUnit->dwTxtFileNo;
							nCurrentExit++;
						}
					}
				}
			}

		if(bAdded)
			D2COMMON_RemoveRoomData(Me->pAct, pRoom->pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, Me->pPath->pRoom1);
	}
delete[] ptCenters;
//	LeaveCriticalSection(&CriticalSection);

	return nCurrentExit;
}
