#pragma once

#include "..\Map.h"

#include <list>
#include <map>

#include "D2Structs.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"

namespace Mapping
{

enum ExitType
{
	Linkage = 1,
	Tile = 2
};

struct Exit
{
public:
	DWORD Target;
	Point Position;
	ExitType Type;
	DWORD TileId;

	Exit(Point location, DWORD levelId, ExitType type, DWORD tileId) :
		Target(levelId), Position(location), Type(type), TileId(tileId) {}
};

class LevelMap;

typedef std::vector<Exit> ExitArray;
typedef std::map<DWORD, LevelMap*> LevelMapList;

class LevelMap : public Map
{
public:
	enum CollisionFlag {
		None				= 0x00000,
		BlockWalk			= 0x00001,
		BlockLineOfSight	= 0x00002,
		Wall				= 0x00004,
		BlockPlayer			= 0x00008,
		AlternateTile		= 0x00010,
		Blank				= 0x00020,
		Missile				= 0x00040,
		Player				= 0x00080,
		NPCLocation			= 0x00100,
		Item				= 0x00200,
		Object				= 0x00400,
		ClosedDoor			= 0x00800,
		NPCCollision		= 0x01000,
		FriendlyNPC			= 0x02000,
		Unknown				= 0x04000,
		DeadBody			= 0x08000, // also portal
		ThickenedWall		= 0x10000,
		Avoid				= 0x20000
	};

private:
	typedef std::list<Room2*> RoomList;

	static LevelMapList cache;

	Act* act;
	const Level* level;
	int width, height;
	int posX, posY;
	Matrix<CollisionFlag> mapPoints;
	CRITICAL_SECTION* lock;

	void Build(void);

	static inline int GetLevelNo(Room2* room, DWORD tile) { return GetTileLevelNo(room, tile); }

	void AddRoom(Room2* const room, RoomList& rooms, UnitAny* player);
	void AddCollisionMap(const CollMap* const map);
	void SetCollisionData(int x, int y, int value);
	bool IsGap(int x, int y, bool abs) const;
	void FillGaps(void);
	void ShrinkMap(void);
	void ThickenWalls(void);

	LevelMap(const Level* level);
	~LevelMap(void);

public:

	static LevelMap* GetMap(Level* level);
	static void ClearCache(void);
	void Dump(const char* file, const PointList& points) const;

	Point AbsToRelative(const Point& point) const;
	Point RelativeToAbs(const Point& point) const;

	inline int GetWidth(void) const { return width; }
	inline int GetHeight(void) const { return height; }
	inline int GetPosX(void) const { return posX; }
	inline int GetPosY(void) const { return posY; }
	/* If these are backwards, it's not me. */
	inline int GetMaxX(void) const { return posX + height; }
	inline int GetMaxY(void) const { return posY + width; }

	int GetMapData(const Point& point, bool abs = true) const;
	bool IsValidPoint(const Point& point, bool abs = true) const;

	void GetExits(ExitArray&) const;

	bool SpaceHasFlag(int flag, const Point& point, bool abs = true) const;
	bool PathHasFlag(int flag, const PointList& points, bool abs = true) const;

	bool SpaceIsWalkable(const Point& point, bool abs = true) const;
	bool SpaceHasLineOfSight(const Point& point, bool abs = true) const;
	bool SpaceIsInvalid(const Point& point, bool abs = true) const;
	bool SpaceIsThickenedWall(const Point& point, bool abs = true) const;

	bool PathIsWalkable(const PointList& points, bool abs = true) const;
	bool PathHasLineOfSight(const PointList& points, bool abs = true) const;

	bool RoomSpaceIsWalkable(Room1 *pRoom1, const Point& point, bool abs) const;
	bool ValueIsWalkable(const WORD *value) const;
	bool ValueHasFlag(int flag, const WORD *value) const;
};

}
