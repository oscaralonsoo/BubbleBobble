#include "Scene.h"
#include <stdio.h>
#include "Globals.h"

Scene::Scene()
{
	player = nullptr;
	level = nullptr;
	enemies = nullptr;
	bubbles = nullptr;
	particles = nullptr;
	font1 = nullptr;
	
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
	if (enemies != nullptr)
	{
		enemies->Release();
		delete enemies;
		enemies = nullptr;
	}
	if (bubbles != nullptr)
	{
		delete bubbles;
		bubbles = nullptr;
	}
	if (particles != nullptr)
	{
		delete particles;
		particles = nullptr;
	}
    if (level != nullptr)
    {
		level->Release();
        delete level;
        level = nullptr;
    }
	if (font1 != nullptr)
	{
		delete font1;
		font1 = nullptr;
	}
	for (Entity* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}
AppStatus Scene::Init()
{
	//Create player
	player = new Player(PLAYER_SPAWN, PlayerState::IDLE, Look::RIGHT);
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

	//Create bubble manager 
	bubbles = new BubbleManager();
	if (bubbles == nullptr)
	{
		LOG("Failed to allocate memory for Shot Manager");
		return AppStatus::ERROR;
	}
	//Initialise bubble manager
	if (bubbles->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Shot Manager");
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
	//Create particle manager 
	particles = new ParticleManager();
	if (particles == nullptr)
	{
		LOG("Failed to allocate memory for Particle Manager");
		return AppStatus::ERROR;
	}
	//Initialise particle manager
	if (particles->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Particle Manager");
		return AppStatus::ERROR;
	}

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

	//Assign the tile map reference to the shot manager to check collisions when bubbles are shot
	bubbles->SetTileMap(level);

	//Assign the particle manager reference to the shot manager to add particles when shots collide
	bubbles->SetParticleManager(particles);

	//Assign the shot manager reference to the enemy manager so enemies can add bubbles
	player->SetShotManager(bubbles);

	bubbles->SetEnemies(enemies->GetEnemies());

	font1 = new Text();
	if (font1 == nullptr)
	{
		LOG("Failed to allocate memory for font 1");
		return AppStatus::ERROR;
	}
	//Initialise text font 1
	if (font1->Initialise(Resource::IMG_TEXT, "images/text.png", ' ', 16) != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
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
	AABB hitbox;
	
	ClearLevel();

	size = LEVEL_WIDTH * LEVEL_HEIGHT;
	if (stage == 1)
	{
		map = new int[size] {																																			 
		2, 3,  1,   1,  1,  1,  1,  1,   1, 15,   0,  0,  0,  1,  1,  1,  1,  1,  1, 19,  0,   0,   0,  1,  1,  1,  1,  1,  1,  1, 2, 3,
		2, 3, 20,  16, 16, 16, 16, 16,  16, 18,   0,  0,  0, 17, 16, 16, 16, 16, 16, 18,  0,   0,   0, 17, 16, 16, 16, 16, 16, 16, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0, 101,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0, 101,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1,  1,  1,   1,  1,   1,  1,  1, 19,  0,  0,  0,  0,  0,  1,  1,   1,   1,  1,  1,  1,  1, 19,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 20, 16,  16, 16,  16, 16, 16, 18,  0,  0,  0,  0,  0, 16, 16,  16,  16, 16, 16, 16,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 15,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 15,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 15,  0,   0,  0, 101,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 101,   0,  0,  0,  0,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1,  1,  1,   1,  1,   1,  1,  1,  1, 19,  0,  0,  0,  1,  1,  1,   1,   1,  1,  1,  1,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 20, 16,  16, 16,  16, 16, 16, 16, 18,  0,  0,  0, 16, 16, 16,  16,  16, 16, 16, 16,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 15,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 15,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1, 15,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  1,  1,  1,   1,  1,   1,  1,  1,  1,  1, 19,  0,  1,  1,  1,  1,   1,   1,  1,  1,  1,  1, 15,  0,  0, 2, 3,
		2, 3, 15,   0,  0, 17, 16, 16,  16, 16,  16, 16, 16, 16, 16, 18,  0, 16, 16, 16, 16,  16,  16, 16, 16, 16, 16, 18,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3,  1,   1,  1,  1,  1, 19,   0,  1,   1,  1, 19,  0,  0,  0,  0,  0,  0,  0,  1,   1,   1, 19,  0,  1,  1,  1,  1,  1, 2, 3,
		2, 3, 20,  16, 16, 16, 16, 18,   0, 17,  16, 16, 18,  0,  0,  0,  0,  0,  0,  0, 17,  16,  16, 18,  0, 17, 16, 16, 16, 16, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 15,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 15, 100,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3,  1,   1,  1,  1,  1,  1,   1, 19,   0,  0,  0,  1,  1,  1,  1,  1,  1, 19,  0,   0,   0,  1,  1,  1,  1,  1,  1,  1, 2, 3

		};
		player->InitScore();
	}
	else if (stage == 2)
	{
		map = new int[size] {
		 4,   4,   4,   4,     4,    4,    4,    4,    4,    4,    4,    4,    4,    4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
		 4,   4,  21,  24,   24,   24,  24,   24,   24,   24,   24,   24,   24,   24,  24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,   4,   4,    25,   0,   0,    4,    4,    4,    4,    4,    4,   4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    25,   0,    0,   4,   4,   4,   4,
		 4,   4,  21,  24,   22,   0,   0,   26,   24,   24,   24,   24,   24,  24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   22,   0,   0,   26,   24,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,   4,   4,   25,   0,   0,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   25,   0,   0,   4,   4,   4,   4,
		 4,   4,  21,  24,   22,   0,   0,   26,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   22,   0,   0,   26,   24,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,   4,   4,   25,   0,   0,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   25,   0,   0,   4,   4,   4,   4,
		 4,   4,  21,  24,   22,   0,   0,   26,   24,   24,   24,   24,   24,   24,   24,  24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   22,   0,   0,   26,   24,   4,   4,
		 4,	  4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,  23, 100, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,
		 4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4

		};
	}
	else if (stage == 3)
	{
		map = new int[size] {
		6,   7,    5,   5,   5,   5,   5,   5,   5,  30,   0,   0,   0,   5,   5,   5,   5,   5,   5,  30,   0,   0,   0,   5,   5,   5,   5,   5,   5,   5,   6,   7,
		 8,   9,   29,  27,  27,  27,  27,  27,  27,  31,   0,   0,   0,  32,  27,  27,  27,  27,  27,  31,   0,   0,   0,  32,  27,  27,  27,  27,  27,  27,   8,   9,
		 6,   7,   30,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   0,   0,   0,   0,   5,   5,   5,  28,   0,   0,   0,   0,   0,   5,   5,   5,  28,   0,   0,   0,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   0,   5,   5,   5,  29,   5,   5,  28,   0,   0,   0,   5,   5,  29,   5,   5,   5,  28,   0,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   0,   5,   5,  29,  27,  31,  32,   5,   5,  28,   0,   5,   5,  29,  31,  32,  27,   5,   5,  28,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   5,  29,  31,   0,   0,   0,  32,   5,   5,  28,   5,  29,  31,   0,   0,   0,  32,   5,  30,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   5,   5,  30,   0,   0,   0,   0,   0,  32,   5,  30,  32,  31,   0,   0,   0,   0,   0,   5,   5,  28,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   5,  29,  31,   0,   0,   0,   0,   0,   0,  32,  31,   0,   0,   0,   0,   0,   0,   0,  32,   5,  30,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   5,  30,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  30,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   5,   5,  28,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   5,  30,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,  32,   5,  30,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  29,  31,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   5,   5,  28,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   5,  30,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   0,  32,   5,  30,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  29,  31,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   0,   5,   5,  28,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   5,  30,   0,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   0,   0,  32,   5,   5,  28,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   5,  29,  31,   0,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   0,   0,  32,   5,   5,  28,   0,   0,   0,   0,   0,   0,   0,   5,   5,  29,  31,   0,   0,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   0,   0,   0,   0,  32,   5,   5,  28,   0,   0,   0,   0,   0,   5,   5,  29,  31,   0,   0,   0,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   0,   0,   0,   0,  32,   5,   5,  28,   0,   0,   0,   5,   5,  29,  31,   0,   0,   0,   0,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   0,   0,   0,   0,   0,   0,   0,   0,   0,  32,   5,   5,  28,   0,   0,   5,  29,  31,   0,   0,   0,   0,   0,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  32,   5,   5,  28,   5,  29,  31,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   8,   9,
		 6,	  7,   30,	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  32,   5,  30,  32,  31,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   6,   7,
		 8,   9,   30,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  32,  31,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   8,   9,
		 6,   7,   30,   100, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   6,   7,
		 8,   9,   5,   5,   5,   5,   5,   5,   5,   28,   0,   0,   0,   5,   5,   5,   5,   5,   5,  28,   0,   0,   0,   5,   5,   5,   5,   5,   5,   5,   8,   9
		};
	}
	else if (stage == 4)
	{
		map = new int[size] {
		 11,   12,  10,  10,  10,  10,  10,  10,  10,  36,   0,   0,   0,  10,  10,  10,  10,  10,  10,  36,   0,   0,   0,  10,  10,  10,  10,  10,  10,  10,   11,   12,
		 13,   14,  35,  33,  33,  33,  33,  33,  33,  37,   0,   0,   0,  38,  33,  33,  33,  33,  33,  37,   0,   0,   0,  38,  33,  33,  33,  33,  33,  33,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,  10,  10,  10,  34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,   0,  10,  10,  10,  35,  33,  10,  10,  10,  34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,   0,  10,  10,  35,  33,  10,  10,  10,  10,  10,  10,  34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,  10,  35,  37,  10,  10,  10,  10,  35,  33,  10,  10,  34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,  10,  10,  36,  10,  10,  10,  10,  35,  10,  34,  38,  10,  10,  34,   0,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,  10,  10,  10,  10,  10,  10,  10,  36,  10,  36,   0,  10,  10,  36,   0,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,  10,  10,  10,  10,  10,  10,  10,  10,  38,  37,  10,  10,  10,  10,  34,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,  10,  35,  33,  33,  33,  10,  10,  10,  10,  10,  10,  10,  10,  10,  36,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,  38,  37,   0,   0,   0,  38,  10,  10,  10,  10,  10,  10,  10,  10,  36,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  38,  33,  10,  10,  10,  10,  10,  10,  36,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,   0,  10,  10,  10,  10,   0,   0,   0,  38,  33,  10,  10,  10,  10,  10,  34,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,  38,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  34,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,   0,   0,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  36,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,  10,  10,  10,  10,  35,  33,  10,  10,  10,  10,  35,  33,  33,  33,  33,  37,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   0,   0,   0,   0,   0,   0,  38,  33,  33,  33,  37,   0,  38,  33,  33,  33,  37,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  36,   0,   0,   0,   0,   0,   0,  10,  10,  34,   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,  10,  34,   0,   0,   0,   0,   0,   0,   13,   14,
		 11,   12,  36,   100, 0,   0,   0,   0,   0,  38,  33,  37,   0,   0,   0,   0,   0,   0,   0,   0,   0,  38,  33,  37,   0,   0,   0,   0,   0,   0,   11,   12,
		 13,   14,  10,  10,  10,  10,  10,  10,  10,  34,   0,   0,   0,  10,  10,  10,  10,  10,  10,  34,   0,   0,   0,  10,  10,  10,  10,  10,  10,  10,   13,   14
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
					enemies->Add(pos, EnemyType::ZENCHAN);

				}
				else if (tile == Tile::MONSTA)
				{
					pos.x += (MONSTA_FRAME_SIZE - MONSTA_PHYSICAL_WIDTH) / 2;
					hitbox = enemies->GetEnemyHitBox(pos, EnemyType::MONSTA);
					enemies->Add(pos, EnemyType::MONSTA);
				}
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
	if (IsKeyPressed(KEY_ONE))		    LoadLevel(1);
	else if (IsKeyPressed(KEY_TWO))	    LoadLevel(2);
	else if (IsKeyPressed(KEY_THREE))	LoadLevel(3);
	else if (IsKeyPressed(KEY_FOUR))	LoadLevel(4);

	level->Update();
	player->Update();

	CheckObjectCollisions();
	CheckEnemiesCollisions();

	hitbox = player->GetHitbox();
	enemies->Update(hitbox);
	bubbles->Update(hitbox);
	particles->Update();
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
		bubbles->Draw();
	}
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		RenderObjectsDebug(YELLOW);
		player->DrawDebug(GREEN);
		enemies->DrawDebug();
		bubbles->DrawDebug(GRAY);
	}
	particles->Draw();

	EndMode2D();

	RenderGUI();
}
void Scene::Release()
{
    level->Release();
	player->Release();

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
void Scene::CheckEnemiesCollisions()
{
	player->SetEnemiesHitbox(enemies->GetHitBoxes());
}
void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	enemies->Release();
	objects.clear();
	bubbles->Clear();
	particles->Clear();
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

	static int frame;
	frame++;
	frame %= 1000;

	font1->Draw(80, 10, TextFormat("1UP"), GREEN);
	font1->Draw(80, 25, TextFormat("SCORE:%d", player->GetScore()), WHITE);
	font1->Draw(600, 10, TextFormat("2UP"), BLUE);
	font1->Draw(600, 25, TextFormat("SCORE:%d", player->GetScore()), WHITE);
	font1->Draw(310, 20, TextFormat("HIGH SCORE"), RED);
	



 	//Temporal approach
	/*DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);

	DrawText(TextFormat("LIFES : %d", player->GetLifes()), 410, 5, 20, LIGHTGRAY);*/

	/*for (Lifes* life : lifes) {
		life->Draw();
	}*/
}