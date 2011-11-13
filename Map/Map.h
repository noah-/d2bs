#pragma once

#include <vector>
#include <utility>
#include <assert.h>

namespace Mapping
{

typedef std::pair<int, int> Point;
typedef std::vector<Point> PointList;

class Map
{
public:
	virtual void Dump(const char* file, const PointList& points) const = 0;

	virtual int GetWidth(void) const = 0;
	virtual int GetHeight(void) const = 0;

	virtual int GetMapData(const Point& point, bool abs) const = 0;
	virtual bool IsValidPoint(const Point& point, bool abs) const = 0;

	virtual bool SpaceHasFlag(int flag, const Point& point, bool abs) const = 0;
	virtual bool PathHasFlag(int flag, const PointList& points, bool abs) const = 0;
};

template <typename T>
class Matrix
{
private:
	typedef std::vector<T> TList;

	std::vector<TList> points;
	int width, height;

public:
	Matrix() : width(0), height(0) {}
	Matrix(int height, int width, T fill) {
		this->width = width; this->height = height;
		points = std::vector<TList>(height);
		for(int i = 0; i < height; i++)
			points[i] = TList(width, fill);
	}
	T __fastcall GetPoint(int x, int y) const {
		assert(x < height && y < width);
		return points[x][y];
	}
	void SetPoint(int x, int y, T value) {
		assert(x < height && y < width);
		points[x][y] = value;
	}
};

}
