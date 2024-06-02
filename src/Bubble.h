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

//Limits levitating
#define BUBBLE_LIMIT_HEIGHT		85
#define BUBBLE_MIN_CENTER		325
#define BUBBLE_MAX_CENTER		425

//Distance of suspension

//Player animations delay
#define ANIM_DELAY				6


enum class BubbleState { LAUNCHING, LEVITATING, HIT, SUSPENSION};
enum class Direction { RIGHT, LEFT };

//Rendering states
enum class BubbleAnim {
	LAUNCHING, LEVITATING, HIT_ZENCHAN, HIT_MONSTA,
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
	void SetCenter();

	//Animation management
	void SetAnimation(int id);
	BubbleAnim GetAnimation();
	void StartLaunching();
	void StartHit(int type);
	void StartLevitating();

	//Logic management
	void LogicLaunching();
	void LogicLevitating();
	void LogicSuspension();
	void HandleStates();

	AppStatus Initialise();
	void Release();
	void SetTileMap(TileMap* tilemap);

	BubbleState state;

private:
	TileMap* map;
	int center;
	bool isMovingUp;
};