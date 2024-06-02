#pragma once
#include "Entity.h"
#include "TileMap.h"
#include "BubbleManager.h"

//Representation model size: 48x48
#define PLAYER_FRAME_SIZE		48

//Logical model size: 12x28
#define PLAYER_PHYSICAL_WIDTH	35
#define PLAYER_PHYSICAL_HEIGHT	35

//Horizontal speed and vertical speed while falling down
#define PLAYER_SPEED			3.1

//Speed of the bubble launched by the player
#define PLAYER_SHOOT_SPEED		7

//When jumping, initial jump speed and maximum falling speed
#define PLAYER_JUMP_FORCE		16

//Frame delay for updating the jump velocity
#define PLAYER_JUMP_DELAY		2

//Player is levitating when abs(speed) <= this value
#define PLAYER_LEVITATING_SPEED	0

//Gravity affects jumping velocity when jump_delay is 0
#define GRAVITY_FORCE			1

//Player spawn position 
#define PLAYER_SPAWN			{75, 595}

//Player animations delay
#define ANIM_IDLE_DELAY			16
#define ANIM_WALKING_DELAY		3
#define ANIM_JUMPING_DELAY		5
#define ANIM_FALLING_DELAY		10
#define ANIM_SHOOTING_DELAY		3
#define ANIM_DEAD_DELAY			7


//Logic states
enum class PlayerState { IDLE, WALKING, JUMPING, FALLING, SHOOTING, DEAD };

//Rendering states
enum class PlayerAnim {
	IDLE_LEFT, IDLE_RIGHT,
	WALKING_LEFT, WALKING_RIGHT,
	JUMPING_LEFT, JUMPING_RIGHT,
	FALLING_LEFT, FALLING_RIGHT,
	SHOOTING_LEFT, SHOOTING_RIGHT,
	DEAD_LEFT, DEAD_RIGHT,
	NUM_ANIMATIONS
};

class Player: public Entity
{
public:
	Player(const Point& p, PlayerState s, Look view);
	~Player();
	
	AppStatus Initialise();

	//Set the ShotManager reference for managing enemy shots
	void SetShotManager(BubbleManager* bubbles);

	//Set the enemies for interactions
	void SetEnemies(std::vector<Enemy*> enemies);

	void SetTileMap(TileMap* tilemap);

	void InitScore();
	void IncrScore(int n);
	int GetScore();
	void InitLifes();
	void DecrLifes();
	int GetLifes();

	void Update();
	void DrawDebug(const Color& col) const;
	void Release();

	void StartDeath();
private:
	bool IsLookingRight() const;
	bool IsLookingLeft() const;

	//Player mechanics
	void MoveX();
	void MoveY();
	void Shoot();
	void LogicJumping();
	void LogicShooting();
	void LogicDead();

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

	//Reference to the ShotManager object
	//This class does not own the object, it only holds a reference to it
	BubbleManager* bubbles;

	//Array of all the enemies in the scene
	std::vector<Enemy*> enemies;

	PlayerState state;
	Look look;
	int jump_delay;
	TileMap *map;

	int score;
	int lifes;
	int attack_delay;	//delay between attacks

	Sound bubbleSound;
	Sound bubbleJump;
};

