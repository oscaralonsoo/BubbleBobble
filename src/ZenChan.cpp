#include "ZenChan.h"
#include "Sprite.h"

ZenChan::ZenChan(const Point& p, int width, int height, int frame_width, int frame_height) :
	Enemy(p, width, height, frame_width, frame_height)
{
	state = ZenChanState::ROAMING;

	current_step = 0;
	current_frames = 0;
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

	InitPattern();

	return AppStatus::OK;
}
void ZenChan::InitPattern()
{
	//Multiplying by 3 ensures sufficient time for displaying all 3 frames of the
	//walking animation, resulting in smoother transitions and preventing the animation
	//from appearing rushed or incomplete
	const int n = ZENCHAN_ANIM_DELAY * 3;
	
	pattern.push_back({ {0, 0}, 2 * n, (int)ZenChanAnim::WALKING_RIGHT });
	pattern.push_back({ {ZENCHAN_SPEED, 0}, n, (int)ZenChanAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)ZenChanAnim::WALKING_RIGHT });
	pattern.push_back({ {ZENCHAN_SPEED, 0}, n, (int)ZenChanAnim::WALKING_RIGHT });
	pattern.push_back({ {0, 0}, n, (int)ZenChanAnim::WALKING_RIGHT });

	pattern.push_back({ {0, 0}, 2 * n, (int)ZenChanAnim::WALKING_LEFT });
	pattern.push_back({ {-ZENCHAN_SPEED, 0}, n, (int)ZenChanAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)ZenChanAnim::WALKING_LEFT });
	pattern.push_back({ {-ZENCHAN_SPEED, 0}, n, (int)ZenChanAnim::WALKING_LEFT });
	pattern.push_back({ {0, 0}, n, (int)ZenChanAnim::WALKING_LEFT });

	current_step = 0;
	current_frames = 0;
}
bool ZenChan::Update(const AABB& box)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	bool shoot = false;
	int anim_id;

	if (state == ZenChanState::ROAMING)
	{
		TraceLog(LOG_INFO, "%s", (map->TestBeforeFalling(GetHitbox()) ? "true" : "false"));
		pos += pattern[current_step].speed;
		current_frames++;

		if (current_frames == pattern[current_step].frames)
		{
			current_step++;
			current_step %= pattern.size();
			current_frames = 0;

			anim_id = pattern[current_step].anim;
			sprite->SetAnimation(anim_id);
			UpdateLook(anim_id);
		}
		
	}

	sprite->Update();

	return shoot;
}
void ZenChan::UpdateLook(int anim_id)
{
	ZenChanAnim anim = (ZenChanAnim)anim_id;
	look = (anim == ZenChanAnim::WALKING_LEFT) ? Look::LEFT : Look::RIGHT;
}

void ZenChan::GetShootingPosDir(Point* p, Point* d) const
{
	
}