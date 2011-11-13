#pragma once

#include "Map.h"

namespace Mapping
{

class BlankMap : public Map
{
private:
	int width, height;
	int posX, posY;

public:
	BlankMap(int wX, int wY, int wH, int wL) { posX = wX; posY = wY; width = wH; height = wL; }
	~BlankMap(void) { }

	void Dump(const char* file, const PointList& points) const { };

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

	int GetMapData(const Point& point, bool abs) const { assert(IsValidPoint(point, abs)); return 0; }
	bool IsValidPoint(const Point& point, bool abs) const { return !!(point.first >= 0 && point.second >= 0 && point.first <= height && point.second <= width); }

	bool SpaceHasFlag(int flag, const Point& point, bool abs = true) const { return ((GetMapData(point, abs) & flag) == flag); }
	bool PathHasFlag(int flag, const PointList& points, bool abs = true) const {
		int count = points.size();
		for(int i = 0; i < count; i++)
			if(!SpaceHasFlag(flag, points[i], abs))
				return false;
		return true;
	}

};

}
