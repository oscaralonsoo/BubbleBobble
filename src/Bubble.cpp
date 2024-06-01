
#include "Bubble.h"
#include "Sprite.h"
#include "TileMap.h"
#include <raymath.h>

Bubble::Bubble()
{
	map = nullptr;
	state = BubbleState::LAUNCHING;
	center = 375;
	isMovingUp = false;

	int i;
	const int n = BUBBLE_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	data.LoadTexture(Resource::IMG_BUBBLE, "images/bubble.png");

	render = new Sprite(data.GetTexture(Resource::IMG_BUBBLE));

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)BubbleAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)BubbleAnim::LEVITATING, 10);
	sprite->AddKeyFrame((int)BubbleAnim::LEVITATING, { 5 * n, 0, n, n });

	sprite->SetAnimationDelay((int)BubbleAnim::LAUNCHING, 5);
	for (i = 0; i < 5; ++i)
		sprite->AddKeyFrame((int)BubbleAnim::LAUNCHING, { (float)i * n, 0, n, n });

	sprite->SetAnimationDelay((int)BubbleAnim::HIT, 10);
	for (i = 2; i < 5; ++i)
		sprite->AddKeyFrame((int)BubbleAnim::HIT, { (float)i * n, n, n, n });

	SetAlive(false);
}
Bubble::~Bubble()
{
}

void Bubble::Init(const Point& p, const Point& dir)
{
	Set(p, dir, BUBBLE_PHYSICAL_WIDTH, BUBBLE_PHYSICAL_HEIGHT, BUBBLE_FRAME_SIZE, BUBBLE_FRAME_SIZE);
}
bool Bubble::IsMovingLeft() const
{
	return dir.x < 0;
}
bool Bubble::IsMovingRight() const
{
	return dir.x > 0;
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
void Bubble::StartLaunching()
{
	if (GetAnimation() != BubbleAnim::LAUNCHING)
	{
		state = BubbleState::LAUNCHING;
		SetAnimation((int)BubbleAnim::LAUNCHING);
		SetCenter();
	}
}
void Bubble::StartHit()
{
	state = BubbleState::LEVITATING;
	SetAnimation((int)BubbleAnim::HIT);
	
}
void Bubble::StartLevitating()
{
	if (GetAnimation() != BubbleAnim::LEVITATING)
	{
		state = BubbleState::LEVITATING;
		SetAnimation((int)BubbleAnim::LEVITATING);
	}
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
		StartLaunching();
		LogicLaunching();
	}
	else if (state == BubbleState::LEVITATING)
	{
		LogicLevitating();
	}
	else if (state == BubbleState::SUSPENSION)
	{
		LogicSuspension();
	}
}

void Bubble::LogicLaunching()
{
	pos.x += dir.x;

	if (GetAnimation() == BubbleAnim::LAUNCHING)
	{
		Sprite* sprite = dynamic_cast<Sprite*>(render);

		if (sprite->IsLastFrame())
		{
			StartLevitating();
		}
	}
}
void Bubble::LogicSuspension()
{
	if (pos.y <= BUBBLE_LIMIT_HEIGHT)
	{
		isMovingUp = false;
	}

	if (!isMovingUp)
	{
		pos.y += 1;

		if (pos.y >= BUBBLE_LIMIT_HEIGHT + 40)
		{
			isMovingUp = true;
		}
	}
	else {
		pos.y -= 1;
	}
}

void Bubble::SetCenter()
{
	center = GetRandomValue(BUBBLE_MIN_CENTER, BUBBLE_MAX_CENTER);
}
void Bubble::LogicLevitating()
{
	if (pos.y > BUBBLE_LIMIT_HEIGHT)
	{
		pos.y -= 1;
	}
	else if (pos.x > center)
	{
		pos.x -= 1;
	}
	else if (pos.x < center)
	{
		pos.x += 1;
	}
	else {
		state = BubbleState::SUSPENSION;
	}
}

void Bubble::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_BUBBLE);

	render->Release();
}

void Bubble::DrawDebug(const Color& col) const
{
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);

	DrawPixel(pos.x, pos.y, WHITE);
}

void Bubble::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
}