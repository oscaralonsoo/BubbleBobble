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

	sprite->SetAnimationDelay((int)MonstaAnim::WALKING_RIGHT, MONSTA_ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::WALKING_RIGHT, { (float)i * n, 0, -n, n });
	sprite->SetAnimationDelay((int)MonstaAnim::WALKING_LEFT, MONSTA_ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::WALKING_LEFT, { (float)i * n, 0, n, n });

	sprite->SetAnimationDelay((int)MonstaAnim::DIED, MONSTA_ANIM_DELAY);
	for (i = 2; i < 6; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::DIED, { (float)i * n, n, n, n });
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)MonstaAnim::DIED, { (float)i * n, 2 * n, n, n });

	sprite->SetAnimationDelay((int)MonstaAnim::HITTED, MONSTA_ANIM_DELAY);
	sprite->AddKeyFrame((int)MonstaAnim::HITTED, { (float) 4*n, 4*n, n, n });

	this->look = look;
	if (look == Look::LEFT)        sprite->SetAnimation((int)MonstaAnim::WALKING_LEFT);
	else if (look == Look::RIGHT) sprite->SetAnimation((int)MonstaAnim::WALKING_RIGHT);

	return AppStatus::OK;
}
void Monsta::Update(const AABB& box, TileMap* map)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	int direction = (look == Look::RIGHT) ? 1 : -1;

	OutOfScreen();

	if (map->TestCollisionWallRight(GetHitbox()))
	{
		StartWalkingLeft();
		pos.x -= MONSTA_OFFSET * direction;
		map->TestCollisionGround(GetHitbox(), &pos.y);
	}
	if (map->TestCollisionWallLeft(GetHitbox()))
	{
		StartWalkingRight();
		pos.x -= MONSTA_OFFSET * direction;
		map->TestCollisionGround(GetHitbox(), &pos.y);
	}

	if (!IsAlive())
	{
		StartHitted();
	}

	if (state == MonstaState::ROAMING)
	{
		pos += {MONSTA_SPEED * direction, 0};

			if (randomValue)
			{
				if (map->TestBeforeFalling(GetHitbox(), direction))
				{
					state = MonstaState::JUMPING;
					randomValue = GetRandomValue(0, 1);
				}
			}
			else
			{
				if (map->TestFalling(GetHitbox()))
				{
					state = MonstaState::FALLING;
					randomValue = GetRandomValue(0, 1);
				}
			}
		
	}
	else if (state == MonstaState::FALLING)
	{
		if (map->TestFalling(GetHitbox()))
		{
			pos += {0, MONSTA_SPEED};
		}
		else {
			state = MonstaState::ROAMING;
		}
	}
	else if (state == MonstaState::JUMPING)
	{
		if (map->TestBeforeFalling(GetHitbox(), direction))
		{
			if (current_pos < MONSTA_JUMP_HEIGHT)
			{
				pos += {3 * direction, -MONSTA_JUMP_SPEED};
				current_pos += MONSTA_JUMP_SPEED;
			}
			else
			{
				pos += {3 * direction, MONSTA_JUMP_SPEED};
			}
		}
		else
		{
			current_pos = 0;
			state = MonstaState::ROAMING;
		}
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
void Monsta::StartWalkingLeft()
{
	look = Look::LEFT;
	SetAnimation((int)MonstaAnim::WALKING_LEFT);
}
void Monsta::StartHitted()
{
	if (GetAnimation() != MonstaAnim::HITTED)
	{
		SetPos({ -20, -20 });
		SetAnimation((int)MonstaAnim::HITTED);
	}
}
void Monsta::StartWalkingRight()
{
	look = Look::RIGHT;
	SetAnimation((int)MonstaAnim::WALKING_RIGHT);
}
void Monsta::KillEnemy()
{
	state = MonstaState::DIED;
	SetAnimation((int)MonstaAnim::DIED);
}