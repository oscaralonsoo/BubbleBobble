#pragma once
#include "Entity.h"
#include "TileMap.h"

//Representation model size: 48x48
#define BUBBLE_FRAME_SIZE		48

//Logical model size: 12x28
#define BUBBLE_PHYSICAL_WIDTH	35
#define BUBBLE_PHYSICAL_HEIGHT	35

//Horizontal speed and vertical speed while falling down
#define BUBBLE_SPEED			7

//Player animations delay
#define ANIM_DELAY				10


enum class BubbleState { DISABLED, LAUNCHING, LEVITATING};
enum class Direction { RIGHT, LEFT };

//Rendering states
enum class BubbleAnim {
	DISABLED, LAUNCHING, LEVITATING,
	NUM_ANIMATIONS
};

class Bubble : public Entity
{
public:
	Bubble(int id, const Point& p, BubbleState s, Direction dir);
	~Bubble();

	void Update();

	// Animation management
	void SetAnimation(int id);
	BubbleAnim GetAnimation();
	void Disable();
	void StartLaunching(Point pos, Point dir);
	void StartLevitating();

	void LogicLaunching();
	void LogicLevitating();
	void HandleStates();

	AppStatus Initialise();
	void Release();
	void SetTileMap(TileMap* tilemap);

	BubbleState state;


private:
	Direction direction;

	TileMap* map;
	int bubbleId;
};