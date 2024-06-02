#pragma once
#include "Enemy.h"
#include "TileMap.h"
#include "Object.h"

#define ZENCHAN_SPEED			3
#define ZENCHAN_ANIM_DELAY	(4*ANIM_DELAY)
#define ZENCHAN_JUMP_HEIGHT     20
#define ZENCHAN_JUMP_SPEED      1
#define ZENCHAN_OFFSET          7   //offset colliders for avoid bugs

enum class ZenChanState { ROAMING, JUMPING, DIED, FALLING, HITTED};
enum class ZenChanAnim {
	WALKING_LEFT, WALKING_RIGHT, HITTED, DIED,
	NUM_ANIMATIONS
};

struct Step
{
	Point speed;	//direction
	int frames;		//duration in number of frames
	int anim;		//graphical representation
};

class ZenChan : public Enemy
{
public:
	ZenChan(const Point& p, int width, int height, int frame_width, int frame_height, int type);
	~ZenChan();

	//Initialize the enemy with the specified look and area
	AppStatus Initialise(Look look) override;

	//Update the enemy according to its logic, return true if the enemy must shoot
	void Update(std::vector<Object*> objects, TileMap* tilemap) override;

	int GetType();
private:
	//Animation management
	ZenChanAnim GetAnimation();
	void SetAnimation(int id);
	void StartWalkingLeft();
	void StartWalkingRight();
	void KillEnemy();
	void StartHitted();

	ZenChanState state;

	Object* obj;
	
	int current_step;	//current step of the pattern
	int current_frames;	//number of frames in the current step
	int current_pos;	//current pos before jump
	int randomValue;	//50% posibilities jump/fall
};

