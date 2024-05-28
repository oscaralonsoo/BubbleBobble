#pragma once
#include "Entity.h"
#include "TileMap.h"

//Representation model size: 48x48
#define ZENCHAN_FRAME_SIZE		48
//Logical model size: 35x35
#define ZENCHAN_PHYSICAL_WIDTH	35
#define ZENCHAN_PHYSICAL_HEIGHT	35

//Representation model size: 32x32
#define TURRET_FRAME_SIZE		32
//Logical model size: 0x0
#define TURRET_PHYSICAL_WIDTH	32
#define TURRET_PHYSICAL_HEIGHT	26

enum class EnemyType { ZENCHAN, TURRET };

class Enemy : public Entity
{
public:
	Enemy(const Point& p, int width, int height, int frame_width, int frame_height);
	virtual ~Enemy();

	//Draw the maximum visibility area of the enemy
	void DrawVisibilityArea(const Color& col) const;

	//Pure virtual functions, any class inheriting from this class must provide its own implementations

	//Initialize the enemy with the specified look and area
	virtual AppStatus Initialise(Look look, const AABB& area) = 0;

	//Set the TileMap reference for managing enemy collisions
	void SetTileMap(TileMap* tilemap);

	//Update the enemy according to its logic, return true if the enemy must shoot
	virtual bool Update(const AABB& box) = 0;

	//Retrieve the position and direction of the shot to be thrown
	virtual void GetShootingPosDir(Point* pos, Point* dir) const = 0;

protected:
	//Return true if the given hitbox is within the visibility area and the enemy is facing it
	bool IsVisible(const AABB& hitbox);

	Look look;
	AABB visibility_area;

	TileMap* map;
};

