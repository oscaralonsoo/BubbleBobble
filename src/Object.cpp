#include "Object.h"
#include "StaticImage.h"

Object::Object(const Point& p, ObjectType t) : Entity(p, OBJECT_PHYSICAL_SIZE, OBJECT_PHYSICAL_SIZE, OBJECT_FRAME_SIZE, OBJECT_FRAME_SIZE)
{
	type = t; 
	
	Rectangle rc;
	const int n = TILE_SIZE;
	switch (type)
	{
		case ObjectType::BANANA: rc = {5*n, 0, n, n}; break;
		case ObjectType::APPLE: rc = {0, 8 * n, n, n}; break;
		case ObjectType::CHERRY: rc = { n, 8 * n, n, n }; break;
		case ObjectType::LEMON: rc = { 2 * n, 8 * n, n, n }; break;
		case ObjectType::PEAR: rc = { 3 * n, 8 * n, n, n }; break;
		case ObjectType::WATERMELON: rc = { 4 * n, 8 * n, n, n }; break;
		case ObjectType::PINEAPPLE: rc = { 5 * n, 8 * n, n, n }; break;
		case ObjectType::STRAWBERRY: rc = { 0, 9 * n, n, n }; break;

		default: LOG("Internal error: object creation of invalid type");
	}

	ResourceManager& data = ResourceManager::Instance();
	render = new StaticImage(data.GetTexture(Resource::IMG_TILES), rc);
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
	if (type == ObjectType::BANANA)		return POINTS_BANANA;
	else if (type == ObjectType::APPLE)	return POINTS_APPLE;
	if (type == ObjectType::CHERRY)		return POINTS_CHERRY;
	else if (type == ObjectType::LEMON)	return POINTS_LEMON;
	if (type == ObjectType::PEAR)		return POINTS_PEAR;
	else if (type == ObjectType::WATERMELON)	return POINTS_WATERMELON;
	if (type == ObjectType::PINEAPPLE)		return POINTS_PINEAPPLE;
	else if (type == ObjectType::STRAWBERRY)	return POINTS_STRAWBERRY;
	else
	{
		LOG("Internal error: object type invalid when giving points");
		return 0;
	}
}