///////////////////////////////////////////////////////////
// TeleportPath.cpp
//
// Abin (abinn32@yahoo.com)
///////////////////////////////////////////////////////////

#include <stdio.h>

#include "TeleportPath.h"
#include "Common.h"

#define RANGE_INVALID	10000  // invalid range flag
int TP_RANGE = 35;

/////////////////////////////////////////////////////////////////////
// Path Finding Result
/////////////////////////////////////////////////////////////////////
enum {   PATH_FAIL = 0,     // Failed, error occurred or no available path
		 PATH_CONTINUE,	    // Path OK, destination not reached yet
		 PATH_REACHED };    // Path OK, destination reached(Path finding completed successfully)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTeleportPath::CTeleportPath(WORD** pCollisionMap, int cx, int cy)
{
	m_ppTable = pCollisionMap;
	m_nCX = cx;
	m_nCY = cy;
	::memset(&m_ptStart, 0, sizeof(POINT));
	::memset(&m_ptEnd, 0, sizeof(POINT));
}

CTeleportPath::~CTeleportPath()
{
}

BOOL CTeleportPath::MakeDistanceTable()
{	
	if (m_ppTable == NULL)
		return FALSE;

	// convert the graph into a distance table
	for (int x = 0; x < m_nCX; x++)	
	{
		for (int y = 0; y < m_nCY; y++)
		{
			if ((m_ppTable[x][y] % 2) == 0)
				m_ppTable[x][y] = (short)CalculateDistance(x, y, m_ptEnd.x, m_ptEnd.y);
			else
				m_ppTable[x][y] = RANGE_INVALID;
		}
	}

	m_ppTable[m_ptEnd.x][m_ptEnd.y] = 1;	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// The "Get Best Move" Algorithm
//
// Originally developed by Niren7, Modified by Abin
/////////////////////////////////////////////////////////////////////
BOOL CTeleportPath::GetBestMove(POINT& pos, int nAdjust)
{	
	if(CalculateDistance(m_ptEnd, pos) <= TP_RANGE)
	{
		pos = m_ptEnd;
		return PATH_REACHED; // we reached the destination
	}

	if (!IsValidIndex(pos.x, pos.y))
		return PATH_FAIL; // fail

	Block(pos, nAdjust);

	POINT p, best = {0};
	int value = RANGE_INVALID;

	for (p.x = pos.x - TP_RANGE; p.x <= pos.x + TP_RANGE; p.x++)
	{
		for (p.y = pos.y - TP_RANGE; p.y <= pos.y + TP_RANGE; p.y++)
		{			
			if (!IsValidIndex(p.x, p.y))
				continue;
		
			if (m_ppTable[p.x][p.y] < value && CalculateDistance(p, pos) <= TP_RANGE)
			{				
				value = m_ppTable[p.x][p.y];
				best = p;					
			}			
		}
	}

	if (value >= RANGE_INVALID)
		return PATH_FAIL; // no path at all	
	
	pos = best;
	Block(pos, nAdjust);	
	return PATH_CONTINUE; // ok but not reached yet
}

DWORD CTeleportPath::FindTeleportPath(POINT ptStart, POINT ptEnd, LPPOINT lpBuffer, DWORD dwMaxCount, int Range)
{

	if (lpBuffer == NULL || dwMaxCount == 0 || m_nCX <= 0 || m_nCY <= 0 || m_ppTable == NULL)
		return 0;

	unsigned int oldRange = TP_RANGE;
	TP_RANGE = Range;

	memset(lpBuffer, 0, sizeof(POINT) * dwMaxCount);
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;

	//GameInfof("start %d %d End %d %d", ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);

	MakeDistanceTable();
	//DumpDistanceTable("c:\\map2.txt");
	 
	lpBuffer[0] = ptStart; // start point
	DWORD dwFound = 1;

	POINT pos = ptStart;

	BOOL bOK = FALSE;
	int nRes = GetBestMove(pos);
	while (nRes != PATH_FAIL && dwFound < dwMaxCount)
	{
		// Reached?
		if (nRes == PATH_REACHED)
		{
			bOK = TRUE;
			lpBuffer[dwFound] = ptEnd;
			dwFound++;
			break; // Finished
		}

		// Perform a redundancy check
		int nRedundancy = GetRedundancy(lpBuffer, dwFound, pos);
		if (nRedundancy == -1)
		{
			// no redundancy
			lpBuffer[dwFound] = pos;
			dwFound++;
		}
		else
		{
			// redundancy found, discard all redundant steps
			dwFound = nRedundancy + 1;
			lpBuffer[dwFound] = pos;
		}	

		nRes = GetBestMove(pos);
	}	

	if (!bOK)
		dwFound = 0;

	TP_RANGE = oldRange;
	return dwFound;
}

void CTeleportPath::Block(POINT pos, int nRange)
{
	nRange = max(nRange, 1);

	for (int i = pos.x - nRange; i < pos.x + nRange; i++)
	{
		for (int j = pos.y - nRange; j < pos.y + nRange; j++)
		{
			if (IsValidIndex(i, j))
				m_ppTable[i][j] = RANGE_INVALID;
		}
	}
}

int CTeleportPath::GetRedundancy(const LPPOINT lpPath, DWORD dwMaxCount, const POINT &pos)
{
	// step redundancy check
	if (lpPath == NULL || dwMaxCount == 0)
		return -1;

	for (DWORD i = 1; i < dwMaxCount; i++)
	{
		if (CalculateDistance(lpPath[i].x, lpPath[i].y, pos.x, pos.y) <= TP_RANGE / 2)
			return i;
	}

	return -1;
}

BOOL CTeleportPath::IsValidIndex(int x, int y) const
{
	return x >= 0 && x < m_nCX && y >= 0 && y < m_nCY;
}

BOOL CTeleportPath::DumpDistanceTable(LPCSTR lpszFilePath) const
{
	if (lpszFilePath == NULL || m_ppTable == NULL)
		return FALSE;

	FILE *fp = NULL;
	fopen_s(&fp, lpszFilePath, "w+");
	if(fp == NULL )
		return FALSE;

	for (int y = 0; y < m_nCY; y++)
	{
		for (int x = 0; x < m_nCX; x++)
		{
			if (m_ptStart.x == x && m_ptStart.y == y)
			{
				fprintf(fp, "%s ", "St");
			}
			else if (m_ptEnd.x == x && m_ptEnd.y == y)
			{
				fprintf(fp, "%s ", "En");
			}
			else
			{
				short iDis = m_ppTable[x][y];

				if (iDis == RANGE_INVALID)
					fprintf(fp, "%s ", "  ");
				else if (iDis >= 255)
					fprintf(fp, "%s ", "??");
				else
					fprintf(fp, "%02X ", iDis);		
			}					
		}

		fprintf(fp, "%c", '\n');
	}

	fclose(fp);
	return TRUE;
}
