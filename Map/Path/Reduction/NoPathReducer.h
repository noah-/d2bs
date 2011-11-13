#pragma once

#include "PathReducer.h"

namespace Mapping
{
namespace Pathing
{
namespace Reducing
{

class NoPathReducer : public PathReducer
{
private:
	LevelMap* map;
public:
	NoPathReducer(const NoPathReducer&);
	NoPathReducer& operator=(const NoPathReducer&);
	NoPathReducer(LevelMap* map_) : map(map_) {}
	// the path is not reduced at all
	inline void Reduce(PointList const & in, PointList& out, bool abs) { out = in; }
	// accept only walkable nodes
	inline bool Reject(Point const & pt, bool abs) {
		return !map->IsValidPoint(pt, abs) ||
			   map->SpaceHasFlag(LevelMap::Avoid, pt, abs) ||
			   map->SpaceHasFlag(LevelMap::BlockWalk, pt, abs) ||
			   map->SpaceHasFlag(LevelMap::BlockPlayer, pt, abs);
	}
	inline void MutatePoint(Point & pt, bool abs){
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

}
}
}