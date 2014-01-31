#pragma once

#include "PathReducer.h"
#include "../Path.h"
#include "../../Diablo_II/ActMap.h"
#
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
	bool justExpand;
	PointList distanceList;
	std::set<Point> VisitedPointList;
	Point bestPtSoFar;
public:
	TeleportPathReducer(const TeleportPathReducer&);
	TeleportPathReducer& operator=(const TeleportPathReducer&);
	TeleportPathReducer(ActMap* m, Distance d, int _range = 20) : map(m), distance(d), range(_range*10) {
	distanceList.clear();
	justExpand = false;
	bestPtSoFar = Point(0,0);
	
	int r = range / 10; 
	for(int x = 0 - r; x <= 0 + r; x ++)
		{
			for(int y = 0 - r; y <= 0 + r; y++)
			{					
				if( Euclidean(Point(x, y),Point(0,0)) < range && Euclidean(Point(x, y),Point(0,0)) > range-5 )
				{
					distanceList.push_back(Point(x,y));
				}
			}		
		}
	
	}

	void Reduce(PointList const & in, PointList& out, bool abs)
	{
		PointList::const_iterator it = in.begin(), end = in.end();
		out.push_back(*it);
		while(it != end)
		{
			Point prev = *(out.end()-1);
			while(it != end && Euclidean(*it, prev) < range) it++;
			it--;
			Point pt;
			pt.first= it->first;
			pt.second = it->second;

		//	MutatePoint(pt,abs);
			out.push_back(pt);
			it++;
		}	


	}
	// this should be in actMap with a filter setting
	// put it here because expanding telport nodes is diffrent for teleport
	void GetOpenNodes(Point const & center, PointList& out, Point const & endpoint)
	{		
		// more points added slows down the a* intelegently pick a point we can jump to with tele to speed it up
		//map->pathinging point further restricsts the nodes. for tele path we dont care if its the best possible path. cuts time in half
		// if were in tele range take the jump
		if(Euclidean(endpoint,center) < range -20)
		{
			out.push_back(endpoint);
			return;
		}
		// find best tele spot, this dosent help in arcane sant
		if(bestPtSoFar.first==0)
			bestPtSoFar = center;
	
		int val=1000000;			
		Point best(0,0);
		int x, y;
		if(Euclidean(bestPtSoFar,center) < 500)
		{
		for(int j = 0; j < distanceList.size(); j++)
		{
			x=distanceList[j].first + center.first;
			y=distanceList[j].second + center.second;
			if(!Reject(Point(x, y),true))
			{						
				if( val > Euclidean(Point(x, y), endpoint))
					{
						val = Euclidean(Point(x, y), endpoint);
						best = Point(x, y);
						out.push_back(best);
					}
			}
		}
		if (best.first != 0 && VisitedPointList.find(best) == VisitedPointList.end() && Euclidean(best, endpoint) < Euclidean(center, endpoint))
		{		
			VisitedPointList.insert(best);
			out.push_back(best);
			if(Euclidean(best,endpoint) < Euclidean(bestPtSoFar , endpoint) )
				bestPtSoFar = best;
			return;
		}
		}
		justExpand = true;
		//expand point normally if smart tele isnt found
		for(int i = 1; i >= -1; i--)
		{
			for(int j = 1; j >= -1; j--)
			{
				if( i == 0 && j == 0 || Reject(Point(center.first+i, center.second+j), true))
					continue;
				//if(map->PathingPointList.find(Point(center.first+i, center.second+j)) != map->PathingPointList.end()) 
					//continue;
				out.push_back(Point(center.first+i, center.second+j));
				VisitedPointList.insert(Point(center.first+i, center.second+j));
			}
		}		
	
		/*if(map->GetLevel()->dwLevelNo != 74)
			return;
		for(int i = range/10; i >= range/10*-1; i =i-4)
		{
			for(int j = range/10; j >= range/10*-1; j = j-4)
			{
			if( i == 0 && j == 0 || Reject(Point(center.first+i, center.second+j), true))
				continue;
			if(distance(Point(center.first+i, center.second+j), center) < range)
				if(map->PathingPointList.find(Point(center.first+i, center.second+j)) == map->PathingPointList.end()) 
				{
					out.push_back(Point(center.first+i, center.second+j));
					map->PathingPointList.insert(Point(center.first+i, center.second+j));
				}
			}
		}
		*/
	} 
	
	bool Reject(Point const & pt, bool abs)
	{
		return	   map->SpaceHasFlag(ActMap::Avoid, pt, abs) ||
			   map->SpaceHasFlag(ActMap::BlockWalk, pt, abs) ||
			   map->SpaceHasFlag(ActMap::BlockPlayer, pt, abs);
	}
	bool isWalkable(Point const & pt, bool abs)
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
		if(isWalkable(pt, abs)) {
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if(i == 0 && j == 0)
						continue;

					Point pt_pp(pt.first+i, pt.second+j);
					if(!isWalkable(pt_pp, abs)) {
						pt.first = pt_pp.first;
						pt.second = pt_pp.second;
						return;
					}

					Point pt_np(pt.first-i, pt.second+j);
					if(!isWalkable(pt_np, abs)) {
						pt.first = pt_np.first;
						pt.second = pt_np.second;
						return;
					}

					Point pt_pn(pt.first+i, pt.second-j);
					if(!isWalkable(pt_pn, abs)) {
						pt.first = pt_pn.first;
						pt.second = pt_pn.second;
						return;
					}

					Point pt_nn(pt.first-i, pt.second-j);
					if(!isWalkable(pt_nn, abs)) {
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
