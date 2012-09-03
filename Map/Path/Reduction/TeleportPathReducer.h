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

class TeleportPathReducer : public PathReducer
{
private:
	ActMap* map;
	Distance distance;
	int range;

public:
	TeleportPathReducer(const TeleportPathReducer&);
	TeleportPathReducer& operator=(const TeleportPathReducer&);
	TeleportPathReducer(ActMap* m, Distance d, int _range = 20) : map(m), distance(d), range(_range*10) {}

	void Reduce(PointList const & in, PointList& out, bool abs)
	{
		PointList::const_iterator it = in.begin(), end = in.end();
		out.push_back(*it);
		while(it != end)
		{
			Point prev = *(out.end()-1);
			while(it != end && distance(*it, prev) < range) it++;
			it--;
			out.push_back(*it);
			it++;
		}
	}
	// this should be in actMap with a filter setting
	// put it here because expanding telport nodes is diffrent for teleport
	void GetOpenNodes(Point const & center, PointList& out)
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
		if(map->GetLevel()->dwLevelNo != 74)
			return;
		for(int i = range/10; i >= range/10*-1; i =i-4)
		{
			for(int j = range/10; j >= range/10*-1; j = j-4)
			{
			if( i == 0 && j == 0)
				continue;
			if(distance(Point(center.first+i, center.second+j), center) < range)
				out.push_back(Point(center.first+i, center.second+j));
			}
		}		
	}
	bool Reject(Point const & pt, bool abs)
	{
		return	   map->SpaceHasFlag(ActMap::Avoid, pt, abs) ||
			   map->SpaceHasFlag(ActMap::BlockWalk, pt, abs) ||
			   map->SpaceHasFlag(ActMap::BlockPlayer, pt, abs);
	}
	int GetPenalty(Point const & pt, bool abs)
	{
		return 0;
	}
	void MutatePoint(Point & pt, bool abs)
	{
		// find the nearest walkable space
		if(Reject(pt, abs)) {
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if(i == 0 && j == 0)
						continue;

					Point pt_pp(pt.first+i, pt.second+j);
					if(!Reject(pt_pp, abs)) {
						pt.first = pt_pp.first;
						pt.second = pt_pp.second;
						return;
					}

					Point pt_np(pt.first-i, pt.second+j);
					if(!Reject(pt_np, abs)) {
						pt.first = pt_np.first;
						pt.second = pt_np.second;
						return;
					}

					Point pt_pn(pt.first+i, pt.second-j);
					if(!Reject(pt_pn, abs)) {
						pt.first = pt_pn.first;
						pt.second = pt_pn.second;
						return;
					}

					Point pt_nn(pt.first-i, pt.second-j);
					if(!Reject(pt_nn, abs)) {
						pt.first = pt_nn.first;
						pt.second = pt_nn.second;
						return;
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
