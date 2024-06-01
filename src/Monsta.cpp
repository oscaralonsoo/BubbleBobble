#include "Monsta.h"
#include "Sprite.h"

Monsta::Monsta(const Point& p, int width, int height, int frame_width, int frame_height) :
	Enemy(p, width, height, frame_width, frame_height)
{
	state = MonstaState::ROAMING;

	current_step = 0;
	current_frames = 0;
	current_pos = 0;
	randomValue = GetRandomValue(0, 1);
}
Monsta::~Monsta()
{
}
AppStatus Monsta::Initialise(Look look)
{
	int i;
	const int n = MONSTA_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	render = new Sprite(data.GetTexture(Resource::IMG_MONSTA));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for zen chan sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)MonstaAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)MonstaAnim::ROAMING_RIGHT, MONSTA_ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::ROAMING_RIGHT, { (float)i * n, 0, -n, n });
	sprite->SetAnimationDelay((int)MonstaAnim::ROAMING_LEFT, MONSTA_ANIM_DELAY);
	for (i = 0; i < 2; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::ROAMING_LEFT, { (float)i * n, 0, n, n });

	sprite->SetAnimationDelay((int)MonstaAnim::DIED, MONSTA_ANIM_DELAY);
	for (i = 2; i < 6; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::DIED, { (float)i * n, n, n, n });
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::DIED, { (float)i * n, 2 * n, n, n });

	sprite->SetAnimationDelay((int)MonstaAnim::HITTED, MONSTA_ANIM_DELAY);
	sprite->AddKeyFrame((int)MonstaAnim::HITTED, { (float) 4*n, 4*n, n, n });

	this->look = look;
	if (look == Look::LEFT)        sprite->SetAnimation((int)MonstaAnim::ROAMING_LEFT);
	else if (look == Look::RIGHT) sprite->SetAnimation((int)MonstaAnim::ROAMING_RIGHT);

	return AppStatus::OK;
}
void Monsta::Update(const AABB& box, TileMap* map)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	Point direction;
	
	direction.x = (look == Look::RIGHT) ? 1 : -1;
	direction.y = (look == Look::RIGHT) ? 1 : -1;


	OutOfScreen();

	if (map->TestCollisionCeiling(GetHitbox()))
	{
		pos.y -= MONSTA_OFFSET * direction.y;
	}
	else if (map->TestCollisionGround(GetHitbox(), &pos.y))
	{
		pos.y += MONSTA_OFFSET * direction.y;
	}
	
	if (map->TestCollisionWallRight(GetHitbox()))
	{
		StartRoamingLeft();
		pos.x -= MONSTA_OFFSET * direction.x;
	}
	else if (map->TestCollisionWallLeft(GetHitbox()))
	{
		StartRoamingRight();
		pos.x -= MONSTA_OFFSET * direction.x;
	}
	

	if (!IsAlive())
	{
		StartHitted();
	}

	if (state == MonstaState::ROAMING)
	{
		pos += {MONSTA_SPEED* direction.x, MONSTA_SPEED* direction.y};
		
	}
	else if (state == MonstaState::HITTED)
	{
		StartHitted();
	}
	else if (state == MonstaState::DIED)
	{

	}

	sprite->Update();
}
MonstaAnim Monsta::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (MonstaAnim)sprite->GetAnimation();
}
void Monsta::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
void Monsta::StartRoamingLeft()
{
	look = Look::LEFT;
	SetAnimation((int)MonstaAnim::ROAMING_LEFT);
}
void Monsta::StartHitted()
{
	if (GetAnimation() != MonstaAnim::HITTED)
	{
		SetPos({ -20, -20 });
		SetAnimation((int)MonstaAnim::HITTED);
	}
}
void Monsta::StartRoamingRight()
{
	look = Look::RIGHT;
	SetAnimation((int)MonstaAnim::ROAMING_RIGHT);
}
void Monsta::KillEnemy()
{
	state = MonstaState::DIED;
	SetAnimation((int)MonstaAnim::DIED);
}