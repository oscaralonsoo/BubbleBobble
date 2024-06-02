#pragma once
#include "Entity.h"
#include "TileMap.h"
#include "Object.h";

//Representation model size: 48x48
#define ZENCHAN_FRAME_SIZE		48
//Logical model size: 35x35
#define ZENCHAN_PHYSICAL_WIDTH	35
#define ZENCHAN_PHYSICAL_HEIGHT	35

//Representation model size: 48x48
#define MONSTA_FRAME_SIZE		48
//Logical model size: 0x0
#define MONSTA_PHYSICAL_WIDTH	35
#define MONSTA_PHYSICAL_HEIGHT	35

enum class EnemyType { ZENCHAN, MONSTA };

class Enemy : public Entity
{
public:
	Enemy(const Point& p, int width, int height, int frame_width, int frame_height);
	virtual ~Enemy();

	//Draw the maximum visibility area of the enemy
	void DrawVisibilityArea(const Color& col) const;

	//Pure virtual functions, any class inheriting from this class must provide its own implementations

	//Initialize the enemy with the specified look and area
	virtual AppStatus Initialise(Look look) = 0;

	//Set state when enemies get killed
	virtual void KillEnemy();

	int GetAnimation();

	//Update the enemy according to its logic, return true if the enemy must shoot
	virtual void Update(std::vector<Object*> objects, TileMap* tilemap) = 0;

protected:
	//Return true if the given hitbox is within the visibility area and the enemy is facing it
	bool IsVisible(const AABB& hitbox);

	Look look;
	AABB visibility_area;
};

