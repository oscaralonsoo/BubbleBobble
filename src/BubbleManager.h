#pragma once
#pragma once
#include <array>
#include "Bubble.h"
#include "TileMap.h"
#include "ParticleManager.h"
#include "EnemyManager.h"

#define MAX_BUBBLES		16

class BubbleManager
{
public:
	BubbleManager();
	~BubbleManager();

	AppStatus Initialise();

	//Set the TileMap reference for managing bubble collisions
	void SetTileMap(TileMap* tilemap);

	//Set the particles manager for bubble explosion
	void SetParticleManager(ParticleManager* particles);

	//Set the enemies for interactions
	void SetEnemies(std::vector<Enemy*> enemies);

	//Add a new shot with the given position and direction
	void Add(const Point& pos, const Point& dir);

	//Remove all the shots
	void Clear();

	//Update shot positions and check for collisions with the level and player
	void Update(const AABB& player_hitbox);

	//Draw all active shots
	void Draw() const;

	//Draw hitboxes of shots for debugging
	void DrawDebug(const Color& col) const;

private:
	std::array<Bubble, MAX_BUBBLES> bubbles;

	//Reference to the TileMap object
	//This class does not own the object, it only holds a reference to it
	TileMap* map;

	//Reference to the TileMap object
	//This class does not own the object, it only holds a reference to it
	ParticleManager* particles;

	std::vector<Enemy*> enemies;
};

