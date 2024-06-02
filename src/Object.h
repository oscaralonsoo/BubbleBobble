#pragma once
#include "Entity.h"

#define OBJECT_PHYSICAL_SIZE	32
#define OBJECT_FRAME_SIZE		32

#define POINTS_BANANA	    500
#define POINTS_APPLE	    700
#define POINTS_CHERRY    	700
#define POINTS_LEMON	    350
#define POINTS_PEAR 	    550
#define POINTS_WATERMELON	600
#define POINTS_PINEAPPLE	5000
#define POINTS_STRAWBERRY	700

enum class ObjectType { BANANA, APPLE, CHERRY, LEMON, PEAR, WATERMELON, PINEAPPLE, STRAWBERRY };

class Object : public Entity
{
public:
	Object(const Point& p, ObjectType t);
	~Object();

	void DrawDebug(const Color& col) const;
	int Points() const;

private:
	ObjectType type;
};

