#pragma once
#include "Enemy.h"
#include "TileMap.h"
#include "Object.h";

#define MONSTA_SPEED			3
#define MONSTA_ANIM_DELAY	(4*ANIM_DELAY)
#define MONSTA_JUMP_HEIGHT     20
#define MONSTA_JUMP_SPEED      1
#define MONSTA_OFFSET          1   //offset colliders for avoid bugs

enum class MonstaState { ROAMING, HITTED, DIED };
enum class MonstaAnim {
	ROAMING_LEFT, ROAMING_RIGHT, HITTED, DIED,
	NUM_ANIMATIONS
};

class Monsta : public Enemy
{
public:
	Monsta(const Point& p, int width, int height, int frame_width, int frame_height, int type);
	~Monsta();

	//Initialize the enemy with the specified look and area
	AppStatus Initialise(Look look) override;

	//Update the enemy according to its logic, return true if the enemy must shoot
	void Update(std::vector<Object*> objects, TileMap* map) override;

	int GetType();

private:
	//Animation management
	MonstaAnim GetAnimation();
	void SetAnimation(int id);
	void StartRoamingLeft();
	void StartRoamingRight();
	void KillEnemy();
	void StartHitted();

	MonstaState state;

	Point direction;

	int current_step;	//current step of the pattern
	int current_frames;	//number of frames in the current step
	int current_pos;	//current pos before jump
	int randomValue;	//50% posibilities jump/fall
};

