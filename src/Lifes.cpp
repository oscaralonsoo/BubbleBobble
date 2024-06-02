#include "Lifes.h"
#include "ResourceManager.h"
#include "StaticImage.h"
#include "Globals.h"

Lifes::Lifes(const Point& p) : Entity(p, LIFES_FRAME_SIZE, LIFES_FRAME_SIZE, LIFES_FRAME_SIZE, LIFES_FRAME_SIZE)
{
	ResourceManager& data = ResourceManager::Instance();
	data.LoadTexture(Resource::IMG_LIFES, "images/lifes.png");

	render = new StaticImage(data.GetTexture(Resource::IMG_LIFES), { 0, 0, LIFES_FRAME_SIZE, LIFES_FRAME_SIZE });
}
Lifes::~Lifes()
{
}