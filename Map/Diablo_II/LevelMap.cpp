#include <algorithm>
#include <assert.h>

#include "LevelMap.h"
#include "D2Structs.h"

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

	mapPoints = Matrix<CollisionFlag>(height, width);

	RoomList addedRooms;
	UnitAny* player = GetPlayerUnit();
	AddRoom(level->pRoom2First, addedRooms, player);

	FillGaps();
	ThickenWalls();

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
		AddCollisionMap(room->pRoom1->Coll);

	// now do the same to every room near this one
	Room2** pRooms = room->pRoom2Near;
	for(DWORD i = 0; i < room->dwRoomsNear; i++)
		AddRoom(pRooms[i], rooms, player);

	// and remove the room afterwards if we added it
	if(added)
		RemoveRoomData(room);
}
void LevelMap::AddCollisionMap(const CollMap* const map)
{
	if(map == NULL)
		return;

	int	x = map->dwPosGameX - posX, y = map->dwPosGameY - posY,
		height = map->dwPosGameX + map->dwSizeGameX - posX, width = map->dwPosGameY + map->dwSizeGameY - posY;

	WORD* p = map->pMapStart;

	for(int i = y; i < width && p != map->pMapEnd; i++)
		for(int j = x; j < height && p != map->pMapEnd; j++, p++)
			SetCollisionData(j, i, *p);
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

			for(int x = i-ThickenAmount; x <= i+ThickenAmount; x++)
			{
				for(int y = j-ThickenAmount; y <= j+ThickenAmount; y++)
				{
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

		Point x1(room->dwPosX * 5, room->dwPosY * 5),
			  x2(room->dwPosX * 5, room->dwPosY * 5 + room->dwSizeY * 5),
			  x3(room->dwPosX * 5 + room->dwSizeX * 5, room->dwPosY * 5 + room->dwSizeY * 5),
			  x4(room->dwPosX * 5 + room->dwSizeX * 5, room->dwPosY * 5);

		Room2** rooms = room->pRoom2Near;
		for(DWORD i = 0; i < room->dwRoomsNear; i++)
		{
			if(!rooms[i]->pRoom1)
			{
				AddRoomData(rooms[i]);
				added.push_back(rooms[i]);
			}

			if(rooms[i]->pLevel->dwLevelNo != level->dwLevelNo)
			{
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

				// definitely a link, is it walkable?
				// find the side most adjacent to this one
				Point y1(rooms[i]->dwPosX * 5, rooms[i]->dwPosY * 5),
					  y2(rooms[i]->dwPosX * 5, rooms[i]->dwPosY * 5 + rooms[i]->dwSizeY * 5),
					  y3(rooms[i]->dwPosX * 5 + rooms[i]->dwSizeX * 5, rooms[i]->dwPosY * 5 + rooms[i]->dwSizeY * 5),
					  y4(rooms[i]->dwPosX * 5 + rooms[i]->dwSizeX * 5, rooms[i]->dwPosY * 5);

				Point start(0,0), end(0,0);

				if(x1 == y4 && x2 == y3)      { start = x1; end = x2; }
				else if(x2 == y1 && x3 == y4) { start = x2; end = x3; }
				else if(x3 == y2 && x4 == y1) { start = x3; end = x4; }
				else if(x4 == y3 && x1 == y2) { start = x4; end = x1; }

				if(start != empty && end != empty)
				{
					if(start.first == GetMaxX())  start.first--;
					if(start.second == GetMaxY()) start.second--;
					if(end.first == GetMaxX())    end.first--;
					if(end.second == GetMaxY())   end.second--;

					int xstep = end.first - start.first,
						ystep = end.second - start.second;
					xstep = xstep > 0 ? 1 : (xstep < 0 ? -1 : 0);
					ystep = ystep > 0 ? 1 : (ystep < 0 ? -1 : 0);

					int spaces = 0;
					Point midpoint(start.first, start.second);
					bool found = false;
					
					// offset for neighbouring room's edge points
					int offsetX = 0, offsetY = 0;
					if (xstep != 0) // top or bottom edge
					{
						if (start.second == x1.second) // top
						{
							offsetY--;
						}
						else
						{
							offsetY++;
						}
					}
					if (ystep != 0) // left or right edge
					{
						if (start.first == x1.first) // left
						{
							offsetX--;
						}
						else
						{
							offsetX++;
						}
					}

					for(int x = 0, y = 0; !found && midpoint != end; x += xstep, y += ystep)
					{
						midpoint = Point(start.first + x, start.second + y);
						if(SpaceIsWalkable(midpoint, true))
						{
							int k;
							for (k = 1; k <= 3; k++) // trying to simulate character (3x3) staying in another room
							{
								if (false == RoomSpaceIsWalkable(rooms[i]->pRoom1, Point(midpoint.first + offsetX * k, midpoint.second + offsetY * k), true))
								{
									break;
								}
							}
							if (k <= 3)
							{
								spaces = 0;
								continue;
							}
							spaces++;
						}
						else if (spaces > 2)
						{
							midpoint = Point((midpoint.first - xstep) - xstep * spaces / 2, (midpoint.second - ystep) - ystep * spaces / 2);
							exits.push_back(Exit(midpoint, rooms[i]->pLevel->dwLevelNo, Linkage, 0));
							found = true;
						}
						else
						{
							spaces = 0;
						}
					}
				}
			}
		}

		for(PresetUnit* preset = room->pPreset; preset; preset = preset->pPresetNext)
		{
			if(preset->dwType == 5) // UNIT_TILE
			{
				DWORD levelId = GetLevelNo(room, preset->dwTxtFileNo);
				if(levelId != 0)
				{
					Point loc((room->dwPosX*5)+preset->dwPosX, (room->dwPosY*5)+preset->dwPosY);
					bool exists = false;

					ExitArray::iterator start = exits.begin(), end = exits.end();
					for(ExitArray::iterator it = start; it != end; it++)
					{
						if(it->Position == loc)
						{
							exists = true;
							break;
						}
					}

					if(!exists)
					{
						exits.push_back(Exit(loc, levelId, Tile, preset->dwTxtFileNo));
					}
				}
			}
		}
	}

	RoomList::iterator start = added.begin(), last = added.end();
	for(RoomList::iterator it = start; it != last; it++)
		RemoveRoomData(*it);

	LeaveCriticalSection(lock);
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
	return !SpaceHasFlag(LevelMap::Avoid, point, abs)		  &&
		   !(SpaceHasFlag(LevelMap::BlockWalk, point, abs) || SpaceHasFlag(LevelMap::BlockPlayer, point, abs)  ||
		     SpaceHasFlag(LevelMap::ClosedDoor, point, abs) || SpaceHasFlag(LevelMap::NPCCollision, point, abs) ||
		     SpaceHasFlag(LevelMap::Object, point, abs));
}

bool LevelMap::RoomSpaceIsWalkable(Room1 *pRoom1, const Point& point, bool abs) const
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
	EnterCriticalSection(lock);
	WORD *p = pRoom1->Coll->pMapStart // origin
	        + x // x
			+ pRoom1->Coll->dwSizeGameX * y; // y
	bool output = ValueIsWalkable(p);
	LeaveCriticalSection(lock);
	return output;
}

bool LevelMap::ValueIsWalkable(const WORD *value) const
{
	return !(
		ValueHasFlag(LevelMap::BlockWalk, value) ||
		ValueHasFlag(LevelMap::BlockPlayer, value) ||
		ValueHasFlag(LevelMap::ClosedDoor, value) ||
		ValueHasFlag(LevelMap::NPCCollision, value) ||
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
