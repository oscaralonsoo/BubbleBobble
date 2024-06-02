#include "Object.h"
#include "StaticImage.h"

Object::Object(const Point& p, ObjectType t) : Entity(p, OBJECT_PHYSICAL_SIZE, OBJECT_PHYSICAL_SIZE, OBJECT_FRAME_SIZE, OBJECT_FRAME_SIZE)
{
	type = t; 
	
	Rectangle rc;
	const int n = OBJECT_FRAME_SIZE;
	switch (type)
	{
		case ObjectType::BANANA: rc = {3*n, 0, n, n}; break;
		case ObjectType::APPLE: rc = {0, 0, n, n}; break;
		case ObjectType::CHERRY: rc = { n, 0, n, n }; break;
		case ObjectType::LEMON: rc = { 2 * n, 0, n, n }; break;
		case ObjectType::PEAR: rc = { 0, n, n, n }; break;
		case ObjectType::WATERMELON: rc = { n, n, n, n }; break;
		case ObjectType::PINEAPPLE: rc = { 3*n, n, n, n }; break;
		case ObjectType::STRAWBERRY: rc = { 2*n, n, n, n }; break;

		default: LOG("Internal error: object creation of invalid type");
	}

	ResourceManager& data = ResourceManager::Instance();
	data.LoadTexture(Resource::IMG_FRUITS, "images/fruits.png");

	render = new StaticImage(data.GetTexture(Resource::IMG_FRUITS), rc);
}
Object::~Object()
{
}
void Object::DrawDebug(const Color& col) const
{
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
}
int Object::Points() const
{
	if (type == ObjectType::BANANA)				return POINTS_BANANA;
	else if (type == ObjectType::APPLE)			return POINTS_APPLE;
	if (type == ObjectType::CHERRY)				return POINTS_CHERRY;
	else if (type == ObjectType::LEMON)			return POINTS_LEMON;
	if (type == ObjectType::PEAR)				return POINTS_PEAR;
	else if (type == ObjectType::WATERMELON)	return POINTS_WATERMELON;
	if (type == ObjectType::PINEAPPLE)			return POINTS_PINEAPPLE;
	else if (type == ObjectType::STRAWBERRY)	return POINTS_STRAWBERRY;
	else
	{
		LOG("Internal error: object type invalid when giving points");
		return 0;
	}
}