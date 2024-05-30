#pragma once
#include <raylib.h>
#include "Player.h"
#include "Bubble.h"
#include "TileMap.h"
#include "Lifes.h"
#include "EnemyManager.h"
#include "Object.h"
#include "BubbleManager.h"

enum class DebugMode { OFF, SPRITES_AND_HITBOXES, ONLY_HITBOXES, SIZE };

class Scene
{
public:
    Scene();
    ~Scene();

    AppStatus Init();
    void Update();
    void Render();
    void Release();

    Player* player;

private:
    AppStatus LoadLevel(int stage);

    void CheckObjectCollisions();
    void CheckEnemiesCollisions();
    void ClearLevel();
    void RenderObjects() const;
    void RenderObjectsDebug(const Color& col) const;

    void RenderGUI() const;

    //Enemies present in the level
    EnemyManager* enemies;

    //Shots thrown by player
    BubbleManager* bubbles;

    std::vector<Object*> objects;
    /*std::vector<Lifes*> lifes;*/
    TileMap* level;

    Camera2D camera;
    DebugMode debug;
};

