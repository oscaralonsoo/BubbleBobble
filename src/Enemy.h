#pragma once
#include "Entity.h"
#include "TileMap.h"

//Representation model size: 48x48
#define ENEMY_FRAME_SIZE		48

//Logical model size: 12x28
#define ENEMY_PHYSICAL_WIDTH	35
#define ENEMY_PHYSICAL_HEIGHT	35

//Horizontal speed and vertical speed while falling down
#define ENEMY_SPEED			2.6

//Player spawn position 
#define ENEMY_SPAWN			{75, 595}

//Player animations delay
#define ANIM_IDLE_DELAY			18
#define ANIM_WALKING_DELAY		8
#define ANIM_JUMPING_DELAY		5
#define ANIM_FALLING_DELAY		12
#define ANIM_SHOOTING_DELAY		3

//Logic states
enum class EnemyState { IDLE, WALKING, DEAD };
enum class EnemyLook { RIGHT, LEFT };

//Rendering states
enum class EnemyAnim {
	IDLE_LEFT, IDLE_RIGHT,
	WALKING_LEFT, WALKING_RIGHT,
	NUM_ANIMATIONS
};

class Enemy : public Entity
{
public:
	Enemy(const Point& p, EnemyState s, EnemyLook view);
	~Enemy();

	AppStatus Initialise();
	void SetTileMap(TileMap* tilemap);

	void Update();
	void DrawDebug(const Color& col) const;
	void Release();

private:
	bool IsLookingRight() const;
	bool IsLookingLeft() const;

	//Player mechanics
	void MoveX();

	//Animation management
	void SetAnimation(int id);
	EnemyAnim GetAnimation();
	void Stop();
	void StartWalkingLeft();
	void StartWalkingRight();
	void ChangeAnimRight();
	void ChangeAnimLeft();

	EnemyState state;
	EnemyLook look;
	TileMap* map;
};

