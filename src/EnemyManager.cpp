#include "EnemyManager.h"
#include "ZenChan.h"
#include "Monsta.h"

EnemyManager::EnemyManager()
{
	map = nullptr;
}
EnemyManager::~EnemyManager()
{
	Release();
}
AppStatus EnemyManager::Initialise()
{
	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_ZENCHAN, "images/zenchan.png") != AppStatus::OK)
	{
		LOG("Failed to load enemies sprite texture");
		return AppStatus::ERROR;
	}
	if (data.LoadTexture(Resource::IMG_MONSTA, "images/monsta.png") != AppStatus::OK)
	{
		LOG("Failed to load enemies sprite texture");
		return AppStatus::ERROR;
	}
	return AppStatus::OK;
}
void EnemyManager::SetTileMap(TileMap* tilemap)
{
	this->map = tilemap;
}
void EnemyManager::Add(const Point& pos, EnemyType type, Look look)
{
	Enemy* enemy;

	if (type == EnemyType::ZENCHAN)
	{
		enemy = new ZenChan(pos, ZENCHAN_PHYSICAL_WIDTH, ZENCHAN_PHYSICAL_HEIGHT, ZENCHAN_FRAME_SIZE, ZENCHAN_FRAME_SIZE);
	}
	else if (type == EnemyType::MONSTA)
	{
		enemy = new Monsta(pos, MONSTA_PHYSICAL_WIDTH, MONSTA_PHYSICAL_HEIGHT, MONSTA_FRAME_SIZE, MONSTA_FRAME_SIZE);
	}
	else
	{
		LOG("Internal error: trying to add a new enemy with invalid type");
		return;
	}

	enemy->Initialise(look);
	enemies.push_back(enemy);
}
AABB EnemyManager::GetEnemyHitBox(const Point& pos, EnemyType type) const
{
	int width, height;
	if (type == EnemyType::ZENCHAN)
	{
		width = ZENCHAN_PHYSICAL_WIDTH;
		height = ZENCHAN_PHYSICAL_HEIGHT;
	}
	else if (type == EnemyType::MONSTA)
	{
		width = MONSTA_PHYSICAL_WIDTH;
		height = MONSTA_PHYSICAL_HEIGHT;
	}
	else
	{
		LOG("Internal error while computing hitbox for an invalid enemy type");
		return {};
	}
	Point p(pos.x, pos.y - (height - 1));
	AABB hitbox(p, width, height);
	return hitbox;
}
std::vector<Enemy*> EnemyManager::GetEnemies() const
{
	return enemies;
}
std::vector<AABB> EnemyManager::GetHitBoxes() const
{
	std::vector<AABB> hitboxes;

	for (Enemy* enemy : enemies)
	{
		hitboxes.push_back(enemy->GetHitbox());
	}

	return hitboxes;
}
void EnemyManager::Update(std::vector<Object*> objects)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(objects, map);
	}
}
void EnemyManager::Draw() const
{
	for (const Enemy* enemy : enemies)
		enemy->Draw();
}
void EnemyManager::DrawDebug() const
{
	for (const Enemy* enemy : enemies)
	{
		enemy->DrawVisibilityArea(DARKGRAY);
		enemy->DrawHitbox(RED);
	}
}
void EnemyManager::Release()
{
	for (Enemy* enemy : enemies)
		delete enemy;
	enemies.clear();
}