/////////////////////////////////////////////////////////////////////////
//
//  WalkPath.h
//
//  Labyrinth Pathing Algorithm by Ustc_tweeg 
//
//  Ustc_tweeg < ustc_tweeg@tom.com >
//
/////////////////////////////////////////////////////////////////////////

#ifndef __WALKPATH_H__
#define __WALKPATH_H__

#define __IN_DIABLOII__ // Comment this line out if not used in game


#ifndef __IN_DIABLOII__
	#include <stdio.h>
#endif

#include "ArrayEx.h"

class CWalkPath  
{
public:	

	CWalkPath(WORD** Map, int MapSizeX, int MapSizeY);
	virtual ~CWalkPath();

	// return number of path points found, including the start and the end point
	// return 0 if pathing fail
	int FindWalkPath(POINT Start, POINT End, LPPOINT Path, DWORD dwMaxCount, int Range, bool Reduction);

private:

	WORD** m_Map;
	int m_MapSizeX, m_MapSizeY;
	int m_Count;

	POINT m_Start,m_End,m_Meet,m_Seperate;
	POINT m_LCurrent,m_RCurrent,m_LastLineDot;

	CArrayEx<POINT, const POINT&> ContinuousPath;
	CArrayEx<POINT, const POINT&> LeftWallPath;
	CArrayEx<POINT, const POINT&> RightWallPath;
	

	int m_LStart,m_RStart;

	BOOL m_Direct,m_FollowWallSuccess;

	BOOL IsBorder(POINT pt);
	BOOL IsBarrier(POINT pt) const;
	BOOL IsDirect(POINT pt1, POINT pt2);
	void FollowTheRope(POINT Dot);
	BOOL FollowTheWall(BOOL RightWall, POINT FollowEnd);
	void SaveFollowWallPath();
	void StraightenThePath(LPPOINT Path, DWORD dwMaxCount);
	DWORD FurtherStraighten(LPPOINT lpPath, DWORD dwMaxCount);
	static void CALLBACK FindPathProc(int X, int Y, LPARAM lpData); 
	static void CALLBACK IsDirectProc(int x, int y, LPARAM lpData);
	static int Split(const POINT& pt1, const POINT& pt2, CArrayEx<POINT, const POINT&>& aSubPath);
};



#endif // __WALKPATH_H__
