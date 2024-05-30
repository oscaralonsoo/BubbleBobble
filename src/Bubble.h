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


enum class BubbleState { LAUNCHING, LEVITATING, HIT};
enum class Direction { RIGHT, LEFT };

//Rendering states
enum class BubbleAnim {
	LAUNCHING, LEVITATING, HIT,
	NUM_ANIMATIONS
};

class Bubble : public Entity
{
public:
	Bubble();
	~Bubble();

	void Update();

	void Init(const Point& p, const Point& dir);
	bool IsMovingLeft() const;
	bool IsMovingRight() const;
	void DrawDebug(const Color& col) const;

	//Animation management
	void SetAnimation(int id);
	BubbleAnim GetAnimation();
	void StartLaunching();
	void StartHit();
	void StartLevitating();

	//Logic management
	void LogicLaunching();
	void LogicLevitating();
	void HandleStates();

	AppStatus Initialise();
	void Release();
	void SetTileMap(TileMap* tilemap);

	BubbleState state;

private:
	TileMap* map;
};