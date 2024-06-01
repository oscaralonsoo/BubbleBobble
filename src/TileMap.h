#pragma once
#include <raylib.h>
#include "Sprite.h"
#include "Point.h"
#include "AABB.h"
#include "Globals.h"
#include <unordered_map>

enum class Tile {

	// -1: area covered by an entity
	EMPTY = -1,
	//  0: air tile
	AIR = 0,

	// 0 < id < 50: static tiles
	BLOCK_SQUARE1 = 1,
	BLOCK_WALL_L1 = 2, BLOCK_WALL_R1 = 3, BLOCK_SQUARE2 = 4, BLOCK_SQUARE3 = 5, BLOCK_WALL_UL3 = 6, BLOCK_WALL_UR3= 7, BLOCK_WALL_DL3 = 8, BLOCK_WALL_DR3 = 9,
	BLOCK_SQUARE4 = 10, BLOCK_WALL_UL4 = 11, BLOCK_WALL_UR4 = 12, BLOCK_WALL_DL4 = 13, BLOCK_WALL_DR4 = 14, BLOCK_VSHADOW1 = 15, BLOCK_HSHADOW1 = 16, BLOCK_HFINAL1 = 17,
	BLOCK_UPCORNER1 = 18, BLOCK_VFINAL1 = 19, BLOCK_CORNER1 = 20, BLOCK_CORNER2 = 21, BLOCK_UPCORNER2 = 22, BLOCK_VSHADOW2 = 23, BLOCK_HSHADOW2 = 24, BLOCK_VFINAL2 = 25,
	BLOCK_HFINAL2 = 26, BLOCK_HSHADOW3 = 27, BLOCK_VFINAL3 = 28, BLOCK_CORNER3 = 29, BLOCK_VSHADOW3 = 30, BLOCK_UPCORNER3 = 31, BLOCK_HFINAL3 = 32, BLOCK_HSHADOW4 = 33,
	BLOCK_VFINAL4 = 34, BLOCK_CORNER4 = 35, BLOCK_VSHADOW4 = 36, BLOCK_UPCORNER4 = 37, BLOCK_HFINAL4 = 38,

	// 50 <= id < 100: special tiles
	ITEM_BANANA = 50, ITEM_CHILI,

	// id >= 100: entities' initial locations
	PLAYER = 100,
	ZENCHAN = 101,
	MONSTA = 102,

	//Intervals
	SOLID_FIRST = BLOCK_SQUARE1,
	SOLID_LAST = BLOCK_WALL_DR4,
	OBJECT_FIRST = ITEM_BANANA,
	OBJECT_LAST = ITEM_CHILI,
	ENTITY_FIRST = PLAYER,
	ENTITY_LAST = MONSTA
};

class TileMap
{
public:
	TileMap();
	~TileMap();

	AppStatus Initialise();
	AppStatus Load(int data[], int w, int h);
	void ClearObjectEntityPositions();

	void Update();
	void Render();
	void Release();

	bool IsTileObject(Tile tile) const;
	bool IsTileEntity(Tile tile) const;

	//Test for collisions with walls
	bool TestCollisionWallLeft(const AABB& box) const;
	bool TestCollisionWallRight(const AABB& box) const;
	
	//Test collision with the ground and update 'py' with the maximum y-position to prevent
	//penetration of the grounded tile, that is, the pixel y-position above the grounded tile.
	//Grounded tile = solid tile (blocks).
	bool TestCollisionGround(const AABB& box, int *py) const;
	
	//Test if there is a ground tile one pixel below the given box
	bool TestFalling(const AABB& box) const;

	//Test if there is a ground tile one pixel below the given box before falling
	bool TestBeforeFalling(const AABB& box, int dir) const;

	//Given a hitbox, computes the maximum swept box model along the X-axis without solid tiles
	AABB GetSweptAreaX(const AABB& hitboxbox) const;

private:
	void InitTileDictionary();

	Tile GetTileIndex(int x, int y) const;
	bool IsTileSolid(Tile tile) const;
	bool CollisionX(const Point& p, int distance) const;
	bool CollisionY(const Point& p, int distance) const;

	//Tile map
	Tile *map;

	//Size of the tile map
	int size, width, height;
	
	//Dictionary of tile frames
	std::unordered_map<int, Rectangle> dict_rect;

	//Tile sheet
	const Texture2D *img_tiles;
};

