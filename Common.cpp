//////////////////////////////////////////////////////////
// Common.cpp
//--------------------------------------------------------
// Common algorithms for general uses.
//
// Abin (abinn32@yahoo.com)
//////////////////////////////////////////////////////////

#include <windows.h>
#include <math.h>
#include <stdlib.h>

#include "Common.h"

#define PI 3.1415926535

long CalculateDistance(const POINT& pt1, const POINT& pt2)
{
	return CalculateDistance(pt1.x, pt1.y, pt2.x, pt2.y);
}

long CalculateAngle(const POINT& pt1, const POINT& pt2)
{
	return CalculateAngle(pt1.x, pt1.y, pt2.x, pt2.y);
}

long CalculateDistance(long x1, long y1, long x2, long y2)
{
	return (long)::sqrt((double)((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}

BOOL PtInCircle(const POINT& pt, const POINT& ptOrigin, int nRadius)
{
	return CalculateDistance(pt, ptOrigin) < ::abs(nRadius);
}

void NormalizeAngle(int& rAngle)
{
	if (::abs(rAngle) >= 360)
		rAngle %= 360;

	if (rAngle < 0)
		rAngle += 360;
}

void NormalizeRect(RECT& rRect)
{
	NormalizeRect(&rRect);
}

void NormalizeRect(LPRECT lpRect)
{
	if (lpRect == NULL)
		return;

	long temp;
	if (lpRect->left > lpRect->right)
	{
		temp = lpRect->left;
		lpRect->left = lpRect->right;
		lpRect->right = temp;
	}

	if (lpRect->top > lpRect->bottom)
	{
		temp = lpRect->top;
		lpRect->top = lpRect->bottom;
		lpRect->bottom = temp;
	}
}

long CalculateAngle(long x1, long y1, long x2, long y2)
{
	// mathematic stuff, now thanks God I haven't forgot all of them...
	if (x1 == x2 && y1 == y2)
		return 0;

	double fAngle = 0.0;
	
	if (x1 == x2)
	{
		// vertical special case
		fAngle = y2 > y1 ? 270.0 : 90.0;
	}
	else if (y1 == y2)
	{
		// horizontal special case
		fAngle = x2 > x1 ? 0.0 : 180.0;
	}
	else
	{		
		// common case
		fAngle = ::atan(((double)y2 - (double)y1) / ((double)x2 - (double)x1)) * 180.0 / PI;

		// determine the phases (1-4)
		// Phases allocation figure:
		/*
		            y
		            |
		     P2     |    P1
		            |
		 -----------+----------->x
		            |
		     P3     |    P4
		            |
		 */
		int nPhase = 0;
		if (y2 < y1)
			nPhase = x2 > x1 ? 1 : 2;
		else
			nPhase =x2 > x1 ? 4 : 3;

		// adjust the angle according to phases
		switch (nPhase)
		{
		case 1:
			fAngle = -fAngle;
			break;

		case 2:
			fAngle = 180.0 - fAngle;
			break;

		case 3:
			fAngle = 180.0 - fAngle;
			break;

		case 4:
			fAngle = 360.0 - fAngle;
			break;

		default:
			fAngle = 0.0;
			break;
		}
	}	

	return (long)fAngle;
}

POINT CalculatePointOnTrack(const POINT& ptOrigin, int nRadius, int nAngle)
{
	if (nRadius == 0)
		return ptOrigin;

	NormalizeAngle(nAngle);

	POINT pt;
	pt.x = long(double(ptOrigin.x) + ::cos((double)nAngle * PI / 180.0) * (double)nRadius);
	pt.y = long(double(ptOrigin.y) - ::sin((double)nAngle * PI / 180.0) * (double)nRadius);
	return pt;
}

POINT CalculateRandomPosition(const POINT& ptOrigin, int nRadiusMin, int nRadiusMax, int nAngleMin/*=0*/, int nAngleMax/*=360*/)
{
	// Data validation
	nRadiusMin = max(0, nRadiusMin);
	nRadiusMax = max(0, nRadiusMax);

	NormalizeAngle(nAngleMin);
	NormalizeAngle(nAngleMax);

	const int R1 = min(nRadiusMin, nRadiusMax);
	const int R2 = max(nRadiusMin, nRadiusMax);
	const int A1 = min(nAngleMin, nAngleMax);
	const int A2 = max(nAngleMin, nAngleMax);

	const int R = (R1 == R2) ? R1 : (R1 + (::rand() % (R2 - R1))); // Final radius
	const int A = (A1 == A2) ? A1 : (A1 + (::rand() % (A2 - A1))); // Final angle

	return CalculatePointOnTrack(ptOrigin, R, A);
}