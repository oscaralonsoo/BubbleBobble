
#include "Bubble.h"
#include "Sprite.h"
#include "TileMap.h"
#include <raymath.h>

Bubble::Bubble(const Point& p, BubbleState s, Direction dir) :
	Entity(p, BUBBLE_PHYSICAL_WIDTH, BUBBLE_PHYSICAL_HEIGHT, BUBBLE_FRAME_SIZE, BUBBLE_FRAME_SIZE)
{
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

	sprite->SetAnimationDelay((int)BubbleAnim::LEVITATING, 5);
	sprite->AddKeyFrame((int)BubbleAnim::LEVITATING, { 0, 0, n, n });
	sprite->SetAnimationDelay((int)BubbleAnim::LAUNCHING, 5);
	sprite->AddKeyFrame((int)BubbleAnim::LAUNCHING, { 0, 0, n, n });

	return AppStatus::OK;
}
void Bubble::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
void Bubble::Stop()
{
	state = BubbleState::LAUNCHING;
	SetAnimation((int)BubbleAnim::LAUNCHING);
}

void Bubble::Update()
{
	Stop();
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();
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