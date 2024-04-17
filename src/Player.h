#pragma once
#include "Entity.h"
#include "TileMap.h"

//Representation model size: 48x48
#define PLAYER_FRAME_SIZE		48

//Logical model size: 12x28
#define PLAYER_PHYSICAL_WIDTH	18
#define PLAYER_PHYSICAL_HEIGHT	42

//Horizontal speed and vertical speed while falling down
#define PLAYER_SPEED			2.6

//When jumping, initial jump speed and maximum falling speed
#define PLAYER_JUMP_FORCE		16

//Frame delay for updating the jump velocity
#define PLAYER_JUMP_DELAY		2

//Player is levitating when abs(speed) <= this value
#define PLAYER_LEVITATING_SPEED	4

//Gravity affects jumping velocity when jump_delay is 0
#define GRAVITY_FORCE			1

//Player animations delay
#define ANIM_IDLE_DELAY			18
#define ANIM_WALKING_DELAY		8
#define ANIM_JUMPING_DELAY		5
#define ANIM_FALLING_DELAY		12
#define ANIM_SHOOTING_DELAY		3

//Logic states
enum class State { IDLE, WALKING, JUMPING, FALLING, SHOOTING, DEAD };
enum class Look { RIGHT, LEFT };

//Rendering states
enum class PlayerAnim {
	IDLE_LEFT, IDLE_RIGHT,
	WALKING_LEFT, WALKING_RIGHT,
	JUMPING_LEFT, JUMPING_RIGHT,
	FALLING_LEFT, FALLING_RIGHT,
	SHOOTING_LEFT, SHOOTING_RIGHT,
	SHOCK_LEFT, SHOCK_RIGHT,
	TELEPORT_LEFT, TELEPORT_RIGHT,
	NUM_ANIMATIONS
};

class Player: public Entity
{
public:
	Player(const Point& p, State s, Look view);
	~Player();
	
	AppStatus Initialise();
	void SetTileMap(TileMap* tilemap);

	void InitScore();
	void IncrScore(int n);
	int GetScore();

	void Update();
	void DrawDebug(const Color& col) const;
	void Release();

private:
	bool IsLookingRight() const;
	bool IsLookingLeft() const;

	//Player mechanics
	void MoveX();
	void MoveY();
	void Shoot();
	void LogicJumping();
	void LogicShooting();

	//Animation management
	void SetAnimation(int id);
	PlayerAnim GetAnimation();
	void Stop();
	void StartWalkingLeft();
	void StartWalkingRight();
	void StartFalling();
	void StartJumping();
	void StartShooting();
	void ChangeAnimRight();
	void ChangeAnimLeft();

	//Jump steps
	bool IsAscending() const;
	bool IsDescending() const;

	State state;
	Look look;
	int jump_delay;

	TileMap *map;

	int score;
};

