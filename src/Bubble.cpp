
#include "Bubble.h"
#include "Sprite.h"
#include "TileMap.h"
#include <raymath.h>

Bubble::Bubble(int id, const Point& p, BubbleState s, Direction dir) :
	Entity(p, BUBBLE_PHYSICAL_WIDTH, BUBBLE_PHYSICAL_HEIGHT, BUBBLE_FRAME_SIZE, BUBBLE_FRAME_SIZE)
{
	bubbleId = id;
	state = s;
	direction = dir;
	map = nullptr;
}
Bubble::~Bubble()
{
}

AppStatus Bubble::Initialise()
{
	int i;
	const int n = BUBBLE_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_BUBBLE, "images/bubble.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}

	render = new Sprite(data.GetTexture(Resource::IMG_BUBBLE));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for bubble sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)BubbleAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)BubbleAnim::LEVITATING, 10);
	sprite->AddKeyFrame((int)BubbleAnim::LEVITATING, { 5*n, 0, n, n });

	sprite->SetAnimationDelay((int)BubbleAnim::LAUNCHING, 5);
	for (i = 0; i < 5; ++i)
		sprite->AddKeyFrame((int)BubbleAnim::LAUNCHING, { (float) i*n, 0, n, n });

	sprite->AddKeyFrame((int)BubbleAnim::DISABLED, { 0, 2*n, n, n });

	return AppStatus::OK;
}
void Bubble::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
BubbleAnim Bubble::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (BubbleAnim)sprite->GetAnimation();
}
void Bubble::Disable()
{
	state = BubbleState::DISABLED;
	SetAnimation((int)BubbleAnim::DISABLED);
}
void Bubble::StartLaunching(Point pos, Point dir)
{
	SetPos(pos);
	SetDir(dir);
	state = BubbleState::LAUNCHING;
	SetAnimation((int)BubbleAnim::LAUNCHING);
}
void Bubble::StartLevitating()
{
	state = BubbleState::LEVITATING;
	SetAnimation((int)BubbleAnim::LEVITATING);
}
void Bubble::Update()
{
	HandleStates();

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();
}

void Bubble::HandleStates()
{
	if (state == BubbleState::LAUNCHING)
	{
		LogicLaunching();
	}
	else if (state == BubbleState::LEVITATING)
	{
		StartLevitating();
		LogicLevitating();
	}
}

void Bubble::LogicLaunching()
{
	dir.x = BUBBLE_SPEED;
	pos.x += dir.x;
	if (GetAnimation() == BubbleAnim::LAUNCHING || GetAnimation() == BubbleAnim::LAUNCHING)
	{
		Sprite* sprite = dynamic_cast<Sprite*>(render);

		if (sprite->IsLastFrame())
		{
			state = BubbleState::LEVITATING;
		}
	}
}

void Bubble::LogicLevitating()
{
	pos.y -= 1;
}

void Bubble::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_BUBBLE);

	render->Release();
}

void Bubble::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
}