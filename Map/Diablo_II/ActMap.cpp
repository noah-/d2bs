#include <algorithm>
#include <assert.h>
#include <cmath>
#include "ActMap.h"
#include "D2Structs.h"
#include <map>
#include <set>
#include "Core.h"

namespace Mapping
{

ActMapList ActMap::cache = ActMapList();
DWORD ActMap::previousLevelNo = 0;

ActMap* ActMap::GetMap(Level* level)
{
	ActMap* map = NULL;
	if (previousLevelNo != level->dwLevelNo)
	{
		if (previousLevelNo != 0)
		{
			cache[0]->CleanUp();
		}
		previousLevelNo = level->dwLevelNo;
		map = new ActMap(level);
		cache[0] = map;
	}
	else
	{
		map = cache[0];
	}
	return map;
}

void ActMap::ClearCache(void)
{
	ActMapList::iterator end = cache.end();
	for(ActMapList::iterator it = cache.begin(); it != end; it++)
		delete it->second;
	cache.clear();
}

ActMap::ActMap(const Level* level)
{
	lock = new CRITICAL_SECTION;
	InitializeCriticalSection(lock);

	assert(act != NULL);
	assert(level != NULL);
	
	this->level = level;
	this->act = level->pMisc->pAct;

	//RoomsAdded= roomsAdded;
	// get the map size
	height = level->dwSizeX * 5;
	width  = level->dwSizeY * 5;
	posX   = level->dwPosX * 5;
	posY   = level->dwPosY * 5;

	Room2* room = level->pRoom2First;
	cachedLevel = room->pLevel;

	posX   = (level->dwPosX == -1 ? 0 : level->dwPosX * 5);
	posY   = (level->dwPosY == -1 ? 0 : level->dwPosY * 5);
	
}

ActMap::~ActMap(void)
{
	DeleteCriticalSection(lock);
	delete lock;
	lock = NULL;
}

Point ActMap::AbsToRelative(const Point& point) const
{
	return Point(point.first - posX, point.second - posY);
}

Point ActMap::RelativeToAbs(const Point& point) const
{
	return Point(point.first + posX, point.second + posY);
}

int ActMap::GetMapData(const Point& point, bool abs) const
{
	if(!abs) 
		return GetMapData(RelativeToAbs(point), true);
	
	//check in rooms we already read
	for(RoomList::iterator cRoom = roomCache.begin(); cRoom != roomCache.end(); cRoom++)
		if(isPointInRoom(*cRoom, point))
			return getCollFromRoom(*cRoom, point);
	
	const Level* currLevel = NULL;
	if (isPointInLevel(this->level, point))
	{
		currLevel = level;
	}
	if (!currLevel)
	{
		for(LevelList::iterator cLvl = levelCache.begin(); cLvl != levelCache.end(); cLvl++)
		{
			if(isPointInLevel(*cLvl, point))
				currLevel = *cLvl;
		}
	}
	if(!currLevel)
	{
		for(Level* lvl = this->act->pMisc->pLevelFirst; lvl; lvl = lvl->pNextLevel)
		{
			if(isPointInLevel(lvl, point)){
				levelCache.push_front(lvl);
				currLevel = lvl;
			}
		}
	}

	int value = ActMap::Avoid;  //Avoid if not found
	if (!currLevel)
		return value;

	EnterCriticalSection(lock); // not sure if this is needed or its correct lock now
	
	
	for(Room2* room = currLevel->pRoom2First; room; room = room->pRoom2Next)
	{
		if (isPointInRoom(room, point))
		{	
			roomCache.push_front(room);
			//this->cachedRoom = room->pRoom1->pRoom2;
			value = getCollFromRoom(room, point);
			break;
		}			
	}

	LeaveCriticalSection(lock);

	return value;
}

bool ActMap::isPointInRoom(const Room2* room, const Point& pt)const
{
	DWORD nX = pt.first;
	DWORD nY = pt.second;
			
	return (nX >= room->dwPosX *5  &&  nY >= room->dwPosY *5 &&
			nX < (room->dwPosX *5 + room->dwSizeX *5) &&  nY < (room->dwPosY *5 + room->dwSizeY *5));			

}

bool ActMap::isPointInLevel(const Level* level, const Point& pt)const
{
DWORD x = pt.first;
DWORD y = pt.second;
	return (x >= level->dwPosX *5  &&  y >= level->dwPosY *5 &&
			x < (level->dwPosX *5 + level->dwSizeX *5) &&  y < (level->dwPosY *5 + level->dwSizeY *5));			

}

WORD ActMap::getAvoidLayerPoint(Room2* room, const Point& pt) const
{
	RoomPointSet::iterator it;
	it = avoidRoomPointSet.find(room); 
	if (it == avoidRoomPointSet.end())
	{
		PointSet pointSet;
		for(PresetUnit* preset = room->pPreset; preset; preset = preset->pPresetNext)
		{
			Point loc((room->dwPosX*5)+preset->dwPosX, (room->dwPosY*5)+preset->dwPosY);
			if (preset->dwTxtFileNo == 435) // barricade tower
			{
				pointSet.insert(loc);
			}
		}
		avoidRoomPointSet[room] = pointSet;
		it = avoidRoomPointSet.find(room);
	}
	if (it->second.find(pt) == it->second.end())
	{
		return 0;
	}
	return ActMap::Avoid;
}

WORD ActMap::getCollFromRoom( Room2* room, const Point& pt) const
{
	if(!room->pRoom1)
	{
		AddRoomData(room);
		RoomsAdded.push_back(room);
	}
	CollMap* map = room->pRoom1->Coll;
	WORD val = *(map->pMapStart+(pt.second - map->dwPosGameY) * (map->dwSizeGameX) + (pt.first - map->dwPosGameX));

	// todo: avoid layer could be used in every area, we can add unwalkable objects like torches, chests etc
	// but for now, just try it with barricade wall in Frigid Highlands (111), Arreat Plateau (112) and Frozen Tundra (117)
	if (room->pLevel->dwLevelNo == 111 || room->pLevel->dwLevelNo == 112 || room->pLevel->dwLevelNo == 117)
	{
		val |= getAvoidLayerPoint(room, pt);
	}
	return val;
}

void ActMap::CleanUp(void){
	
	for(RoomList::iterator it = RoomsAdded.begin(); it != RoomsAdded.end(); it++)
		RemoveRoomData(*it);
	RoomsAdded.clear();
	levelCache.clear();
	roomCache.clear();
	avoidRoomPointSet.clear();

}

void ActMap::GetExits(ExitArray& exits) const
{
	static const Point empty(0,0);
	EnterCriticalSection(lock);

	RoomList added;

	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		if(!room->pRoom1)
		{
			AddRoomData(room);
			added.push_back(room);
		}

		if(room->pLevel->dwLevelNo != level->dwLevelNo)
			continue;

		FindRoomTileExits(room, exits);

	}
	FindRoomLinkageExits(exits);

	RoomList::iterator start = added.begin(), last = added.end();
	for(RoomList::iterator it = start; it != last; it++)
		RemoveRoomData(*it);

	LeaveCriticalSection(lock);
}

bool ActMap::ExitExists(Point loc, ExitArray& exits) const
{
	ExitArray::iterator start = exits.begin(), end = exits.end();
	for(ExitArray::iterator it = start; it != end; it++)
	{
		if(it->Position == loc)
		{
			return true;
		}
	}
	return false;
}

bool ActMap::ExitExists(DWORD dwLevelNo, ExitArray& exits) const
{
	ExitArray::iterator begin = exits.begin(), last = exits.end();
	for(ExitArray::iterator it = begin; it != last; it++)
	{
		if(it->Target == dwLevelNo)
		{
			return true;
		}
	}
	return false;
}

void ActMap::FindRoomTileExits(Room2* room, ExitArray& exits) const
{
	for(PresetUnit* preset = room->pPreset; preset; preset = preset->pPresetNext)
	{
		if(preset->dwType == 5) // UNIT_TILE
		{
			DWORD levelId = GetLevelNo(room, preset->dwTxtFileNo);
			if(levelId != 0)
			{
				Point loc((room->dwPosX*5)+preset->dwPosX, (room->dwPosY*5)+preset->dwPosY);

				if(!ExitExists(loc, exits))
				{
					exits.push_back(Exit(loc, levelId, Tile, preset->dwTxtFileNo));
				}
			}
		}
	}
}

void ActMap::FindRoomLinkageExits(ExitArray& exits) const
{
	using namespace std;

	multimap<int, std::pair<Point, std::pair<Point, int>>> exitMap; // <level, <rooms[i], <middlepoint, size> > >
	
	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		Room2** rooms = room->pRoom2Near;
		for(DWORD i = 0; i < room->dwRoomsNear; i++)
		{
			if(rooms[i]->pLevel->dwLevelNo == level->dwLevelNo)
				continue;

			// does this link already exist?
			bool exists = false;
			ExitArray::iterator begin = exits.begin(), last = exits.end();
			for(ExitArray::iterator it = begin; it != last; it++)
			{
				if(it->Target == rooms[i]->pLevel->dwLevelNo)
				{
					exists = true;
					break;
				}
			}
			if(exists)
				continue;

			// A -> origin of local room, B -> opposite corner from origin in local room ... X, Y -> like local but for adjecent room
			Point A, B, X, Y, startPoint, edgeDirection, orthogonalDirection;
			int overlappingX, overlappingY, edgeSize;
			// this could be enum
			bool startLeft = false, startRight = false, startTop = false, startBottom = false;

			A = Point(room->dwPosX * 5, room->dwPosY * 5);
			B = Point(room->dwPosX * 5 + room->dwSizeX * 5, room->dwPosY * 5 + room->dwSizeY * 5);

			X = Point(rooms[i]->dwPosX * 5, rooms[i]->dwPosY * 5);
			Y = Point(rooms[i]->dwPosX * 5 + rooms[i]->dwSizeX * 5, rooms[i]->dwPosY * 5 + rooms[i]->dwSizeY * 5);

			overlappingX = min(B.first,  Y.first)  - max(A.first,  X.first);
			overlappingY = min(B.second, Y.second) - max(A.second, X.second);
	
			if (overlappingX > 0) // top or bottom edge
			{
				if (A.second < X.second) // bottom edge
				{
					startPoint = Point(max(A.first, X.first), B.second-1);
					startBottom = true;
				}
				else
				{
					startPoint = Point(max(A.first, X.first), A.second);
					startTop = true;
				}
			}
			else if (overlappingY > 0) // left or right edge
			{
				if (A.first < X.first) // right edge
				{
					startPoint = Point(B.first-1, max(A.second, X.second));
					startRight = true;
				}
				else
				{
					startPoint = Point(A.first, max(A.second, X.second));
					startLeft = true;
				}
			}
	
			if (overlappingX < 0 || overlappingY < 0)
			{
				// Print("ÿc1d2bsÿc3ActMap::GetExitsÿc0 adjecent room is out of reach - it's not even touching local room in corner!");
				continue;
			}
			if (overlappingX > 0 && overlappingY > 0)
			{
				// Print("ÿc1d2bsÿc3ActMap::GetExitsÿc0 WTF local and adjecent rooms are overlapping (they share some points)!!!");
				continue;             
			}
			if (overlappingX < 3 && overlappingY < 3)
			{
				// edge is not large enough to even bother to check for exit (exit should be at least 3 points wide)
				continue;
			}
	
			if (startLeft || startRight) // -> going down
			{
				edgeSize = overlappingY; // number of vertical touched points
				edgeDirection = Point(0, 1); // down
				orthogonalDirection = Point((startLeft ? -1 : 1), 0); // checking adjecent spaces -> left / right
			}
			else // startTop || startBottom -> going right
			{
				edgeSize = overlappingX; // number of horizontal touched points
				edgeDirection = Point(1, 0); // right
				orthogonalDirection = Point(0, (startTop ? -1 : 1)); // checking adjecent spaces -> up / down
			}

			Point currentPoint, lastWalkablePoint;
			bool isPointWalkable = false;
			int spaces = 0, j;
			for (j = 0; j < edgeSize; j++)
			{
				// would be nice to convert this line to => currentPoint = startPoint + edgeDirection * j;
				currentPoint = Point(startPoint.first + j * edgeDirection.first, startPoint.second + j * edgeDirection.second);
				isPointWalkable = EdgeIsWalkable(currentPoint, orthogonalDirection, true);
				if (isPointWalkable)
				{
					lastWalkablePoint = currentPoint;
					spaces++;
				}
				if (false == isPointWalkable || j + 1 == edgeSize)
				{
					if (spaces > 0)
					{
						Point centerEdgePoint = GetEdgeCenterPoint(currentPoint, edgeDirection);
						currentPoint = Point(lastWalkablePoint.first - edgeDirection.first * spaces / 2, lastWalkablePoint.second - edgeDirection.second * spaces / 2);
						exitMap.insert(make_pair(rooms[i]->pLevel->dwLevelNo, make_pair(centerEdgePoint, make_pair(currentPoint, spaces))));
					}
					spaces = 0;
				}
			}
		}
	}

	std::pair<Point, std::pair<Point, int>> currentExit;
	std::multimap<int, std::pair<Point, std::pair<Point, int>>>::iterator it = exitMap.begin();
	int level = 0;
	double minDistance = -1, tmpDistance;
	do
	{
		if (level == 0 && it == exitMap.end())
		{
			break;
		}
		if (level == 0)
		{
			level = it->first;
			currentExit = it->second;
		}
		if (it == exitMap.end() || level != it->first)
		{
			exits.push_back(Exit(currentExit.second.first, level, Linkage, 0));
			if (it == exitMap.end())
			{
				break;
			}
			level = it->first;
			currentExit = it->second;
			minDistance = -1;
		}

		tmpDistance = GetDistance(
				it->second.first.first,
				it->second.first.second,
				it->second.second.first.first,
				it->second.second.first.second
		);
		if (minDistance == -1 || minDistance > tmpDistance)
		{
			currentExit = it->second;
			minDistance = tmpDistance;
		}
	} while(it++ != exitMap.end());
}

Point ActMap::GetEdgeCenterPoint(const Point &currentPoint, const Point &edgeDirection) const
{
	Point startPoint = Point(currentPoint.first, currentPoint.second);
	Point left = startPoint, right = startPoint;
	for (int i = -1; IsValidPoint(startPoint, true); i--)
	{
		if (SpaceIsWalkableForExit(startPoint, true))
		{
			left = startPoint;
		}
		startPoint = Point(currentPoint.first + edgeDirection.first * i, currentPoint.second + edgeDirection.second * i);
	}
	startPoint = Point(currentPoint.first, currentPoint.second);
	for (int i = 1; IsValidPoint(startPoint, true); i++)
	{
		if (SpaceIsWalkableForExit(startPoint, true))
		{
			right = startPoint;
		}
		startPoint = Point(currentPoint.first + edgeDirection.first * i, currentPoint.second + edgeDirection.second * i);
	}
	return Point((left.first + right.first) / 2, (left.second + right.second) / 2);
}

/**
 *    This method will check six points - orthogonal to area-crossing-edge
 *    * 3 points from local room
 *    * 3 points from adjecent room (room behind area-crossing-edge)
 *
 *                            __                              
 *    distanceLocal       -2    |                             i    3
 *                        -1    +--- pRoom1Local              n    2
 *    edgePoint           (0) __|                            -->   1
 *                                                            o 
 *    -------------[ area join edge ]----------------         r
 *                            __                              d 
 *                         1    |                             e    4
 *                         2    +--- pRoom1Adjecent           r    5
 *    distanceAdjecent     3  __|                                  6
 */
bool ActMap::EdgeIsWalkable(const Point& edgePoint, const Point& offsetPoint, bool abs) const
{
	int k;
	const int distanceLocal    = -1;
	const int distanceAdjecent =  2;
	for (k = distanceLocal; k <= distanceAdjecent; k++)
	{
		if (k <= 0)  // simulation of character-size (3 point in width/height) staying in local room
		{
			const Point currentPoint = Point(edgePoint.first + offsetPoint.first * (distanceLocal-k), edgePoint.second + offsetPoint.second * (distanceLocal-k));
			if (false == SpaceIsWalkableForExit(currentPoint, abs))
			{
				break;
			}
		}

		// -------------[ area join edge ]---------------- 

		else // simulation of character-size (3 point in width/height) staying in adjecent room
		{
			const Point currentPoint = Point(edgePoint.first + offsetPoint.first * k, edgePoint.second + offsetPoint.second * k);
			if (false == SpaceIsWalkableForExit(currentPoint, abs))
			{
				break;
			}
		}
	}
	return k > distanceAdjecent;
}

bool ActMap::IsValidPoint(const Point& point, bool abs) const
{
	if(abs) return IsValidPoint(AbsToRelative(point), false);
	return !!(point.first >= 0 && point.second >= 0 && point.first < height && point.second < width);
}

bool ActMap::OneSpaceHasFlag(int flag, const Point& point, bool abs) const
{
	if(!abs) 
		return OneSpaceHasFlag(flag, RelativeToAbs(point), true);

	int val = GetMapData(point, abs);
	return ((val & flag) == flag);
}

bool ActMap::SpaceHasFlag(int flag, const Point& point, bool abs) const
{
	if(!abs) 
		return SpaceHasFlag(flag, RelativeToAbs(point), true);

	int val = GetMapData(point, abs)
		    | GetMapData(Point(point.first - 1, point.second), abs)
			| GetMapData(Point(point.first + 1, point.second), abs)
			| GetMapData(Point(point.first, point.second - 1), abs)
			| GetMapData(Point(point.first, point.second + 1), abs);
	return ((val & flag) == flag);
}

bool ActMap::PathHasFlag(int flag, const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceHasFlag(flag, points[i], abs))
			return false;
	return true;
}

bool ActMap::SpaceIsWalkable(const Point& point, bool abs) const
{
	// ignore closed doors here, because we want to path through them
			//(!IsValidPoint(point, abs)                        ||
	return !(SpaceHasFlag(ActMap::Avoid, point, abs)        ||
		     SpaceHasFlag(ActMap::BlockWalk, point, abs)    ||
			 SpaceHasFlag(ActMap::BlockPlayer, point, abs)  ||
			 SpaceHasFlag(ActMap::NPCCollision, point, abs) ||
		     SpaceHasFlag(ActMap::Object, point, abs)       ||
			 SpaceHasFlag(ActMap::ThickenedWall, point, abs)
			 );
}

bool ActMap::SpaceIsWalkableForExit(const Point& point, bool abs) const
{
	// ignore almost everything
	return !(
		     SpaceHasFlag(ActMap::Avoid, point, abs)      ||
			 SpaceHasFlag(ActMap::BlockWalk, point,  abs) ||
		     SpaceHasFlag(ActMap::BlockPlayer, point, abs)
			);
}

bool ActMap::SpaceHasLineOfSight(const Point& point, bool abs) const
{
	return !SpaceHasFlag(ActMap::Avoid, point, abs) && !SpaceHasFlag(ActMap::BlockLineOfSight, point, abs);
}

bool ActMap::SpaceIsInvalid(const Point& point, bool abs) const { return SpaceHasFlag(ActMap::Avoid, point, abs); }

bool ActMap::PathIsWalkable(const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceIsWalkable(points[i], abs))
			return false;
	return true;
}

bool ActMap::PathHasLineOfSight(const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceHasLineOfSight(points[i], abs))
			return false;
	return true;
}

void ActMap::Dump(const char* file, const PointList& points) const
{
	FILE* f = NULL;
	fopen_s(&f, file, "wt+");
	if(!f) return;
	fprintf(f, "Map (%d x %d) at (%d, %d) for area %s (id %d):\n", width, height, posX, posY, GetLevelName(level), level->dwLevelNo);

	fprintf(f, "Exits:\n");

	ExitArray exits;
	GetExits(exits);
	int i = 1;
	for(ExitArray::iterator it = exits.begin(); it != exits.end(); it++)
	{
		Point rel = AbsToRelative(it->Position);
		fprintf(f, "Exit %d: at (%d, %d, relative: %d, %d) of type %s linkage to area %s (id: %d, tile id: %d)\n", i++,
						it->Position.first, it->Position.second, rel.first, rel.second,
						it->Type == Linkage ? "Area" : "Tile", GetLevelIdName(it->Target), it->Target, it->TileId);
	}

	fprintf(f, "\n");

	PointList::const_iterator begin = points.begin(), end = points.end();
	Point first, last;
	if(points.size() == 0)
	{
		first = Point(-1, -1);
		last = Point(-1, -1);
	}
	else
	{
		first = *begin;
		last = *(end-1);
	}

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			Point pt(i, j);
			char c = '.';
			if(SpaceHasFlag(ActMap::Avoid, pt, false))
				c = 'A';
			else if(pt == first)
				c = 'S';
			else if(pt == last)
				c = 'E';
			else if(std::find(begin, end, pt) != end)
				c = 'P';
			else if(SpaceIsInvalid(pt, false))
				c = '?';
			else if(GetMapData(pt, false) == ActMap::ThickenedWall)
				c = 'T';
			else if(!SpaceIsWalkable(pt, false))
				c = 'X';
			fprintf(f, "%c", c);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}


void ActMap::DumpLevel(const char* file) const
{
	EnterCriticalSection(lock);
	FILE* f = NULL;
	fopen_s(&f, file, "wt+");
	if(!f) return;
	fprintf(f, "Map (%d x %d) at (%d, %d) for area %s (id %d):\n", width, height, posX, posY, GetLevelName(level), level->dwLevelNo);
	
	fprintf(f, "--------------------------\n");
	fprintf(f, "Presets:\n");
	RoomList added;
	int i = 1;
	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		for(PresetUnit* preset = room->pPreset; preset; preset = preset->pPresetNext)
		{
			Point loc((room->dwPosX*5)+preset->dwPosX, (room->dwPosY*5)+preset->dwPosY);
			Point rel = AbsToRelative(loc);
			fprintf(f, "Preset %d: at (%d, %d, relative: %d, %d) of type %d, txtFile %d, _1 %d, _3 %d, room (%d, %d)\n",
			i++, loc.first, loc.second, rel.first, rel.second, preset->dwType, preset->dwTxtFileNo, preset->_1, preset->_3, room->dwPosX, room->dwPosY);
		}
	}

	fprintf(f, "--------------------------\n");
	fprintf(f, "Units:\n");
	i = 1;
	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		if(!room->pRoom1)
		{
			AddRoomData(room);
			added.push_back(room);
		}

		for(UnitAny* pUnit = room->pRoom1->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
		{
			Point loc(D2CLIENT_GetUnitX(pUnit),D2CLIENT_GetUnitY(pUnit));
			Point rel = AbsToRelative(loc);
			char tmp[128] = "";
			GetUnitName(pUnit, tmp, 128);
			fprintf(f, "Unit %d: at (%d, %d, relative: %d, %d) of type %d, txtFile %d, %s, %d, %d\n", 
			i++, loc.first, loc.second, rel.first, rel.second, pUnit->dwType, pUnit->dwTxtFileNo, tmp, pUnit->wX, pUnit->wY);
		}
	}
	RoomList::iterator start = added.begin(), last = added.end();
	for(RoomList::iterator it = start; it != last; it++)
		RemoveRoomData(*it);
	
	fprintf(f, "--------------------------\n");
	fprintf(f, "Exits:\n");
	ExitArray exits;
	GetExits(exits);
    std::set<Point> exitPoints;
	i = 1;
	for(ExitArray::iterator it = exits.begin(); it != exits.end(); it++)
	{
		Point rel = AbsToRelative(it->Position);
		fprintf(f, "Exit %d: at (%d, %d, relative: %d, %d) of type %s linkage to area %s (id: %d, tile id: %d)\n", i++,
		it->Position.first, it->Position.second, rel.first, rel.second,
		it->Type == Linkage ? "Area" : "Tile", GetLevelIdName(it->Target), it->Target, it->TileId);
		exitPoints.insert(rel);
	}
	fprintf(f, "\n");
	
	for(int j = 0; j < width; j++)
	{
		for(int i = 0; i < height; i++)
		{
				// skipping group of Avoid points at the end of row
				bool onlyAvoid = true;
				int k;
				for (k = i; k < height; k++)
				{
					Point pt(k, j);
					if (!OneSpaceHasFlag(ActMap::Avoid, pt, false))
					{
						onlyAvoid = false;
						break;
					}
				}
				if (onlyAvoid)
				{
					break;
				}
				for (; i < k; i++)
				{
					fprintf(f, "A");
				}

			Point pt(i, j);
			char c = '.';
			if (OneSpaceHasFlag(ActMap::ClosedDoor, pt, false))
				c = 'D';
			if (OneSpaceHasFlag(ActMap::Object, pt, false))
				c = 'O';
			if (OneSpaceHasFlag(ActMap::NPCCollision, pt, false))
				c = 'N';
			if (OneSpaceHasFlag(ActMap::BlockPlayer, pt, false))
				c = 'P';
			if (OneSpaceHasFlag(ActMap::BlockWalk, pt, false))
				c = 'W';
			if(OneSpaceHasFlag(ActMap::Avoid, pt, false))
				c = 'A';
			if(exitPoints.end() != exitPoints.find(pt))
				c = '%';
			fprintf(f, "%c", c);
		}
		fprintf(f, "\n");
	}
	fclose(f);
	LeaveCriticalSection(lock);
}

}
