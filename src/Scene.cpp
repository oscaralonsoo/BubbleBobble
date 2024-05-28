#include "Scene.h"
#include <stdio.h>
#include "Globals.h"

Scene::Scene()
{
	player = nullptr;
	bubbles.clear();
	level = nullptr;
	enemies = nullptr;
	
	camera.target = { 0, 0 };				//Center of the screen
	camera.offset = { 0, MARGIN_GUI_Y };	//Offset from the target (center of the screen)
	camera.rotation = 0.0f;					//No rotation
	camera.zoom = 1.0f;						//Default zoom

	debug = DebugMode::OFF;
}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
	if (!bubbles.empty())
	{
		for (Bubble* bubble : bubbles) {
			bubble->Release();
			delete bubble;
		}
		bubbles.clear();
	}
	if (enemies != nullptr)
	{
		enemies->Release();
		delete enemies;
		enemies = nullptr;
	}
    if (level != nullptr)
    {
		level->Release();
        delete level;
        level = nullptr;
    }
	for (Entity* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}
AppStatus Scene::Init()
{
	//Create bubbles
	for (int i = 0; i < MAX_BUBBLES; i++)
	{
		bubbles.push_back(new Bubble(i, PLAYER_SPAWN, BubbleState::DISABLED, Direction::RIGHT));
	}
	if (bubbles.empty())
	{
		LOG("Failed to allocate memory for Bubble");
		return AppStatus::ERROR;
	}
	//Initialise bubbles
	for (Bubble* bubble : bubbles) {
		if (bubble->Initialise() != AppStatus::OK)
		{
			LOG("Failed to initialise Bubble");
			return AppStatus::ERROR;
		}
	}

	//Create player
	player = new Player(PLAYER_SPAWN, PlayerState::IDLE, Look::RIGHT, bubbles);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	//Initialise player
	if (player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}

	//Create enemy manager
	enemies = new EnemyManager();
	if (enemies == nullptr)
	{
		LOG("Failed to allocate memory for Enemy Manager");
		return AppStatus::ERROR;
	}
	//Initialise enemy manager
	if (enemies->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Enemy Manager");
		return AppStatus::ERROR;
	}

	//Create lifes
	/*for (int i = 0; i < MAX_BUBBLES; i++)
	{
		lifes.push_back(new Lifes(PLAYER_SPAWN));
	}
	if (lifes.empty())
	{
		LOG("Failed to allocate memory for Lifes");
		return AppStatus::ERROR;
	}
	//Initialise lifes
	for (Lifes* life : lifes) {
		if (life->Initialise() != AppStatus::OK)
		{
			LOG("Failed to initialise Lifes");
			return AppStatus::ERROR;
		}
	}
	*/
	//Create level 
    level = new TileMap();
    if (level == nullptr)
    {
        LOG("Failed to allocate memory for Level");
        return AppStatus::ERROR;
    }
	//Initialise level
	if (level->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}
	//Load level
	if (LoadLevel(1) != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}
	//Assign the tile map reference to the player to check collisions while navigating
	player->SetTileMap(level);

	//Assign the tile map reference to the enemies to check collisions while rooming
	enemies->SetTileMap(level);

	//Assign the tile map reference to the shot manager to check collisions when shots are shot
	//shots->SetTileMap(level);

	for (Bubble* bubble : bubbles) {
		bubble->SetTileMap(level);
	}


    return AppStatus::OK;
}
AppStatus Scene::LoadLevel(int stage)
{
	int size;
	int x, y, i;
	Tile tile;
	Point pos;
	int *map = nullptr;
	Object* obj;
	AABB hitbox, area;
	
	ClearLevel();

	size = LEVEL_WIDTH * LEVEL_HEIGHT;
	if (stage == 1)
	{
		map = new int[size] {																																			 
			 2,   3,   1,   1,   1,   1,   1,   1,   1,   8,   0,   0,   0,   1,   1,   1,   1,   1,   1,   8,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   2,   3,
			 2,   3,   9,   5,   5,   5,   5,   5,   5,   7,   0,   0,   0,   6,   5,   5,   5,   5,   5,   7,   0,   0,   0,   6,   5,   5,   5,   5,   5,   5,   2,   3,
			 2,   3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   0,   0,   0,   102, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   102, 0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   8,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   8,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   9,   5,   5,   5,   5,   5,   5,   7,   0,   0,   0,   0,   0,   5,   5,   5,   5,   5,   5,   5,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   4,   0,   0,   0,   101, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   102, 0,   0,   0,   0,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   8,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   9,   5,   5,   5,   5,   5,   5,   5,   7,   0,   0,   0,   5,   5,   5,   5,   5,   5,   5,   5,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   8,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   4,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   6,   5,   5,   5,   5,   5,   5,   5,   5,   5,   7,   0,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   7,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   1,   1,   1,   1,   1,   8,   0,   1,   1,   1,   8,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   8,   0,   1,   1,   1,   1,   1,   2,   3,
			 2,   3,   9,   5,   5,   5,   5,   7,   0,   6,   5,   5,   7,   0,   0,   0,   0,   0,   0,   0,   6,   5,   5,   7,   0,   6,   5,   5,   5,   5,   2,   3,
			 2,	  3,   4,	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   4,   100, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,
			 2,   3,   1,   1,   1,   1,   1,   1,   1,   8,   0,   0,   0,   1,   1,   1,   1,   1,   1,   8,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   2,   3

		};
		player->InitScore();
	}
	else if (stage == 2)
	{
		map = new int[size] {
			13,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  13,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,  13,
			 11,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,   0,   0,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   9,
			10,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,   0,   0,   0,   0,   0,   0,   0,   0,  63,   0,   0,  10,
			 9,   0,   0,   0,  62,   0,   0,   0,   0,   0,   0,   0,   9,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,
			10,   0,   0,  62,   0,   0,   0,   0,   0,   0,   0,   0,  10,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,
			 9,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,  22,  23,  13,  17,  18,  17,  18,  17,  18,  13,   0,   9,
			10,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,  20,  21,   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,
			 9,   0,  13,  17,  18,  17,  18,  17,  18,  13,  22,  23,  13,  20,  21,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,
			10,   0,   0,   0,   0,   0,   0,   0,   0,   0,  20,  21,   0,  20,  21,   0,   0,   0,   0,   0,   0,   0,   0,  13,  10,
			 9,   0,   0,   0,   0,   0,   0,   0,   0,   0,  20,  21,   0,  20,  21,   0,   0,   0,   0,   0,   0,	  0,   0,   0,   9,
			10,  13,   0,   0,   0,  62,  62,   0,   0,   0,  20,  21,   0,  20,  21,   0,   0,   0,  63,   0,   0,   0,   0,   0,  10,
			 9,   0,   0,   0,   0,   0,   0,   0,   0,   0,  20,  21,   0,  20,  21,   0,   0,  63,   0,  63,   0,   0,   0,  13,   9,
			10,   0,   0,  -1,   0,   0,   0,   0,   0,   0,  20,  21,   0,  20,  21,   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,
			 9,  13,   0, 100,   0,   0,   0,   0,   0,   0,  20,  21,   0,  20,  21,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,
			10,  17,  18,  13,   0,   0,   0,   0,  13,  17,  18,  17,  18,  17,  18,  17,  13,   0,   0,   0,   0,	 13,  17,  18,  10,
			 9,   1,   2,  40,  70,  70,  70,  70,  41,   9,   5,   6,   9,   5,   6,   9,  40,  70,  70,  70,  70,  41,   1,   2,   9,
			10,   3,   4,  13,  14,  15,  16,  11,  12,  10,   7,   8,  10,   7,   8,  10,  13,  14,  15,  16,  11,  12,   3,   4,   10
		};
	}
	else
	{
		//Error level doesn't exist or incorrect level number
		LOG("Failed to load level, stage %d doesn't exist", stage);
		return AppStatus::ERROR;	
	}

	//Tile map
	level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT);

	//Entities and objects
	i = 0;
	for (y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (x = 0; x < LEVEL_WIDTH; ++x)
		{
			tile = (Tile)map[i];
			if (level->IsTileEntity(tile) || level->IsTileObject(tile))
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;

				if (tile == Tile::PLAYER)
				{
					player->SetPos(pos);
				}
				/*else if (tile == Tile::ITEM_APPLE)
				{
					obj = new Object(pos, ObjectType::APPLE);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_CHILI)
				{
					obj = new Object(pos, ObjectType::CHILI);
					objects.push_back(obj);
				}*/
				else if (tile == Tile::ZENCHAN)
				{
					pos.x += (ZENCHAN_FRAME_SIZE - ZENCHAN_PHYSICAL_WIDTH) / 2;
					hitbox = enemies->GetEnemyHitBox(pos, EnemyType::ZENCHAN);
					area = level->GetSweptAreaX(hitbox);
					enemies->Add(pos, EnemyType::ZENCHAN, area);

				}
				/*else if (tile == Tile::TURRET_LEFT)
				{
					hitbox = enemies->GetEnemyHitBox(pos, EnemyType::TURRET);
					area = level->GetSweptAreaX(hitbox);
					enemies->Add(pos, EnemyType::TURRET, area, Look::LEFT);
				}
				else if (tile == Tile::TURRET_RIGHT)
				{
					hitbox = enemies->GetEnemyHitBox(pos, EnemyType::TURRET);
					area = level->GetSweptAreaX(hitbox);
					enemies->Add(pos, EnemyType::TURRET, area, Look::RIGHT);
				}*/
				else
				{
					LOG("Internal error loading scene: invalid entity or object tile id")
				}
			}
			++i;
		}
	}
	//Remove initial positions of objects and entities from the map
	level->ClearObjectEntityPositions();

	delete[] map;
	return AppStatus::OK;
}
void Scene::Update()
{
	Point p1, p2;
	AABB hitbox;

	//Switch between the different debug modes: off, on (sprites & hitboxes), on (hitboxes) 
	if (IsKeyPressed(KEY_F1))
	{
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	}
	//Debug levels instantly
	if (IsKeyPressed(KEY_ONE))		LoadLevel(1);
	else if (IsKeyPressed(KEY_TWO))	LoadLevel(2);

	level->Update();
	player->Update();
	CheckObjectCollisions();

	hitbox = player->GetHitbox();
	enemies->Update(hitbox);
	for (Bubble* bubble : bubbles) {
		bubble->Update();
	}
}
void Scene::Render()
{
	BeginMode2D(camera);

    level->Render();
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		RenderObjects(); 
		enemies->Draw();
		player->Draw();
		for (Bubble* bubble : bubbles) {
			bubble->Draw();
		}
	}
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		RenderObjectsDebug(YELLOW);
		player->DrawDebug(GREEN);
		enemies->DrawDebug();
		for (Bubble* bubble : bubbles) {
			bubble->DrawDebug(BLUE);
		}
	}

	EndMode2D();

	RenderGUI();
}
void Scene::Release()
{
    level->Release();
	player->Release();

	for (Bubble* bubble : bubbles) {
		bubble->Release();
	}
	ClearLevel();
}
void Scene::CheckObjectCollisions()
{
	AABB player_box, obj_box;

	player_box = player->GetHitbox();
	auto it = objects.begin();
	while (it != objects.end())
	{
		obj_box = (*it)->GetHitbox();
		if (player_box.TestAABB(obj_box))
		{
			player->IncrScore((*it)->Points());

			//Delete the object
			delete* it;
			//Erase the object from the vector and get the iterator to the next valid element
			it = objects.erase(it);
		}
		else
		{
			//Move to the next object
			++it;
		}
	}
}
void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();

	enemies->Release();
}
void Scene::RenderObjects() const
{
	for (Object* obj : objects)
	{
		obj->Draw();
	}
}
void Scene::RenderObjectsDebug(const Color& col) const
{
	for (Object* obj : objects)
	{
		obj->DrawDebug(col);
	}
}
void Scene::RenderGUI() const
{
	//Temporal approach
	DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);

	DrawText(TextFormat("LIFES : %d", player->GetLifes()), 410, 5, 20, LIGHTGRAY);

	/*for (Lifes* life : lifes) {
		life->Draw();
	}*/
}