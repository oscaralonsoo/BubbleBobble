#pragma once
#pragma once
#include <array>
#include "Bubble.h"
#include "TileMap.h"
#include "ParticleManager.h"

#define MAX_BUBBLES		16

class BubbleManager
{
public:
	BubbleManager();
	~BubbleManager();

	AppStatus Initialise();

	//Set the TileMap reference for managing shot collisions
	void SetTileMap(TileMap* tilemap);

	//Set the TileMap reference for managing shot collisions
	void SetParticleManager(ParticleManager* particles);

	void SetEnemiesHitbox(std::vector<AABB> hitboxes);

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

	//Array of all the hitboxes of the enemies in the scene
	std::vector<AABB> enemies_hitbox;
};

