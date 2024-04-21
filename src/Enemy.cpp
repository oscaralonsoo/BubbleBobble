
#include "Enemy.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Globals.h"
#include <raymath.h>

Enemy::Enemy(const Point& p, EnemyState s, EnemyLook view) :
	Entity(p, ENEMY_PHYSICAL_WIDTH, ENEMY_PHYSICAL_HEIGHT, ENEMY_FRAME_SIZE, ENEMY_FRAME_SIZE)
{
	state = s;
	look = view;
	map = nullptr;
}
Enemy::~Enemy()
{
}
AppStatus Enemy::Initialise()
{
	int i;
	const int n = ENEMY_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_ENEMY, "images/enemy.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}

	render = new Sprite(data.GetTexture(Resource::IMG_ENEMY));
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for enemy sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)EnemyAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)EnemyAnim::WALKING_RIGHT, ANIM_WALKING_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_RIGHT, { (float)i * n, 0, -n, n });
	sprite->SetAnimationDelay((int)EnemyAnim::WALKING_LEFT, ANIM_WALKING_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_LEFT, { (float)i * n, 0, n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::DEAD_RIGHT, ANIM_DEAD_DELAY);
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_RIGHT, { 2*n, n, -n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_RIGHT, { 3*n, n, -n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_RIGHT, { 4*n, n, -n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_RIGHT, { 5*n, n, -n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_RIGHT, { 0,   2*n, -n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_RIGHT, { n,   2*n, -n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_RIGHT, { 2*n, 2*n, -n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::DEAD_LEFT, ANIM_DEAD_DELAY);
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_LEFT, { 2*n, n, n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_LEFT, { 3*n, n, n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_LEFT, { 4*n, n, n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_LEFT, { 5*n, n, n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_LEFT, { 0,   2*n, n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_LEFT, { n,   2*n, n, n });
	sprite->AddKeyFrame((int)EnemyAnim::DEAD_LEFT, { 2*n, 2*n, n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::IDLE_RIGHT, ANIM_IDLE_DELAY);
	sprite->AddKeyFrame((int)EnemyAnim::IDLE_RIGHT, { 0, 0, -n, n });
	sprite->SetAnimationDelay((int)EnemyAnim::IDLE_LEFT, ANIM_IDLE_DELAY);
	sprite->AddKeyFrame((int)EnemyAnim::IDLE_LEFT, { 0, 0, n, n });

	return AppStatus::OK;
}
void Enemy::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
}
bool Enemy::IsLookingRight() const
{
	return look == EnemyLook::RIGHT;
}
bool Enemy::IsLookingLeft() const
{
	return look == EnemyLook::LEFT;
}
void Enemy::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
EnemyAnim Enemy::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (EnemyAnim)sprite->GetAnimation();
}
void Enemy::Stop()
{
	dir = {0,0};
	state = EnemyState::IDLE;
	if (IsLookingRight())	SetAnimation((int)EnemyAnim::IDLE_RIGHT);
	else					SetAnimation((int)EnemyAnim::IDLE_LEFT);
}
void Enemy::StartWalkingLeft()
{
	state = EnemyState::WALKING;
	look = EnemyLook::LEFT;
	SetAnimation((int)EnemyAnim::WALKING_LEFT);
}
void Enemy::StartWalkingRight()
{
	state = EnemyState::WALKING;
	look = EnemyLook::RIGHT;
	SetAnimation((int)EnemyAnim::WALKING_RIGHT);
}
void Enemy::ChangeAnimRight()
{
	look = EnemyLook::RIGHT;
	switch (state)
	{
	case EnemyState::IDLE:	 SetAnimation((int)EnemyAnim::IDLE_RIGHT);    break;
	case EnemyState::WALKING: SetAnimation((int)EnemyAnim::WALKING_RIGHT); break;
	}
}
void Enemy::ChangeAnimLeft()
{
	look = EnemyLook::LEFT;
	switch (state)
	{
	case EnemyState::IDLE:	 SetAnimation((int)EnemyAnim::IDLE_LEFT);    break;
	case EnemyState::WALKING: SetAnimation((int)EnemyAnim::WALKING_LEFT); break;
	}
}
void Enemy::Update()
{
	MoveX();
	
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();
}
void Enemy::MoveX()
{
	/*AABB box;
	int prev_x = pos.x;
	
	box = GetHitbox();
	if (map->TestCollisionWallRight(box))
	{
		pos.x = prev_x;
	}*/
	
	//Provisional movement (CHANGE)
	if (GetAnimation() != EnemyAnim::DEAD_LEFT && GetAnimation() != EnemyAnim::DEAD_RIGHT)
	{
		if (posTest == 0)
		{
			posTest = GetPos().x;
		}

		if (GetPos().x > (posTest + LIMIT))
		{
			dirTest = -1;
			StartWalkingLeft();
		}
		else if (GetPos().x < (posTest - LIMIT))
		{
			dirTest = 1;
			StartWalkingRight();
		}
		pos.x += ENEMY_SPEED * dirTest;
	}
	
}
void Enemy::DrawDebug(const Color& col) const
{
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);

	DrawPixel(pos.x, pos.y, WHITE);
}

void Enemy::StartDeath()
{
	if (GetAnimation() != EnemyAnim::DEAD_LEFT && GetAnimation() != EnemyAnim::DEAD_RIGHT)
	{
		state = EnemyState::DEAD;
		if (IsLookingRight())	SetAnimation((int)EnemyAnim::DEAD_RIGHT);
		else					SetAnimation((int)EnemyAnim::DEAD_LEFT);
	}
}

void Enemy::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_ENEMY);

	render->Release();
}