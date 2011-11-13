#pragma once

#include "PathReducer.h"
#include "../Path.h"
#include "../../Diablo_II/LevelMap.h"

namespace Mapping
{
namespace Pathing
{
namespace Reducing
{

#pragma warning ( disable: 4512 )

class WalkPathReducer : public PathReducer
{
private:
	LevelMap* map;
	Distance distance;
	int range;

	inline void swap(int* x, int* y) { int t = *x; *x = *y; *y = t; }

	void Line(Point start, Point end, PointList& list, bool absolute)
	{
		int x0 = start.first, y0 = start.second,
			x1 = end.first, y1 = end.second;
		bool steep = abs(y1 - y0) > abs(x1 - x0);
		if(steep)
		{
			swap(&x0, &y0);
			swap(&x1, &y1);
		}
		if(x0 > x1)
		{
			swap(&x0, &x1);
			swap(&y0, &y1);
		}
		int dx = x1 - x0, dy = abs(y1 - y0),
			error = dx/2,
			ystep, y = y0;

		if(y0 < y1) ystep = 1;
		else ystep = -1;

		for(int x = x0; x < x1; x++)
		{
			Point p((steep ? y : x), (steep ? x : y));
			list.push_back(p);

			if(!(map->PathIsWalkable(list, absolute) && distance(start, p) < range))
			{
				list.pop_back();
				break;
			}

			error -= dy;
			if(error < 0)
			{
				y += ystep;
				error += dx;
			}
		}
	}

public:
	WalkPathReducer(const WalkPathReducer&);
	WalkPathReducer& operator=(const WalkPathReducer&);
	WalkPathReducer(LevelMap* m, Distance d, int _range = 20) : map(m), distance(d), range(_range*10) {}

	void Reduce(PointList const & in, PointList& out, bool abs)
	{
		PointList::const_iterator it = in.begin(), end = in.end();
		out.push_back(*it);
		while(it != end)
		{
			PointList path;
			Point current = *(out.end()-1);
			Point last;
			do {
				// doesn't matter how far we are, break out here
				if(it == end) break;

				last = *(it++);
				path.clear();
				Line(current, last, path, abs);
			} while(map->PathIsWalkable(path, abs) && distance(current, last) < range);
			it--;
			out.push_back(*it);
			it++;
		}
	}
	bool Reject(Point const & pt, bool abs)
	{
		return !map->IsValidPoint(pt, abs) ||
			   map->SpaceHasFlag(LevelMap::Avoid, pt, abs) ||
			   map->SpaceHasFlag(LevelMap::BlockWalk, pt, abs) ||
			   map->SpaceHasFlag(LevelMap::BlockPlayer, pt, abs);
	}
	void MutatePoint(Point & pt, bool abs)
	{
		// find the nearest walkable space
		if(Reject(pt, abs)) {
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if(i == 0 && j == 0)
						continue;
					Point ptN(pt.first+i, pt.second+j);
					if(!Reject(ptN, abs)) {
						pt.first = ptN.first;
						pt.second = ptN.second;
					}
				}
			}
		}
	}
};

#pragma warning ( default: 4512 )

}
}
}
