#include "ZenChan.h"
#include "Sprite.h"

ZenChan::ZenChan(const Point& p, int width, int height, int frame_width, int frame_height) :
	Enemy(p, width, height, frame_width, frame_height)
{
	state = ZenChanState::ROAMING;

	current_step = 0;
	current_frames = 0;
	current_pos = 0;
}
ZenChan::~ZenChan()
{
}
AppStatus ZenChan::Initialise(Look look, const AABB& area)
{
	int i;
	const int n = ZENCHAN_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	render = new Sprite(data.GetTexture(Resource::IMG_ENEMY));
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

	this->look = look;
	if (look == Look::LEFT)        sprite->SetAnimation((int)ZenChanAnim::WALKING_LEFT);
	else if (look == Look::RIGHT) sprite->SetAnimation((int)ZenChanAnim::WALKING_RIGHT);

	visibility_area = area;

	return AppStatus::OK;
}
bool ZenChan::Update(const AABB& box)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	int direction = (look == Look::RIGHT) ? 1 : -1;

	if (map->TestCollisionWallRight(GetHitbox()))
	{
		StartWalkingLeft();
	}
	if (map->TestCollisionWallLeft(GetHitbox()))
	{
		StartWalkingRight();
	}

	if (state == ZenChanState::ROAMING)
	{
		pos += {ZENCHAN_SPEED * direction, 0};

		if (map->TestFalling(GetHitbox()))
		{
			if (GetRandomValue(0, 1))
			{
				TraceLog(LOG_INFO, "salta");
				state = ZenChanState::JUMPING;
			}
			else
			{
				TraceLog(LOG_INFO, "salta");
				state = ZenChanState::JUMPING;
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
		if (map->TestFalling(GetHitbox()))
		{
			pos += {ZENCHAN_SPEED * direction, 0};
		}
		else
		{
			state = ZenChanState::ROAMING;
		}

	}

	sprite->Update();

	return shoot;
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
void ZenChan::StartWalkingRight()
{
	look = Look::RIGHT;
	SetAnimation((int)ZenChanAnim::WALKING_RIGHT);
}
void ZenChan::GetShootingPosDir(Point* p, Point* d) const
{
	
}