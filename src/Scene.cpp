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
	current_stage = 0;

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

	for (Lifes* life : lifes)
	{
		delete life;
	}
	lifes.clear();
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

	//Create static lifes
	lifes.push_back(new Lifes({ 0,  624 }));
	lifes.push_back(new Lifes({ 24, 624 }));
	lifes.push_back(new Lifes({ 48, 624 }));
	lifes.push_back(new Lifes({ 72, 624 }));
	lifes.push_back(new Lifes({ 96, 624 }));

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
	
	current_stage = stage;

	ClearLevel();

	size = LEVEL_WIDTH * LEVEL_HEIGHT;
	if (stage == 1)
	{
		map = new int[size] {																																			 
		2, 3,  1,   1,  1,  1,  1,  1,   1, 19,   0,  0,  0,  1,  1,  1,  1,  1,  1, 23,  0,   0,   0,  1,  1,  1,  1,  1,  1,  1, 2, 3,
		2, 3, 24,  20, 20, 20, 20, 20,  20, 22,   0,  0,  0, 21, 20, 20, 20, 20, 20, 22,  0,   0,   0, 21, 20, 20, 20, 20, 20, 20, 2, 3,
		2, 3, 19,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  0,  0,  0, 101,  0,  54,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0, 101,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1,  1,  1,   1,  1,   1,  1,  1, 23,  0,  0,  0,  0,  0,  1,  1,   1,   1,  1,  1,  1,  1, 23,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 24, 20,  20, 20,  20, 20, 20, 22,  0,  0,  0,  0,  0, 20, 20,  20,  20, 20, 20, 20,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 19,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 19,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 19,  0,   0,  0, 101,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 101,   0,  0,  53,  0,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1,  1,  1,   1,  1,   1,  1,  1,  1, 23,  0,  0,  0,  1,  1,  1,   1,   1,  1,  1,  1,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 24, 20,  20, 20,  20, 20, 20, 20, 22,  0,  0,  0, 20, 20, 20,  20,  20, 20, 20, 20,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 19,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 19,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1, 19,  0,   57,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  1,  1,  1,   1,  1,   1,  1,  1,  1,  1, 23,  0,  1,  1,  1,  1,   1,   1,  1,  1,  1,  1, 19,  0,  0, 2, 3,
		2, 3, 19,   0,  0, 21, 20, 20,  20, 20,  20, 20, 20, 20, 20, 22,  0, 20, 20, 20, 20,  20,  20, 20, 20, 20, 20, 22,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 19,  50,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0, 52,  0,  0, 2, 3,
		2, 3,  1,   1,  1,  1,  1, 23,   0,  1,   1,  1, 23,  0,  0,  0,  0,  0,  0,  0,  1,   1,   1, 23,  0,  1,  1,  1,  1,  1, 2, 3,
		2, 3, 24,  20, 20, 20, 20, 22,   0, 21,  20, 20, 22,  0,  0,  0,  0,  0,  0,  0, 21,  20,  20, 22,  0, 21, 20, 20, 20, 20, 2, 3,
		2, 3, 19,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 19,   0,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3, 19, 100,  0,  0,  0,  0,   0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,  0,  0,  0,  0,  0,  0, 2, 3,
		2, 3,  1,   1,  1,  1,  1,  1,   1, 23,   0,  0,  0,  1,  1,  1,  1,  1,  1, 23,  0,   0,   0,  1,  1,  1,  1,  1,  1,  1, 2, 3

		};
		player->InitScore();
	}
	else if (stage == 2)
	{
		map = new int[size] {
		 5,   6,    4,   4,   4,   4,   4,   4,   4,  27,   0,   0,   0,   4,   4,   4,   4,   4,   4,  27,   0,   0,   0,   4,   4,   4,   4,   4,   4,   4,   5,   6,
		 7,   8,   25,  28,  28,  28,  28,  28,  28,  26,   0,   0,   0,  30,  28,  28,  28,  28,  28,  26,   0,   0,   0,  30,  28,  28,  28,  28,  28,  28,   7,   8,
		 5,   6,   27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,
		 7,   8,   27,   0,   0, 101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 101,   0,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   4,   4,   4,   4,   4,   4,   4,   4,   4,  29,   0,   0,   0,   4,   4,   4,   4,   4,   4,   4,   4,   4,  29,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   4,  25,  28,  28,  28,  28,  28,   4,   4,  27,   0,   0,   0,   4,   4,  25,  28,  28,  28,  28,  28,   4,  27,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   4,  27,   0,   0,   0,   0,   0,   4,   4,  27,   0,   0,   0,   4,   4,  27,   0,   0,   0,   0,   0,   4,  27,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   4,  27, 101,   0,   0,   0,   0,   4,   4,  27,   0,   0,   0,   4,   4,  27,   0,   0,   0, 101,   0,   4,  27,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   4,   4,   4,   4,  29,   0,   0,   4,   4,  27,   0,   0,   0,   4,   4,  27,   0,   0,   4,   4,   4,   4,  27,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,  30,  28,  28,  28,  26,   0,   0,   4,   4,  27,   0,   0,   0,   4,   4,  27,   0,   0,  30,  28,  28,  28,  26,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,  27,   0,   0,   0,   4,   4,  27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,  27,   0,   0,   0,   4,   4,  27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   4,  27,   0,   0,   0,   4,   4,  27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   4,   4,   4,   4,  29,   0,   0,  30,  28,  26,   0,   0,   0,  30,  28,  26,   0,   0,   4,   4,   4,   4,  29,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   4,  25,  28,  28,  26,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  30,  28,  28,   4,  27,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   4,  27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,  27,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   4,  27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,  27,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   4,  27, 101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 101,   0,   4,  27,   0,   0,   7,   8,
		 5,   6,   27,   0,   0,   4,   4,   4,   4,   4,   4,   4,  29,   0,   0,   4,   4,  29,   0,   0,   4,   4,   4,   4,   4,   4,   4,  27,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,  30,  28,  28,  28,  28,  28,  28,  26,   0,   0,  30,  28,  26,   0,   0,  30,  28,  28,  28,  28,  28,  28,  26,   0,   0,   7,   8,
		 5,	  6,   27,	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,
		 7,   8,   27,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,   8,
		 5,   6,   27, 100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,   6,
		 7,   8,    4,   4,   4,   4,   4,   4,   4,  29,   0,   0,   0,   4,   4,   4,   4,   4,   4,  29,   0,   0,   0,   4,   4,   4,   4,   4,   4,   4,   7,   8

		};
	}
	else if (stage == 3)
	{
		map = new int[size] {
		 10,   11,    9,   9,   9,   9,   9,   9,   9,  34,   0,   0,   0,   9,   9,   9,   9,   9,   9,  34,   0,   0,   0,   9,   9,   9,   9,   9,   9,   9,   10,   11,
		 12,   13,   33,  31,  31,  31,  31,  31,  31,  35,   0,   0,   0,  36,  31,  31,  31,  31,  31,  35,   0,   0,   0,  36,  31,  31,  31,  31,  31,  31,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 102,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,   0,   0,   0,   9,   9,   9,  32,   0,   0,   0,   0,   0,   9,   9,   9,  32,   0,   0,   0,   0,   0,   0,   0,   10,   11,
		 12,   13,   34,   0, 102,   0,   0,   0,   9,   9,   9,  33,   9,   9,  32,   0,   0,   0,   9,   9,  33,   9,   9,   9,  32,   0,   0,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,   9,   9,  33,  31,  35,  36,   9,   9,  32,   0,   9,   9,  33,  35,  36,  31,   9,   9,  32,   0,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   0,   9,  33,  35,   0,   0,   0,  36,   9,   9,  32,   9,  33,  35,   0,   0,   0,  36,   9,  34,   0,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   9,   9,  34,   0,   0,   0,   0,   0,  36,   9,  34,  36,  35,   0,   0,   0,   0,   0,   9,   9,  32,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   9,  33,  35,   0,   0,   0,   0,   0,   0,  36,  35,   0,   0,   0,   0,   0, 102,   0,  36,   9,  34,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   9,  34,   0,   0,   0,   0, 102,   0,   0, 102,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,  34,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   9,   9,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,   9,  34,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,  36,   9,  34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 102,   0,   0,   9,  33,  35,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   0,   9,   9,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,   9,  34,   0,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,  36,   9,  34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,  33,  35,   0,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   0,   0,   9,   9,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,   9,  34,   0,   0,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,   0,  36,   9,   9,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,   9,   9,  33,  35,   0,   0,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   0,   0,   0,  36,   9,   9,  32,   0,   0,   0, 102,   0,   0,   0,   9,   9,  33,  35,   0,   0,   0,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,   0,   0,   0,  36,   9,   9,  32,   0,   0,   0,   0,   0,   9,   9,  33,  35,   0,   0,   0,   0,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   0,   0,   0,   0,   0,  36,   9,   9,  32,   0,   0,   0,   9,   9,  33,  35,   0,   0,   0,   0,   0,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,   9,   9,  32,   0,   0,   9,  33,  35,   0,   0,   0,   0,   0,   0,   0,   0,   0,   10,   11,
		 12,   13,   34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,   9,   9,  32,   9,  33,  35,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   12,   13,
		 10,   11,   34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,   9,  34,  36,  35,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   10,   11,  
		 12,   13,   34,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,  35,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   12,   13,
		 10,   11,   34, 100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   10,   11,
		 12,   13,    9,   9,   9,   9,   9,   9,   9,  32,   0,   0,   0,   9,   9,   9,   9,   9,   9,  32,   0,   0,   0,   9,   9,   9,   9,   9,   9,   9,   12,   13
		};
	}
	else if (stage == 4)
	{
		map = new int[size] {
		 15,   16,  14,  14,  14,  14,  14,  14,  14,  40,   0,   0,   0,  14,  14,  14,  14,  14,  14,  40,   0,   0,   0,  14,  14,  14,  14,  14,  14,  14,   15,   16,
		 17,   18,  39,  37,  37,  37,  37,  37,  37,  41,   0,   0,   0,  42,  37,  37,  37,  37,  37,  41,   0,   0,   0,  42,  37,  37,  37,  37,  37,  37,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 102,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,   0,   0, 102,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 102,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,  14,  14,  14,  14,  38,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0, 102,   0,   0,   0,   0,   0,  14,  14,  14,  39,  37,  14,  14,  14,  38,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,   0,  14,  14,  39,  37,  14,  14,  14,  14,  14,  14,  38,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,  14,  39,  41,  14,  14,  14,  14,  39,  37,  14,  14,  38,   0,   0,   0,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,  14,  14,  40,  14,  14,  14,  14,  39,  14,  38,  42,  14,  14,  38,   0,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,  14,  14,  14,  14,  14,  14,  14,  40,  14,  40,   0,  14,  14,  40,   0,   0,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,  14,  14,  14,  14,  14,  14,  14,  14,  42,  41,  14,  14,  14,  14,  38,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,  14,  39,  37,  37,  37,  14,  14,  14,  14,  14,  14,  14,  14,  14,  40,   0,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,  42,  41,   0,   0,   0,  42,  14,  14,  14,  14,  14,  14,  14,  14,  40,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  42,  37,  14,  14,  14,  14,  14,  14,  40,   0,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,   0,  14,  14,  14,  14,   0,   0,   0,  42,  37,  14,  14,  14,  14,  14,  38,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,  42,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  38,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,   0,   0,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  14,  40,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,  14,  14,  14,  14,  39,  37,  14,  14,  14,  14,  39,  37,  37,  37,  37,  41,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   0,   0,   0,   0,   0,   0,  42,  37,  37,  37,  41,   0,  42,  37,  37,  37,  41,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 102,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  40,   0,   0,   0,   0,   0,   0,  14,  14,  38,   0,   0,   0,   0,   0,   0,   0,   0,   0,  14,  14,  38,   0,   0,   0,   0,   0,   0,   17,   18,
		 15,   16,  40,   100, 0,   0,   0,   0,   0,  42,  37,  41,   0,   0,   0,   0,   0,   0,   0,   0,   0,  42,  37,  41,   0,   0,   0,   0,   0,   0,   15,   16,
		 17,   18,  14,  14,  14,  14,  14,  14,  14,  38,   0,   0,   0,  14,  14,  14,  14,  14,  14,  38,   0,   0,   0,  14,  14,  14,  14,  14,  14,  14,   17,   18
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
				else if (tile == Tile::ITEM_BANANA)
				{
					obj = new Object(pos, ObjectType::BANANA);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_APPLE)
				{
					obj = new Object(pos, ObjectType::APPLE);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_CHERRY)
				{
					obj = new Object(pos, ObjectType::CHERRY);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_LEMON)
				{
					obj = new Object(pos, ObjectType::LEMON);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_PEAR)
				{
					obj = new Object(pos, ObjectType::PEAR);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_WATERMELON)
				{
					obj = new Object(pos, ObjectType::WATERMELON);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_PINEAPPLE) 
				{
					obj = new Object(pos, ObjectType::PINEAPPLE);
					objects.push_back(obj);
				}
				else if (tile == Tile::ITEM_STRAWBERRY)
				{
					obj = new Object(pos, ObjectType::STRAWBERRY);
					objects.push_back(obj);
				}
				else if (tile == Tile::ZENCHAN)
				{
					pos.x += (ZENCHAN_FRAME_SIZE - ZENCHAN_PHYSICAL_WIDTH) / 2;
					enemies->Add(pos, EnemyType::ZENCHAN);

					obj = new Object({0, 0}, (ObjectType)GetRandomValue(0, 7));
					obj->SetAlive(false);
					objects.push_back(obj);
				}
				else if (tile == Tile::MONSTA)
				{
					pos.x += (MONSTA_FRAME_SIZE - MONSTA_PHYSICAL_WIDTH) / 2;
					enemies->Add(pos, EnemyType::MONSTA);

					obj = new Object({ 0, 0 }, (ObjectType)GetRandomValue(0, 7));
					obj->SetAlive(false);
					objects.push_back(obj);
				}
				else
				{
					LOG("Internal error loading scene: invalid entity or object tile id");
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

	//Pass to the next level
	if (objects.empty())
	{
		LoadLevel(++current_stage);
	}

	level->Update();
	player->Update();

	CheckObjectCollisions();
	CheckEnemiesCollisions();

	hitbox = player->GetHitbox();
	enemies->Update(objects);
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
bool Scene::Win()
{
	return current_stage > 4;
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

	lifes[player->GetLifes()]->SetAlive(false);
}
void Scene::CheckEnemiesCollisions()
{
	player->SetEnemies(enemies->GetEnemies());
	bubbles->SetEnemies(enemies->GetEnemies());
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
		if (obj->IsAlive())
		{
			obj->Draw();
		}
	}
	for (Lifes* life : lifes)
	{
		if (life->IsAlive())
		{
			life->Draw();
		}
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
	font1->Draw(600, 25, TextFormat("SCORE:"), WHITE);
	font1->Draw(310, 20, TextFormat("HIGH SCORE"), RED);
	



 	//Temporal approach
	/*DrawText(TextFormat("SCORE : %d", player->GetScore()), 10, 10, 8, LIGHTGRAY);

	DrawText(TextFormat("LIFES : %d", player->GetLifes()), 410, 5, 20, LIGHTGRAY);*/

	/*for (Lifes* life : lifes) {
		life->Draw();
	}*/
}