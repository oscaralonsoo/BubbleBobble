#include "Lifes.h"
#include "ResourceManager.h"
#include "StaticImage.h"
/*#include "Globals.h"

Lifes::Lifes()
{
	
}
Lifes::~Lifes()
{
}

AppStatus Lifes::Initialise()
{
	int i;
	const int n = LIFES_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	if (data.LoadTexture(Resource::IMG_LIFES, "images/lifes.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	
	render = new StaticImage(data.GetTexture(Resource::IMG_LIFES), { 0, 0, n, n });
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for bubble sprite");
		return AppStatus::ERROR;
	}

	return AppStatus::OK;
}

void DrawLifes()
{
	StaticImages:Draw();
}


*/