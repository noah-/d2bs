//////////////////////////////////////////////////////////
// Common.h
//--------------------------------------------------------
// Common algorithms for general uses.
//
// Abin (abinn32@yahoo.com)
//////////////////////////////////////////////////////////


long CalculateDistance(const POINT& pt1, const POINT& pt2);
long CalculateDistance(long x1, long y1, long x2, long y2);
long CalculateAngle(const POINT& pt1, const POINT& pt2);
long CalculateAngle(long x1, long y1, long x2, long y2);
void NormalizeAngle(int& rAngle);
void NormalizeRect(RECT& rRect);
void NormalizeRect(LPRECT lpRect);
POINT CalculatePointOnTrack(const POINT& ptOrigin, int nRadius, int nAngle);
POINT CalculateRandomPosition(const POINT& ptOrigin, int nRadiusMin, int nRadiusMax, int nAngleMin = 0, int nAngleMax = 360);
BOOL PtInCircle(const POINT& pt, const POINT& ptOrigin, int nRadius);
