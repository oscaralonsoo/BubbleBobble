#include "TileMap.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <cstring>

TileMap::TileMap()
{
	map = nullptr;
	width = 0;
	height = 0;
	img_tiles = nullptr;

	InitTileDictionary();
}
TileMap::~TileMap()
{
	if (map != nullptr)
	{
		delete[] map;
		map = nullptr;
	}
}
void TileMap::InitTileDictionary()
{
	const int n = TILE_SIZE;

	dict_rect[(int)Tile::BLOCK_SQUARE1] = { n,  0, n, n };
	dict_rect[(int)Tile::BLOCK_SQUARE1_R] = { 2 * n,  0, n, n };
	dict_rect[(int)Tile::BLOCK_SQUARE1_B] = { 3 * n, 0, n, n };
	dict_rect[(int)Tile::BLOCK_SQUARE1_BL] = { 4 * n, 0, n, n };
	dict_rect[(int)Tile::BLOCK_WALL_L] = { 0, n, n, n };
	dict_rect[(int)Tile::BLOCK_WALL_R] = { n, n, n, n };
	dict_rect[(int)Tile::BLOCK_SQUARE1_BR] = { 2 * n, n, n, n };
	dict_rect[(int)Tile::BLOCK_SQUARE1_TR] = { 3 * n, n, n, n };
	dict_rect[(int)Tile::BLOCK_CORNER] = { 4 * n, n, n, n };
}
AppStatus TileMap::Initialise()
{
	ResourceManager& data = ResourceManager::Instance();

	if (data.LoadTexture(Resource::IMG_TILES, "images/tileset.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	img_tiles = data.GetTexture(Resource::IMG_TILES);

	return AppStatus::OK;
}
AppStatus TileMap::Load(int data[], int w, int h)
{
	size = w*h;
	width = w;
	height = h;

	if (map != nullptr)	delete[] map;

	map = new Tile[size];
	if (map == nullptr)	
	{
		LOG("Failed to allocate memory for tile map");
		return AppStatus::ERROR;
	}
	memcpy(map, data, size * sizeof(int));

	return AppStatus::OK;
}
void TileMap::ClearObjectEntityPositions()
{
	int i;
	Tile tile;
	for (i = 0; i < size; ++i)
	{
		tile = map[i];
		if (IsTileEntity(tile) || IsTileObject(tile) || tile == Tile::EMPTY)
			map[i] = Tile::AIR;
	}
}
void TileMap::Update()
{
}
Tile TileMap::GetTileIndex(int x, int y) const
{
	int idx = x + y*width;
	if(idx < 0 || idx >= size)
	{
		LOG("Error: Index out of bounds. Tile map dimensions: %dx%d. Given index: (%d, %d)", width, height, x, y)
		return Tile::AIR;
	}
	return map[x + y * width];
}
bool TileMap::IsTileObject(Tile tile) const
{
	return Tile::OBJECT_FIRST <= tile && tile <= Tile::OBJECT_LAST;
}
bool TileMap::IsTileEntity(Tile tile) const
{
	return Tile::ENTITY_FIRST <= tile && tile <= Tile::ENTITY_LAST;
}
bool TileMap::IsTileSolid(Tile tile) const
{
	return Tile::SOLID_FIRST <= tile && tile <= Tile::SOLID_LAST;
}
bool TileMap::TestCollisionWallLeft(const AABB& box) const
{
	return CollisionX(box.pos, box.height);
}
bool TileMap::TestCollisionWallRight(const AABB& box) const
{
	return CollisionX(box.pos + Point(box.width - 1, 0), box.height);
}
bool TileMap::TestCollisionGround(const AABB& box, int* py) const
{
	Point p(box.pos.x, *py);	//control point
	int tile_y;

	if (CollisionY(p, box.width))
	{
		tile_y = p.y / TILE_SIZE;

		*py = tile_y * TILE_SIZE - 1;
		return true;
	}
	return false;
}
bool TileMap::TestFalling(const AABB& box) const
{
	return !CollisionY(box.pos + Point(0, box.height), box.width);
}
bool TileMap::TestBeforeFalling(const AABB& box, int dir) const
{
	return !CollisionY(box.pos + Point(box.width * dir, box.height), box.width);
}
bool TileMap::CollisionX(const Point& p, int distance) const
{
	int x, y, y0, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y0 = p.y / TILE_SIZE;
	y1 = (p.y + distance - 1) / TILE_SIZE;
	
	//Iterate over the tiles within the vertical range
	for (y = y0; y <= y1; ++y)
	{
		//One solid tile is sufficient
		if (IsTileSolid(GetTileIndex(x, y)))
			return true;
	}
	return false;
}
bool TileMap::CollisionY(const Point& p, int distance) const
{
	int x, y, x0, x1;
	Tile tile;

	//Calculate the tile coordinates and the range of tiles to check for collision
	y = p.y / TILE_SIZE;
	x0 = p.x / TILE_SIZE;
	x1 = (p.x + distance - 1) / TILE_SIZE;

	//Iterate over the tiles within the horizontal range
	for (x = x0; x <= x1; ++x)
	{
		tile = GetTileIndex(x, y);

		if (IsTileSolid(tile))
			return true;
	}
	return false;
}

AABB TileMap::GetSweptAreaX(const AABB& hitbox) const
{
	AABB box;
	int column, x, y, y0, y1;
	bool collision;

	box.pos.y = hitbox.pos.y;
	box.height = hitbox.height;

	column = hitbox.pos.x / TILE_SIZE;
	y0 = hitbox.pos.y / TILE_SIZE;
	y1 = (hitbox.pos.y + hitbox.height - 1) / TILE_SIZE;

	//Compute left tile index
	collision = false;
	x = column - 1;
	while (!collision && x >= 0)
	{
		//Iterate over the tiles within the vertical range
		for (y = y0; y <= y1; ++y)
		{
			//One solid tile is sufficient
			if (IsTileSolid(GetTileIndex(x, y)))
			{
				collision = true;
				break;
			}
		}
		if (!collision) x--;
	}
	box.pos.x = (x + 1) * TILE_SIZE;

	//Compute right tile index
	collision = false;
	x = column + 1;
	while (!collision && x < LEVEL_WIDTH)
	{
		//Iterate over the tiles within the vertical range
		for (y = y0; y <= y1; ++y)
		{
			//One solid tile is sufficient
			if (IsTileSolid(GetTileIndex(x, y)))
			{
				collision = true;
				break;
			}
		}
		if (!collision) x++;
	}
	box.width = x * TILE_SIZE - box.pos.x;

	return box;
}

void TileMap::Render()
{
	Tile tile;
	Rectangle rc;
	Vector2 pos;

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			tile = map[i * width + j];
			if (tile != Tile::AIR)
			{
				pos.x = (float)j * TILE_SIZE;
				pos.y = (float)i * TILE_SIZE;

				rc = dict_rect[(int)tile];
				DrawTextureRec(*img_tiles, rc, pos, WHITE);
			}
		}
	}
}
void TileMap::Release()
{
	ResourceManager& data = ResourceManager::Instance(); 
	data.ReleaseTexture(Resource::IMG_TILES);

	dict_rect.clear();
}