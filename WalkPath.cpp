/////////////////////////////////////////////////////////////////////
//
//  WalkPath.cpp 
//                   
//  Ustc_tweeg < ustc_tweeg@tom.com >
//
/////////////////////////////////////////////////////////////////////

#include "WalkPath.h"
#include "Common.h"

// #define __SHOW_DEBUG_INFO__ // comment this line if you do not want to output debug messages

#ifndef __SHOW_DEBUG_INFO__
	#define __PRINTF DummyMsg
#else
	#ifndef __IN_DIABLOII__
		#define __PRINTF printf
	#else
		#define __PRINTF Print
	#endif
#endif

#define STRAIGHTEN_A 6
#define STRAIGHTEN_B 3
#define STRAIGHTEN_C 2
unsigned int MAX_WALK_RANGE = 15;

struct DIRECT2DATA
{
	BOOL bDirect;
	const CWalkPath* pClass;
	CArrayEx<POINT, const POINT&>* pSubPath;
};

// deta-x, deta-y, counterclockwise, clockwise, right-wall-next-point, left-wall-next-point

static const int Nearby[8][6] =	{   { 1, 0,   1, 7,   6, 2 },
									{ 1, 1,   2, 0,   0, 2 },
									{ 0, 1,   3, 1,   0, 4 },
									{-1, 1,   4, 2,   2, 4 },
									{-1, 0,   5, 3,   2, 6 },
									{-1,-1,   6, 4,   4, 6 },
									{ 0,-1,   7, 5,   4, 0 },
									{ 1,-1,   0, 6,   6, 0 } };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int DummyMsg(LPCSTR lpszFmt, ...)
{
	return 0; // silent to screen messages
}


CWalkPath::CWalkPath(WORD** Map, int MapSizeX, int MapSizeY)
{

	m_Map = Map;

	m_MapSizeX = MapSizeX;
	m_MapSizeY = MapSizeY;

	m_Count = 0;

	m_FollowWallSuccess = TRUE;
}

CWalkPath::~CWalkPath()
{

}

///////////////////////////////////////////////////////////////////////
// Private Functions
///////////////////////////////////////////////////////////////////////

BOOL CWalkPath::IsBorder(POINT pt)
{
	return ( pt.x<0 || pt.y<0 || pt.x >= m_MapSizeX || pt.y >= m_MapSizeY);
}

BOOL CWalkPath::IsBarrier(POINT pt) const
{
	return (m_Map[pt.x][pt.y] % 2);
}

void CWalkPath::FollowTheRope(POINT Dot)
{
	ContinuousPath.Add(Dot);
}

BOOL CWalkPath::FollowTheWall(BOOL RightWall, POINT FollowEnd)
{
	POINT NearbyC,NearbyA, NearbyB,TempCurrent;
	int m,n; // follow-right-wall-nearbya, follow-left-wall-nearbya
	TempCurrent.x = TempCurrent.y = 0;

	if(RightWall)
	{	
		//__PRINTF("right current %d %d\n" , m_RCurrent.x,m_RCurrent.y);
		m = m_RStart;
		for(int i=0; i<8; i++)
		{
			NearbyC.x = m_RCurrent.x + Nearby [Nearby[m][2]] [0];
			NearbyC.y = m_RCurrent.y + Nearby [Nearby[m][2]] [1];
			NearbyA.x = m_RCurrent.x + Nearby [m] [0];
			NearbyA.y = m_RCurrent.y + Nearby [m] [1];
			NearbyB.x = m_RCurrent.x + Nearby [Nearby[m][3]] [0];
			NearbyB.y = m_RCurrent.y + Nearby [Nearby[m][3]] [1];

			if( NearbyA.x == FollowEnd.x && NearbyA.y == FollowEnd.y) return true;

			//__PRINTF("right nearbya %d %d\n" , NearbyA.x,NearbyA.y);

			if( !(IsBorder(NearbyA) || IsBarrier(NearbyA))
				&& ( IsBorder(NearbyB) || IsBarrier(NearbyB)) )
			{ 
				if(!TempCurrent.x && !TempCurrent.y)
				{
				TempCurrent = NearbyA;

				if(!(IsBorder(NearbyC) || IsBarrier(NearbyC)))
					RightWallPath.Add(NearbyC);
				else 
					RightWallPath.Add(NearbyA);

				m_RStart = Nearby[m][4];
				}
			}
			m = Nearby[m][2];
		}
		m_RCurrent = TempCurrent;
		TempCurrent.x = TempCurrent.y = 0;
	}
	else
	{	
		n = m_LStart;
		//__PRINTF("left current %d %d\n" , m_LCurrent.x,m_LCurrent.y);
		for(int i=0; i<8; i++)
		{
			NearbyC.x = m_LCurrent.x + Nearby [Nearby[n][3]] [0];
			NearbyC.y = m_LCurrent.y + Nearby [Nearby[n][3]] [1];
			NearbyA.x = m_LCurrent.x + Nearby [n] [0];
			NearbyA.y = m_LCurrent.y + Nearby [n] [1];
			NearbyB.x = m_LCurrent.x + Nearby [Nearby[n][2]] [0];
			NearbyB.y = m_LCurrent.y + Nearby [Nearby[n][2]] [1];

			if( NearbyA.x == FollowEnd.x && NearbyA.y == FollowEnd.y) return true;

			//__PRINTF("left nearbya %d %d\n" , NearbyA.x,NearbyA.y);
			if( !(IsBorder(NearbyA) || IsBarrier(NearbyA))
				&& ( IsBorder(NearbyB) || IsBarrier(NearbyB)))
			{
				if(!TempCurrent.x && !TempCurrent.y)
				{
				TempCurrent = NearbyA;

				if(!(IsBorder(NearbyC) || IsBarrier(NearbyC)))
					LeftWallPath.Add(NearbyC);
				else 
					LeftWallPath.Add(NearbyA);
				m_LStart = Nearby[n][5];
				}
			}
			n = Nearby[n][3];
		}
		m_LCurrent = TempCurrent;
		TempCurrent.x = TempCurrent.y = 0;
		
	}
	return false;
}

void CALLBACK CWalkPath::FindPathProc(int x, int y, LPARAM pWp) 
{ 
	CWalkPath* p = (CWalkPath*)pWp;

	POINT TempPoint;
	TempPoint.x = x; 
	TempPoint.y = y;

	if( p->IsBarrier(TempPoint) && !p->IsBarrier(p->m_LastLineDot) && p->m_FollowWallSuccess )
	{
		p->m_Seperate = p->m_LastLineDot;
		for(int i=0; i<8; i++)
		{
			if((TempPoint.x- p->m_LastLineDot.x)==Nearby[i][0] 
				&& (TempPoint.y- p->m_LastLineDot.y)==Nearby[i][1])
				p->m_LStart = p->m_RStart = i;
		}
		__PRINTF("seperate x=%d y=%d\n",p->m_Seperate.x,p->m_Seperate.y);
	}
	else if(!p->IsBarrier(TempPoint) ) 
	{
		if( p->IsBarrier(p->m_LastLineDot))
		{
			p->m_Meet = TempPoint;
			__PRINTF("meet x=%d y=%d\n",p->m_Meet.x,p->m_Meet.y);
			p->m_LCurrent = p->m_RCurrent = p->m_Seperate;
			p->SaveFollowWallPath();
		}
		if(p->m_FollowWallSuccess) p->FollowTheRope(TempPoint);
	}
	p->m_LastLineDot = TempPoint;		
	
} 

void CWalkPath::SaveFollowWallPath()
{
	for(int i=0; i<3000; i++)//3000
	{
		if(FollowTheWall(TRUE, m_Meet))
		{
			for(int j=0; j< RightWallPath.GetSize(); j++)
				ContinuousPath.Add( RightWallPath[j]);

			RightWallPath.RemoveAll();
			LeftWallPath.RemoveAll();
			m_FollowWallSuccess = TRUE;

			return;
		}
		
		if(FollowTheWall(FALSE,m_Meet))
		{
			for(int j=0; j< LeftWallPath.GetSize(); j++)
				ContinuousPath.Add( LeftWallPath[j]);

			RightWallPath.RemoveAll();
			LeftWallPath.RemoveAll();
			m_FollowWallSuccess = TRUE;

			return;
		}
	}

	RightWallPath.RemoveAll();
	LeftWallPath.RemoveAll();
	m_FollowWallSuccess = FALSE;
}

void CWalkPath::StraightenThePath(LPPOINT Path, DWORD dwMaxCount)
{

	CArrayEx<POINT, const POINT&> TempPathA;
	CArrayEx<POINT, const POINT&> TempPathB;

	int TempCountB,TempCountC;
	int i,j;

	TempCountB = TempCountC = 0;

	__PRINTF("ContinuousCount %d\n", ContinuousPath.GetSize());

	// first straighten
	for( i=STRAIGHTEN_A; i<ContinuousPath.GetSize(); i+=STRAIGHTEN_A )
	{
		if( IsDirect(ContinuousPath[i-STRAIGHTEN_A], ContinuousPath[i]) )
		{
			TempPathA.Add(ContinuousPath[i-STRAIGHTEN_A]);
		}
		else 
		{
			for(j=i-STRAIGHTEN_A; j<i; j++)
				TempPathA.Add(ContinuousPath[j]);
		}
	}

	if( IsDirect(ContinuousPath[i-STRAIGHTEN_A], ContinuousPath.GetLast()) )
		{
			TempPathA.Add(ContinuousPath[i-STRAIGHTEN_A]);
			TempPathA.Add(ContinuousPath.GetLast());
		}
	else 
		{
			for( j=i-STRAIGHTEN_A; j<ContinuousPath.GetSize(); j++)
				TempPathA.Add(ContinuousPath[j]);
		}

	//2nd straighten

	for( i=STRAIGHTEN_B; i<TempPathA.GetSize(); i+=STRAIGHTEN_B )
	{
		if( IsDirect(TempPathA[i-STRAIGHTEN_B], TempPathA[i]) )
		{
			TempPathB.Add(TempPathA[i-STRAIGHTEN_B]);
		}
		else 
		{
			for(j=i-STRAIGHTEN_B; j<i; j++)
				TempPathB.Add(TempPathA[j]);
		}
	}

	if( IsDirect(TempPathA[i-STRAIGHTEN_B], TempPathA.GetLast()) )
		{
			TempPathB.Add(TempPathA[i-STRAIGHTEN_B]);
			TempPathB.Add(TempPathA.GetLast());
		}
	else 
		{
			for( j=i-STRAIGHTEN_B; j<TempPathA.GetSize(); j++)
				TempPathB.Add(TempPathA[j]);
		}

	//3rd straighten
	for( i=STRAIGHTEN_C; i<TempPathB.GetSize(); i+=STRAIGHTEN_C )
	{
		if( IsDirect(TempPathB[i-STRAIGHTEN_C], TempPathB[i]) )
		{
			if(m_Count<(int)dwMaxCount)
				Path[m_Count++] = TempPathB[i-STRAIGHTEN_C]; 
		}
		else 
		{
			for(j=i-STRAIGHTEN_C; j<i; j++)
				if(m_Count<(int)dwMaxCount)
					Path[m_Count++] = TempPathB[j];
		}
	}

	if( IsDirect(TempPathB[i-STRAIGHTEN_C], TempPathB.GetLast()))
		{
			if(m_Count<(int)dwMaxCount)
				Path[m_Count++] =TempPathB[i-STRAIGHTEN_C];
		//	if(m_Count<dwMaxCount)
		//		Path[m_Count++] = TempPathB[TempCountB-1];
		}
	else 
		{
			for( j=i-STRAIGHTEN_C; j<TempPathB.GetSize(); j++)
				if(m_Count<(int)dwMaxCount)
					Path[m_Count++] = TempPathB[j];
		}

}

BOOL CWalkPath::IsDirect(POINT pt1, POINT pt2)
{
	m_Direct = TRUE;
	LineDDA(pt1.x,pt1.y,pt2.x,pt2.y,IsDirectProc,(long)this);

	return m_Direct;
}

void CALLBACK CWalkPath::IsDirectProc(int x, int y, LPARAM lpData) 
{ 
	CWalkPath* p = (CWalkPath*)lpData; 
	POINT TempPoint;

	TempPoint.x = x;
	TempPoint.y = y;

	if (p->IsBarrier(TempPoint)) p->m_Direct = FALSE;
}

///////////////////////////////////////////////////////////////////////
// Public Functions
///////////////////////////////////////////////////////////////////////

int CWalkPath::FindWalkPath (POINT Start, POINT End, LPPOINT Path, DWORD dwMaxCount, int Range, bool Reduction)
{
	m_Start = m_LastLineDot = Start;
	m_End = End;

	unsigned int oldRange = MAX_WALK_RANGE;
	MAX_WALK_RANGE = Range;

	if(!(IsBorder(End)|| IsBorder(Start)))
	{
		__PRINTF("start x=%d y=%d\n",m_Start.x,m_Start.y);
		LineDDA(Start.x, Start.y, End.x, End.y, FindPathProc, (long)this);
		FindPathProc(End.x, End.y, (long)this);
		__PRINTF("end x=%d y=%d\n",m_End.x,m_End.y);

		if(m_FollowWallSuccess)
		{
		/*	FILE *fp = fopen("a.txt", "w+");
			for (int  y = 0; y < m_MapSizeY; y ++)
			{		
				for (int  x= 0;  x< m_MapSizeX; x ++)
				{
					char ch=0;
					for(int k=0; k<ContinuousPath.GetSize();k++)
						if( x==ContinuousPath[k].x && y==ContinuousPath[k].y )ch='*';
					if (!ch)
						ch = (m_Map[x][y] % 2) ? 'X' : ' ';
					fprintf(fp, "%C", ch); // X - unreachable
				}
				fprintf(fp, "%c", '\n');
			}
			fclose(fp);*/

			if (Reduction)
			{
				StraightenThePath(Path,dwMaxCount);
				__PRINTF("after Straighten: %d\n", m_Count);

				m_Count = FurtherStraighten(Path, m_Count);
				__PRINTF("after FurtherStraighten: %d\n", m_Count);
			}
			else
			{
			for(int j=0; j<ContinuousPath.GetSize(); j++)
				if(m_Count<(int)dwMaxCount)
					Path[m_Count++] = ContinuousPath[j];
			}
		}
	}

	MAX_WALK_RANGE = oldRange;
	return m_Count;
}

//////////////////////////////////////////////////////////////////////
// FurtherStraighten       provided by Abin
//--------------------------------------------------------------------
// Further removal of redundant steps.
//////////////////////////////////////////////////////////////////////

DWORD CWalkPath::FurtherStraighten(LPPOINT lpPath, DWORD dwMaxCount)
{
	if (lpPath == NULL || dwMaxCount < 3)
		return dwMaxCount;

	CArrayEx<POINT, const POINT&> aTemp;
	aTemp.SetSize(dwMaxCount);
	for (int t = 0; t < (int)dwMaxCount; t++)
		aTemp[t] = lpPath[t];

	// straighten the path
	for (int nStart = 0; nStart + 2 < aTemp.GetSize(); nStart++)
	{
		for (int nEnd = aTemp.GetSize() - 1; nEnd > nStart + 1; nEnd--)
		{
			if (IsDirect(aTemp[nStart], aTemp[nEnd]))
			{
				aTemp.RemoveAt(nStart + 1, nEnd - nStart - 1);
				break;
			}
		}
	}

	if (aTemp.GetSize() >= (int)dwMaxCount)
		return dwMaxCount;

	// split long paths
	CArrayEx<POINT, const POINT&> aSubPath;
	for (int i = 0; i + 1 < aTemp.GetSize(); i++)
	{
		int nSubPath = Split(aTemp[i], aTemp[i + 1], aSubPath);
		if (nSubPath)
		{
			aTemp.InsertAt(i + 1, &aSubPath);
			i += nSubPath;
		}
	}

	// output
	const DWORD OUTPUT = min((DWORD)aTemp.GetSize(), dwMaxCount);
	::memcpy(lpPath, aTemp.GetData(), OUTPUT * sizeof(POINT));
	return OUTPUT;
}

int CWalkPath::Split(const POINT &pt1, const POINT &pt2, CArrayEx<POINT, const POINT&> &aSubPath)
{
	aSubPath.RemoveAll();
	const DWORD DISTANCE = CalculateDistance(pt1, pt2);
	if (DISTANCE <= MAX_WALK_RANGE)
		return 0;

	DWORD dwSubCount = DISTANCE / MAX_WALK_RANGE;
	if ((DISTANCE % MAX_WALK_RANGE) == 0)
		dwSubCount--;

	const int ANGLE = CalculateAngle(pt1, pt2);

	for (DWORD i = 0; i < dwSubCount; i++)
		aSubPath.Add(CalculatePointOnTrack(pt1, MAX_WALK_RANGE * (i + 1), ANGLE));

	return aSubPath.GetSize();
}
