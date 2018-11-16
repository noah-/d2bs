#pragma once

#include "Path.h"
#include "..\Map.h"
#include "Reduction\PathReducer.h"
#include "D2Structs.h"
#include <vector>
#include <set>
#include <queue>
#include <cmath>

namespace Mapping
{
namespace Pathing
{

inline int __fastcall Manhattan(Point const & start, Point const & end)
{
	return 10*(std::abs(start.first-end.first) + std::abs(start.second-end.second));	
}

inline int __fastcall DiagonalShortcut(Point const & start, Point const & end)
{
	int xdist = std::abs(start.first-end.first);
	int ydist = std::abs(start.second-end.second);
	return ((xdist > ydist) ? 14*ydist + 10*(xdist-ydist) : 14*xdist + 10*(ydist-xdist));
}
inline int __fastcall Chebyshev(Point const & start, Point const & end)
{
	int xdist = (start.first-end.first);
	int ydist = (start.second-end.second);
	return (xdist > ydist ? xdist : ydist);
}
inline int __fastcall Euclidean(Point const & start, Point const & end)
{
	double dx = (double)(end.first - start.first);
	double dy = (double)(end.second - start.second);
	dx = pow(dx, 2);
	dy = pow(dy, 2);
	return sqrt(dx + dy)*10;
	
}

inline int __fastcall EstimateDistance(const Map* m, const Point& point, const Point& end) { return DiagonalShortcut(point, end); }

#pragma warning ( disable: 4512 )

class Node
{
public:
	Node* const parent;
	Point point;
	int g, h;

	Node() : parent(0) {}
	Node(Point s, Node* p, int _g, int _h) : point(s), parent(p), g(_g), h(_h) {}
};
struct NodeComparer {
	inline bool __fastcall operator() (const Node* left, const Node* right) const { return (left->g + left->h) > (right->g + right->h); }
};

template <class Allocator = std::allocator<Node> >
class AStarPath : public Path
{
private:
	Allocator alloc;

	Map* map;
	Reducing::PathReducer* reducer;
	Estimator estimate;
	Distance distance;

	inline void ReverseList(Node* node, PointList& list)
	{
		list.clear();
		Node* current = node;
		do { list.insert(list.begin(), current->point); } while((current = current->parent) != NULL);
	}

	void FindPath(Point const & start, Point const & end, Node** result, std::vector<Node*>& nodes, bool abs)
	{
		std::priority_queue<Node*, std::vector<Node*>, NodeComparer> open;
		std::set<Point> closed;
		PointList newNodes;
		Node* begin = alloc.allocate(1);
		UnitAny* player = D2CLIENT_GetPlayerUnit();
		DWORD startLvl = player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
		
		// if we don't get a valid node, just return
		if(!begin) return;

		alloc.construct(begin, Node(start, NULL, 0, estimate(map, start, end)));
		nodes.push_back(begin);
		open.push(begin);
		DWORD ticks = GetTickCount();
		while(!open.empty())
		{
			Node* current = open.top();
			open.pop();

			if(closed.find(current->point) != closed.end()) continue;

			if(current->point == end)
			{
				*result = current;
				return;
			}
			// this is to make d2 not completly freeze on long paths. 
			if (GetTickCount() - ticks > 500){
				map->AllowCritSpace();
				ticks=GetTickCount();
			}
			if (!GameReady() || startLvl != D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->dwLevelNo){
				Log("Pather lvl change while pathing");
				return;
			}
			if (Vars.bQuitting)
				return;

			bool result = closed.insert(current->point).second;
			assert(result == true);
			(void)(result); // shut up compiler about unused variable warning

			// getOpenNodes should be in map along with a filter
			reducer->GetOpenNodes(current->point, newNodes ,end);
			while (!newNodes.empty())
			{
				Point point = newNodes.back();
				newNodes.pop_back();
				
				if (reducer->Reject(point, abs) && point != end)
				{
					closed.insert(point);				
					continue;
				}
				Node* next = alloc.allocate(1);
				// if we don't get a valid node, just return
				if(!next) return;
				int pointPenalty = reducer->GetPenalty(point, abs);
				alloc.construct(next, Node(point, current, current->g + distance(current->point, point) + pointPenalty,
											estimate(map, point, end)));
				nodes.push_back(next);
				open.push(next);
			}
		}
	}

public:
	AStarPath(const AStarPath&);
	AStarPath& operator=(const AStarPath&);
	AStarPath(Map* _map, Reducing::PathReducer* _reducer, Estimator _estimate = EstimateDistance, Distance _distance = DiagonalShortcut) :
		map(_map), reducer(_reducer), estimate(_estimate), distance(_distance), alloc(Allocator()) {}

	inline Allocator const & GetAllocator() { return alloc; }
	inline void SetMap(Map* map) { this->map = map; }
	inline void SetPathReducer(Reducing::PathReducer* reducer) { this->reducer = reducer; }

	void GetPath(Point const & _start, Point const & _end, PointList& list, bool abs = true)
	{
		Node* result = NULL;
		Point start = _start, end = _end;
		
		// if we don't have a valid start and end, try mutating the points
		if(reducer->Reject(start, abs))
			reducer->MutatePoint(start, abs);
		if(reducer->Reject(end, abs))
			reducer->MutatePoint(end, abs);

		// if they still get rejected, forget it
		//if(!map->IsValidPoint(start, abs) || !map->IsValidPoint(end, abs)) return;

		std::vector<Node*> nodes;
		FindPath(start, end, &result, nodes, abs);
		if(result)
		{
			PointList in;
			ReverseList(result, in);
			reducer->Reduce(in, list, abs);
		}
		else
			list = PointList();

		std::vector<Node*>::iterator lbegin = nodes.begin(), lend = nodes.end();
		for(std::vector<Node*>::iterator it = lbegin; it != lend; it++)
		{
			alloc.destroy((*it));
			alloc.deallocate((*it), sizeof(*it));
		}
	}
};

#pragma warning ( default: 4512 )

}
}
