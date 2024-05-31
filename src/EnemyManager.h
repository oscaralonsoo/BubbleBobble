#pragma once
#include "Enemy.h"
#include "TileMap.h"

class EnemyManager
{
public:
	EnemyManager();
	~EnemyManager();

	AppStatus Initialise();

	//Set the TileMap reference for managing enemy collisions
	void SetTileMap(TileMap* tilemap);

	//Add a new enemy with the given position, type, action area and looking direction
	void Add(const Point& pos, EnemyType type, const AABB& area, Look look = Look::RIGHT);

	//Retrieve the hitbox of an enemy based on the given position and type
	AABB GetEnemyHitBox(const Point& pos, EnemyType type) const;

	//Retrieve the hitbox of all the enemies in the scene
	std::vector<AABB> GetHitBoxes() const;

	//Retrieve the hitbox of all the enemies in the scene
	std::vector<Enemy*> GetEnemies() const;

	//Update enemies according to their logic. If the given player hitbox is visible to them,
	//they will shoot by adding shots to the ShotManager
	void Update(const AABB& player_hitbox);

	//Draw all enemies
	void Draw() const;

	//Draw enemy hitboxes for debugging
	void DrawDebug() const;

	//Delete all enemies and clear the enemy vector
	void Release();

private:
	std::vector<Enemy*> enemies;
};

