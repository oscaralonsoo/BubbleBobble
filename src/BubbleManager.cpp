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
void BubbleManager::SetEnemies(std::vector<Enemy*> enemies)
{
	this->enemies = enemies;
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
	if (!found) LOG("Not enough space to add a new bubble in the BubbleManager!");
}
void BubbleManager::Clear()
{
	for (Bubble& bubble : bubbles)
		bubble.SetAlive(false);
}
void BubbleManager::Update(const AABB& player_hitbox)
{
	AABB box;

	for (Bubble& bubble : bubbles)
	{
		if (bubble.IsAlive())
		{
			//Update position
			bubble.Update();

			//Check level collisions
			box = bubble.GetHitbox();

			//Check enemy collision
			for (Enemy* enemy : enemies)
			{
				if (box.TestAABB(enemy->GetHitbox()) && bubble.state == BubbleState::LAUNCHING)
				{
					bubble.SetPos(enemy->GetPos());
					enemy->SetAlive(false);
					bubble.StartHit();
				}
			}

			//Check wall collision
			if (bubble.IsMovingRight() && map->TestCollisionWallRight(box) || bubble.IsMovingLeft() && map->TestCollisionWallLeft(box))
			{
				if (bubble.GetAnimation() != BubbleAnim::HIT)
				{
					bubble.StartLevitating();
				}
			}

			//Check player collision
			if (box.TestAABB(player_hitbox) && bubble.state != BubbleState::LAUNCHING)
			{
				if (bubble.GetAnimation() == BubbleAnim::HIT)
				{
					for (Enemy* enemy : enemies)
					{
						if (!enemy->IsAlive())
						{
							enemy->SetAlive(true);
							enemy->SetPos(bubble.GetPos());
							enemy->KillEnemy();
							break;
						}
					}
				}
				
				bubble.SetAlive(false);
				bubble.StartLaunching();

				Point p;
				p.x = box.pos.x - (TILE_SIZE - BUBBLE_PHYSICAL_WIDTH) / 2;
				p.y = box.pos.y - (TILE_SIZE - BUBBLE_PHYSICAL_HEIGHT) / 2;
				particles->Add(p);
			}

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
