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
	BLOCK_WALL_L = 2, BLOCK_WALL_R = 3, BLOCK_SQUARE1_R = 4, BLOCK_SQUARE1_B = 5, BLOCK_SQUARE1_BL = 6,
	BLOCK_SQUARE1_BR = 7, BLOCK_SQUARE1_TR = 8, BLOCK_CORNER = 9,

	// id >= 100: entities' initial locations
	PLAYER = 100,

	//Intervals
	STATIC_FIRST = BLOCK_SQUARE1,
	STATIC_LAST = BLOCK_CORNER,
	SOLID_FIRST = BLOCK_SQUARE1,
	SOLID_LAST = BLOCK_WALL_R,
	ENTITY_FIRST = PLAYER,
	ENTITY_LAST = PLAYER
};

class TileMap
{
public:
	TileMap();
	~TileMap();

	AppStatus Initialise();
	AppStatus Load(int data[], int w, int h);
	void Update();
	void Render();
	void Release();

	//Test for collisions with walls
	bool TestCollisionWallLeft(const AABB& box) const;
	bool TestCollisionWallRight(const AABB& box) const;
	
	//Test collision with the ground and update 'py' with the maximum y-position to prevent
	//penetration of the grounded tile, that is, the pixel y-position above the grounded tile.
	//Grounded tile = solid tile (blocks).
	bool TestCollisionGround(const AABB& box, int *py) const;
	
	//Test if there is a ground tile one pixel below the given box
	bool TestFalling(const AABB& box) const;

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

