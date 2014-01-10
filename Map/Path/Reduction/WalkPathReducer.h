#pragma once

#include "PathReducer.h"
#include "../Path.h"
#include "../../Diablo_II/ActMap.h"

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
	ActMap* map;
	Distance distance;
	int range;

	inline void swap(int* x, int* y) { int t = *x; *x = *y; *y = t; }

	/*
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
	*/

public:
	WalkPathReducer(const WalkPathReducer&);
	WalkPathReducer& operator=(const WalkPathReducer&);
	WalkPathReducer(ActMap* m, Distance d, int _range = 20) : map(m), distance(d), range(_range*10) {}

	/*
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
	*/

	void Reduce(PointList const & in, PointList& out, bool abs)
	{
		PointList::const_iterator it = in.begin(), end = in.end();
		out.push_back(*it);
		Point lineStartStep;
		Point lineStartPoint;
		Point currentStep;
		Point previousPoint;
		Point secondStep;
		int differentStepCount = 0;
		int lineLength = 0;
		bool first = true;
		while(it != end)
		{
			if (first)
			{
				lineStartPoint = *it;
				it++;
				lineLength++;
				first = false;
				if (it != end)
				{
					lineStartStep = Point(lineStartPoint.first - (*it).first, lineStartPoint.second - (*it).second);
					secondStep = lineStartStep;
				}
				continue;
			}
			previousPoint = *it;
			it++;
			lineLength++;
			if (it == end)
			{
				out.push_back(previousPoint);
				break;
			}
			currentStep = Point(previousPoint.first - (*it).first, previousPoint.second - (*it).second);
			if (currentStep != lineStartStep)
			{
				if (lineStartStep == secondStep)
				{
					secondStep = currentStep;
				}
				differentStepCount++;
			}
			else if (differentStepCount == 1)
			{
				differentStepCount = 0;
			}
			if (lineLength > 5 && ((currentStep != lineStartStep && currentStep != secondStep) || differentStepCount > 1 || lineLength > 20))
			{
				out.push_back(previousPoint);
				lineStartStep = Point(currentStep);
				secondStep = lineStartStep;
				lineLength = 1;
				differentStepCount = 0;
			}
		}
		/*
		char mapFileName[200];
		sprintf_s(mapFileName, "b:\\maps-pathing\\%03d %s in.txt", map->GetLevel()->dwLevelNo, GetLevelName(map->GetLevel()));
		map->Dump(mapFileName, in);
		sprintf_s(mapFileName, "b:\\maps-pathing\\%03d %s out.txt", map->GetLevel()->dwLevelNo, GetLevelName(map->GetLevel()));
		map->Dump(mapFileName, out);
		*/
	}

	bool Reject(Point const & pt, bool abs)
	{
		return map->SpaceHasFlag(ActMap::Avoid, pt, abs)     ||
			   map->SpaceHasFlag(ActMap::BlockWalk, pt, abs) ||
			   map->SpaceHasFlag(ActMap::BlockPlayer, pt, abs);
	}

	void GetOpenNodes(Point const & center, PointList& out, Point const & endpoint)
	{
		for(int i = 1; i >= -1; i--)
		{
			for(int j = 1; j >= -1; j--)
			{
				if( i == 0 && j == 0)
					continue;
				out.push_back(Point(center.first+i, center.second+j));
			}
		}		
	}

	int GetPenalty(Point const & pt, bool abs)
	{
		return map->SpaceHasFlag(ActMap::Object, pt, abs) ? 60 : (map->SpaceHasFlag(ActMap::ClosedDoor, pt, abs) ? 80 : 0);
	}

	void MutatePoint(Point & pt, bool abs)
	{
		bool mutated = false;
		// find the nearest walkable space
		if(Reject(pt, abs))
		{
			for(int i = -2; !mutated && i <= 2; i++)
			{
				for(int j = -2; !mutated && j <= 2; j++)
				{
					if(i == 0 && j == 0)
						continue;
					Point ptN(pt.first+i, pt.second+j);
					if(!Reject(ptN, abs))
					{
						pt.first = ptN.first;
						pt.second = ptN.second;
						mutated = true;
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
