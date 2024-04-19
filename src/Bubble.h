#pragma once
#include "Entity.h"
#include "TileMap.h"

//Representation model size: 48x48
#define BUBBLE_FRAME_SIZE		48

//Logical model size: 12x28
#define BUBBLE_PHYSICAL_WIDTH	35
#define BUBBLE_PHYSICAL_HEIGHT	35

//Horizontal speed and vertical speed while falling down
#define BUBBLE_SPEED			2.6

//Player animations delay
#define ANIM_DELAY				10


enum class BubbleState { LAUNCHING, LEVITATING};
enum class Direction { RIGHT, LEFT };

//Rendering states
enum class BubbleAnim {
	LAUNCHING, LEVITATING,
	NUM_ANIMATIONS
};

class Bubble : public Entity
{
public:
	Bubble(const Point& p, BubbleState s, Direction dir);
	~Bubble();

	void Update();
	void Stop();
	void SetAnimation(int id);

	AppStatus Initialise();
	void Release();
	void SetTileMap(TileMap* tilemap);

private:
	BubbleState state;
	Direction direction;

	TileMap* map;

};