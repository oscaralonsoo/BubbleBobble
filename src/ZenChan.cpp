#include "ZenChan.h"
#include "Sprite.h"

ZenChan::ZenChan(const Point& p, int width, int height, int frame_width, int frame_height) :
	Enemy(p, width, height, frame_width, frame_height)
{
	state = ZenChanState::ROAMING;

	current_step = 0;
	current_frames = 0;
	current_pos = 0;
	randomValue = GetRandomValue(0, 1);
}
ZenChan::~ZenChan()
{
}
AppStatus ZenChan::Initialise(Look look)
{
	int i;
	const int n = ZENCHAN_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	render = new Sprite(data.GetTexture(Resource::IMG_ZENCHAN));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for zen chan sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)ZenChanAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)ZenChanAnim::WALKING_RIGHT, ZENCHAN_ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)ZenChanAnim::WALKING_RIGHT, { (float)i * n, 0, -n, n });
	sprite->SetAnimationDelay((int)ZenChanAnim::WALKING_LEFT, ZENCHAN_ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)ZenChanAnim::WALKING_LEFT, { (float)i * n, 0, n, n });

	sprite->SetAnimationDelay((int)ZenChanAnim::DIED, ZENCHAN_ANIM_DELAY);
	for (i = 2; i < 6; ++i)
		sprite->AddKeyFrame((int)ZenChanAnim::DIED, { (float)i * n, n, n, n });
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)ZenChanAnim::DIED, { (float)i * n, 2 * n, n, n });

	sprite->SetAnimationDelay((int)ZenChanAnim::HITTED, ZENCHAN_ANIM_DELAY);
	sprite->AddKeyFrame((int)ZenChanAnim::HITTED, { (float) 4*n, 4*n, n, n });

	this->look = look;
	if (look == Look::LEFT)        sprite->SetAnimation((int)ZenChanAnim::WALKING_LEFT);
	else if (look == Look::RIGHT) sprite->SetAnimation((int)ZenChanAnim::WALKING_RIGHT);

	return AppStatus::OK;
}
void ZenChan::Update(const AABB& box, TileMap* map)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	int direction = (look == Look::RIGHT) ? 1 : -1;

	OutOfScreen();

	if (state != ZenChanState::DIED)
	{
		if (map->TestCollisionWallRight(GetHitbox()))
		{
			StartWalkingLeft();
			pos.x -= ZENCHAN_OFFSET * direction;
			map->TestCollisionGround(GetHitbox(), &pos.y);
		}
		if (map->TestCollisionWallLeft(GetHitbox()))
		{
			StartWalkingRight();
			pos.x -= ZENCHAN_OFFSET * direction;
			map->TestCollisionGround(GetHitbox(), &pos.y);

		}
	}
	

	if (!IsAlive())
	{
		StartHitted();
	}

	if (state == ZenChanState::ROAMING)
	{
		pos += {ZENCHAN_SPEED* direction, 0};

		if (randomValue)
		{
			if (map->TestBeforeFalling(GetHitbox(), direction))
			{
				state = ZenChanState::JUMPING;
				randomValue = GetRandomValue(0, 1);
			}
		}
		else
		{
			if (map->TestFalling(GetHitbox()))
			{
				state = ZenChanState::FALLING;
				randomValue = GetRandomValue(0, 1);
			}
		}

	}
	else if (state == ZenChanState::FALLING)
	{
		if (map->TestFalling(GetHitbox()))
		{
			pos += {0, ZENCHAN_SPEED};
		}
		else {
			state = ZenChanState::ROAMING;
		}
	}
	else if (state == ZenChanState::JUMPING)
	{
		if (map->TestBeforeFalling(GetHitbox(), direction))
		{
			if (current_pos < ZENCHAN_JUMP_HEIGHT)
			{
				pos += {3 * direction, -ZENCHAN_JUMP_SPEED};
				current_pos += ZENCHAN_JUMP_SPEED;
			}
			else
			{
				pos += {3 * direction, ZENCHAN_JUMP_SPEED};
			}
		}
		else
		{
			current_pos = 0;
			state = ZenChanState::ROAMING;
		}
	}
	else if (state == ZenChanState::HITTED)
	{
		StartHitted();
	}
	else if (state == ZenChanState::DIED)
	{

		if (!map->TestCollisionCeiling(GetHitbox()))
		{
			pos.y += 1;
		}
		else {

		}
	}

	sprite->Update();

}
ZenChanAnim ZenChan::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (ZenChanAnim)sprite->GetAnimation();
}
void ZenChan::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
void ZenChan::StartWalkingLeft()
{
	look = Look::LEFT;
	SetAnimation((int)ZenChanAnim::WALKING_LEFT);
}
void ZenChan::StartHitted()
{
	if (GetAnimation() != ZenChanAnim::HITTED)
	{
		SetPos({ -20, -20 });
		SetAnimation((int)ZenChanAnim::HITTED);
	}
}
void ZenChan::StartWalkingRight()
{
	look = Look::RIGHT;
	SetAnimation((int)ZenChanAnim::WALKING_RIGHT);
}
void ZenChan::KillEnemy()
{
	state = ZenChanState::DIED;
	SetAnimation((int)ZenChanAnim::DIED);
}