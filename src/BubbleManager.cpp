#include "BubbleManager.h"

BubbleManager::BubbleManager()
{
	map = nullptr;
	particles = nullptr;
}
BubbleManager::~BubbleManager()
{
}
AppStatus BubbleManager::Initialise()
{
	//Nothing needs to be done because the texture is located within the enemy sprites
	//and loaded by the EnemyManager

	return AppStatus::OK;
}
void BubbleManager::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
}
void BubbleManager::SetParticleManager(ParticleManager* particles)
{
	this->particles = particles;
}
void BubbleManager::Add(const Point& pos, const Point& dir)
{
	bool found = false;
	for (Bubble& bubble : bubbles)
	{
		if (!bubble.IsAlive())
		{
			bubble.Init(pos, dir);
			found = true;
			break;
		}
	}
	if (!found) LOG("Not enough space to add a new shot in the BubbleManager!");
}
void BubbleManager::Clear()
{
	for (Bubble& bubble : bubbles)
		bubble.SetAlive(false);
}
void BubbleManager::Update(const AABB& player_hitbox)
{
	AABB box;
	bool hit;

	for (Bubble& bubble : bubbles)
	{
		if (bubble.IsAlive())
		{
			//Update position
			bubble.Update();

			//Check level collisions
			box = bubble.GetHitbox();
			hit = false;
			if (bubble.IsMovingLeft())		  hit = map->TestCollisionWallLeft(box);
			else if (bubble.IsMovingRight()) hit = map->TestCollisionWallRight(box);

			//Check player collision
			if (!hit) hit = box.TestAABB(player_hitbox);

			/*if (hit)
			{
				bubble.SetAlive(false);
				Point p;
				p.x = box.pos.x - (TILE_SIZE - BUBBLE_PHYSICAL_WIDTH) / 2;
				p.y = box.pos.y - (TILE_SIZE - BUBBLE_PHYSICAL_HEIGHT) / 2;
				//particles->Add(p);
			}*/
		}
	}
}
void BubbleManager::Draw() const
{
	//Iterate over the shots vector by reference instead of by value to avoid a copy
	//of each Entity object
	for (const Bubble& bubble : bubbles)
		if (bubble.IsAlive())	bubble.Draw();
}
void BubbleManager::DrawDebug(const Color& col) const
{
	for (const Bubble& bubble : bubbles)
		if (bubble.IsAlive()) bubble.DrawHitbox(col);
}
