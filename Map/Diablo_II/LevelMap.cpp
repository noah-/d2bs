#include <algorithm>
#include <assert.h>

#include "LevelMap.h"
#include "D2Structs.h"
#include <map>

namespace Mapping
{

LevelMapList LevelMap::cache = LevelMapList();

LevelMap* LevelMap::GetMap(Level* level)
{
	if(cache.count(level->dwLevelNo) > 0)
		return cache[level->dwLevelNo];
	LevelMap* map = new LevelMap(level);
	cache[level->dwLevelNo] = map;
	return map;
}

void LevelMap::ClearCache(void)
{
	LevelMapList::iterator end = cache.end();
	for(LevelMapList::iterator it = cache.begin(); it != end; it++)
		delete it->second;
	cache.clear();
}

LevelMap::LevelMap(const Level* level)
{
	lock = new CRITICAL_SECTION;
	InitializeCriticalSection(lock);

	assert(act != NULL);
	assert(level != NULL);

	this->level = level;
	this->act = level->pMisc->pAct;

	// get the map size
	height = level->dwSizeX * 5;
	width  = level->dwSizeY * 5;
	posX   = level->dwPosX * 5;
	posY   = level->dwPosY * 5;

	bool added = false;
	Room2* room = level->pRoom2First;
	if(!level->pRoom2First->pRoom1)
	{
		AddRoomData(room);
		added = true;
	}

	Build();

	if(added)
		RemoveRoomData(room);

	posX   = (level->dwPosX == -1 ? 0 : level->dwPosX * 5);
	posY   = (level->dwPosY == -1 ? 0 : level->dwPosY * 5);
}

LevelMap::~LevelMap(void)
{
	DeleteCriticalSection(lock);
	delete lock;
	lock = NULL;
}

void LevelMap::Build(void)
{
	EnterCriticalSection(lock);

	mapPoints = Matrix<CollisionFlag>(height, width, LevelMap::Avoid);

	RoomList addedRooms;
	UnitAny* player = GetPlayerUnit();
	AddRoom(level->pRoom2First, addedRooms, player);

	FillGaps();
	ThickenWalls();
	DrillExits();

	LeaveCriticalSection(lock);
}

void LevelMap::AddRoom(Room2* const room, RoomList& rooms, UnitAny* player)
{
	if(!room || room->pLevel->dwLevelNo != level->dwLevelNo)
		return;

	// ignore rooms we've seen before
	if(std::find(rooms.begin(), rooms.end(), room) != rooms.end())
		return;

	rooms.push_back(room);

	// add the room if necessary
	bool added = false;
	if(!room->pRoom1)
	{
		AddRoomData(room);
		added = true;
	}

	// add the collision data to the map
	if(room->pRoom1)
		AddCollisionMap(room->pRoom1);

	// now do the same to every room near this one
	Room2** pRooms = room->pRoom2Near;
	for(DWORD i = 0; i < room->dwRoomsNear; i++)
		AddRoom(pRooms[i], rooms, player);

	// and remove the room afterwards if we added it
	if(added)
		RemoveRoomData(room);
}

void LevelMap::AddCollisionMap(Room1* pRoom1)
{
	CollMap* map = pRoom1->Coll;
	if(map == NULL)
		return;

	int	x = map->dwPosGameX - posX, y = map->dwPosGameY - posY,
		height = map->dwPosGameX + map->dwSizeGameX - posX, width = map->dwPosGameY + map->dwSizeGameY - posY;

	WORD* p = map->pMapStart;

	for(int i = y; i < width && p != map->pMapEnd; i++)
	{
		for(int j = x; j < height && p != map->pMapEnd; j++, p++)
		{
			if ((*p & LevelMap::ClosedDoor) == LevelMap::ClosedDoor || (*p & LevelMap::Player) == LevelMap::Player)
			{
				SetCollisionData(j, i, 0);
			}
			else if
			(
				(
					(*p & LevelMap::FriendlyNPC)  == LevelMap::FriendlyNPC ||
					(*p & LevelMap::NPCCollision) == LevelMap::NPCCollision ||
					(*p & LevelMap::NPCLocation)  == LevelMap::NPCLocation ||
					(*p & LevelMap::DeadBody)     == LevelMap::DeadBody ||
					(*p & LevelMap::Item)         == LevelMap::Item

				)
				&&	
				(*p & LevelMap::BlockWalk) != LevelMap::BlockWalk &&
				(*p & LevelMap::Wall)      != LevelMap::Wall &&
				(*p & LevelMap::Object)    != LevelMap::Object
			)
			{
				SetCollisionData(j, i, 0);
			}
			else if (
					(*p & LevelMap::AlternateTile) == LevelMap::AlternateTile  &&
					(*p & LevelMap::BlockWalk) != LevelMap::BlockWalk &&
					(*p & LevelMap::Wall)      != LevelMap::Wall
			)
			{
				SetCollisionData(j, i, 0);
			}
			else
			{
				SetCollisionData(j, i, *p);
			}
		}
	}

	for(UnitAny* pUnit = pRoom1->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
	{
		char tmp[128] = "";
		GetUnitName(pUnit, tmp, 128);
		if (
			pUnit->dwType == 2 && (
				0 == strcmp(tmp, "Barrel") ||
				0 == strcmp(tmp, "Door") ||
				0 == strcmp(tmp, "door") ||
				0 == strcmp(tmp, "Urn") ||
				0 == strcmp(tmp, "LargeUrn")
			)
		)
		{
			Point loc(D2CLIENT_GetUnitX(pUnit),D2CLIENT_GetUnitY(pUnit));
			SetCollisionData(loc.first - posX, loc.second - posY, 0);
		}
	}
}
void LevelMap::SetCollisionData(int x, int y, int value)
{
	// sync this to ensure that we don't try to add to a dead map or something
	EnterCriticalSection(lock);

	assert(IsValidPoint(Point(x, y), false));
	mapPoints.SetPoint(x, y, (CollisionFlag)value);

	LeaveCriticalSection(lock);
}

bool LevelMap::IsGap(int x, int y, bool abs) const
{
	const int gapSize = 3;
	Point pt(x, y);
	assert(IsValidPoint(pt, abs));
	if(SpaceIsWalkable(pt, abs))
		return false;

	int spaces = 0;

	for(int i = x-gapSize-1; i <= x+gapSize+1 && spaces < gapSize; i++)
	{
		Point pt(i, y);
		if(IsValidPoint(pt, abs))
			spaces = (SpaceIsWalkable(pt, abs) ? 0 : spaces+1);
	}

	if(spaces < gapSize)
		return true;

	for(int i = y-gapSize-1, spaces = 0; i <= y+gapSize+1 && spaces < gapSize; i++)
	{
		Point pt(x, i);
		if(IsValidPoint(pt, abs))
			spaces = (SpaceIsWalkable(pt, abs) ? 0 : spaces+1);
	}

	return spaces < gapSize;
}
void LevelMap::FillGaps(void)
{
	EnterCriticalSection(lock);

	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			if(IsGap(i, j, false))
				SetCollisionData(i, j, LevelMap::Avoid);

	LeaveCriticalSection(lock);
}
void LevelMap::ThickenWalls(void)
{
	const int ThickenAmount = 1;

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			Point pt(i, j);
			if(SpaceIsWalkable(pt, false) || GetMapData(pt, false) == LevelMap::ThickenedWall)
				continue;

			for(int k=-1, x = i-ThickenAmount; x <= i+ThickenAmount; k++, x++)
			{
				for(int l=-1, y = j-ThickenAmount; y <= j+ThickenAmount; l++, y++)
				{
					if ((k == 0 && l == 0) || (k != 0 && l != 0))
						continue;

					Point point(x, y);
					if(IsValidPoint(point, false) && SpaceIsWalkable(point, false))
						SetCollisionData(x, y, LevelMap::ThickenedWall);
				}
			}
		}
	}
}

Point LevelMap::AbsToRelative(const Point& point) const
{
	return Point(point.first - posX, point.second - posY);
}
Point LevelMap::RelativeToAbs(const Point& point) const
{
	return Point(point.first + posX, point.second + posY);
}

int LevelMap::GetMapData(const Point& point, bool abs) const
{
	if(abs) return GetMapData(AbsToRelative(point), false);

	assert(IsValidPoint(point, abs));

	EnterCriticalSection(lock);
	CollisionFlag value = mapPoints.GetPoint(point.first, point.second);
	LeaveCriticalSection(lock);

	return value;
}
bool LevelMap::IsValidPoint(const Point& point, bool abs) const
{
	if(abs) return IsValidPoint(AbsToRelative(point), false);
	return !!(point.first >= 0 && point.second >= 0 && point.first < height && point.second < width);
}
void LevelMap::GetExits(ExitArray& exits) const
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
	FindRoomLinkageExits(exits, added);

	RoomList::iterator start = added.begin(), last = added.end();
	for(RoomList::iterator it = start; it != last; it++)
		RemoveRoomData(*it);

	LeaveCriticalSection(lock);
}

void LevelMap::DrillExits()
{
	ExitArray exits;
	GetExits(exits);
	ExitArray::iterator begin = exits.begin(), last = exits.end();
	for(ExitArray::iterator it = begin; it != last; it++)
	{
		SetCollisionData(it->Position.first - posX, it->Position.second - posY, 0);
		bool blind = true;
		for(int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				if (x == 0 && y == 0)
				{
					continue;
				}
				if (IsValidPoint(Point(it->Position.first + x, it->Position.second + y), true) && SpaceIsWalkable(Point(it->Position.first + x, it->Position.second + y), true))
				{
					blind = false;
					break;
				}
			}
			if (blind == false)
			{
				break;
			}
		}
		if (blind == false)
		{
			continue;
		}
		for(int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				if ((x != 0 && y != 0) || (x == 0 && y == 0))
				{
					continue;
				}
				if (IsValidPoint(Point(it->Position.first + x, it->Position.second + y), true))
				{
					SetCollisionData(it->Position.first + x - posX, it->Position.second + y - posY, 0);
				}
			}
		}
		for(int x = -2; x <= 2; x++)
		{
			for (int y = -2; y <= 2; y++)
			{
				if (((x < -1 || x > 1)  && (y < -1 || y > 1)) || (x == 0 && y == 0))
				{
					continue;
				}
				if (IsValidPoint(Point(it->Position.first + x, it->Position.second + y), true) && SpaceHasFlag(LevelMap::ThickenedWall, Point(it->Position.first + x, it->Position.second + y), true))
				{
					SetCollisionData(it->Position.first + x - posX, it->Position.second + y - posY, 0);
				}
			}
		}
	}
}

bool LevelMap::ExitExists(Point loc, ExitArray& exits) const
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

bool LevelMap::ExitExists(DWORD dwLevelNo, ExitArray& exits) const
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

void LevelMap::FindRoomLinkageExits(ExitArray& exits, RoomList& added) const
{
	using namespace std;
	static const Point empty(0,0);

	UnitAny* player = GetPlayerUnit();
	const Point playerOrigin(player->pPath->xPos - posX, player->pPath->yPos - posY);

	multimap<int, std::pair<Point, std::pair<Point, int>>> exitMap; // <level, <rooms[i], <middlepoint, size> > >
	
	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		Room2** rooms = room->pRoom2Near;
		for(DWORD i = 0; i < room->dwRoomsNear; i++)
		{
			if(!rooms[i]->pRoom1)
			{
				AddRoomData(rooms[i]);
				added.push_back(rooms[i]);
			}

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
				// Print("ÿc1d2bsÿc3LevelMap::GetExitsÿc0 adjecent room is out of reach - it's not even touching local room in corner!");
				continue;
			}
			if (overlappingX > 0 && overlappingY > 0)
			{
				// Print("ÿc1d2bsÿc3LevelMap::GetExitsÿc0 WTF local and adjecent rooms are overlapping (they share some points)!!!");
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
				isPointWalkable = EdgeIsWalkable(currentPoint, orthogonalDirection, rooms[i]->pRoom1, true);
				if (isPointWalkable)
				{
					lastWalkablePoint = currentPoint;
					spaces++;
				}
				if (false == isPointWalkable || j + 1 == edgeSize)
				{
					if (spaces > 2)
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

Point LevelMap::GetEdgeCenterPoint(const Point &currentPoint, const Point &edgeDirection) const
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

void LevelMap::FindRoomTileExits(Room2* room, ExitArray& exits) const
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
bool LevelMap::EdgeIsWalkable(const Point& edgePoint, const Point& offsetPoint, Room1 *pRoom1Adjecent, bool abs) const
{
	int k;
	const int distanceLocal    = -2;
	const int distanceAdjecent =  3;
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
			if (false == RoomSpaceIsWalkable(pRoom1Adjecent, currentPoint, abs))
			{
				break;
			}
		}
	}
	return k > distanceAdjecent;
}

bool LevelMap::SpaceHasFlag(int flag, const Point& point, bool abs) const
{
	return ((GetMapData(point, abs) & flag) == flag);
}
bool LevelMap::PathHasFlag(int flag, const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceHasFlag(flag, points[i], abs))
			return false;
	return true;
}

bool LevelMap::SpaceIsWalkable(const Point& point, bool abs) const
{
	// ignore closed doors here, because we want to path through them
	return !(!IsValidPoint(point, abs)                        ||
			 SpaceHasFlag(LevelMap::Avoid, point, abs)        ||
		     SpaceHasFlag(LevelMap::BlockWalk, point, abs)    ||
			 SpaceHasFlag(LevelMap::BlockPlayer, point, abs)  ||
			 SpaceHasFlag(LevelMap::NPCCollision, point, abs) ||
		     SpaceHasFlag(LevelMap::Object, point, abs)       ||
			 SpaceHasFlag(LevelMap::ThickenedWall, point, abs)
			 );
}

bool LevelMap::SpaceIsWalkableForExit(const Point& point, bool abs) const
{
	// ignore almost everything
	return !(!IsValidPoint(point, abs)                        ||
		     SpaceHasFlag(LevelMap::Avoid, point, abs)        ||
			 SpaceHasFlag(LevelMap::BlockWalk, point,  abs)   ||
		     SpaceHasFlag(LevelMap::BlockPlayer, point, abs)
			);
}

bool LevelMap::RoomSpaceIsWalkable(Room1 *pRoom1, const Point& point, bool abs)const
{
	unsigned int x = point.first, y = point.second;
	if (abs)
	{
		// convert to origin
		x -= pRoom1->Coll->dwPosGameX; // already x5
		y -= pRoom1->Coll->dwPosGameY; // already x5
	}
	// out of room?
	if (x >= pRoom1->Coll->dwSizeGameX || y >= pRoom1->Coll->dwSizeGameY || x < 0 || y < 0)
	{
		return false;
	}
	// p = pointer to space in room1 - by relative coords
	WORD *p = pRoom1->Coll->pMapStart // origin
	        + x // x
			+ pRoom1->Coll->dwSizeGameX * y; // y
	bool output = ValueIsWalkable(p);
	return output;
}

bool LevelMap::ValueIsWalkable(const WORD *value) const
{
	return !(
		ValueHasFlag(LevelMap::BlockWalk, value) ||
		ValueHasFlag(LevelMap::BlockPlayer, value) ||
		ValueHasFlag(LevelMap::Object, value)
	);
}

bool LevelMap::ValueHasFlag(int flag, const WORD *value) const
{
	return ((*value & flag) == flag);
}

bool LevelMap::SpaceHasLineOfSight(const Point& point, bool abs) const
{
	return !SpaceHasFlag(LevelMap::Avoid, point, abs) && !SpaceHasFlag(LevelMap::BlockLineOfSight, point, abs);
}

bool LevelMap::SpaceIsInvalid(const Point& point, bool abs) const { return SpaceHasFlag(LevelMap::Avoid, point, abs); }

bool LevelMap::SpaceIsThickenedWall(const Point& point, bool abs) const { return SpaceHasFlag(LevelMap::ThickenedWall, point, abs); }

bool LevelMap::PathIsWalkable(const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceIsWalkable(points[i], abs))
			return false;
	return true;
}
bool LevelMap::PathHasLineOfSight(const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceHasLineOfSight(points[i], abs))
			return false;
	return true;
}

void LevelMap::Dump(const char* file, const PointList& points) const
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
			if(SpaceHasFlag(LevelMap::Avoid, pt, false))
				c = 'A';
			else if(pt == first)
				c = 'S';
			else if(pt == last)
				c = 'E';
			else if(std::find(begin, end, pt) != end)
				c = 'P';
			else if(SpaceIsInvalid(pt, false))
				c = '?';
			else if(GetMapData(pt, false) == LevelMap::ThickenedWall)
				c = 'T';
			else if(!SpaceIsWalkable(pt, false))
				c = 'X';
			fprintf(f, "%c", c);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

}
