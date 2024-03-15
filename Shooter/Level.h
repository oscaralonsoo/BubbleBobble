#pragma once
#include <raylib.h>
#include "Entity.h"

//Velocities
#define PLAYER_SPEED    8
#define SCENE_SPEED     2
#define SHOOT_SPEED     16

//Maximum number of shots on screen
#define MAX_SHOTS       32

//Offset for positioning shots
#define SHOT_POS_X      29
#define SHOT_POS_Y1     3
#define SHOT_POS_Y2     59

class Level
{
public:
    Level(int width, int height);
    ~Level();

    bool Init();
    void HandleInputPlayer();
    void Update();
    void Render();
    void Release();

private:
    bool LoadTextures();
    void UnloadTextures();

private:
    int window_width, window_height;
    Entity *Player, *Shots[MAX_SHOTS], *Scene;
    int idx_shot;

    bool god_mode;

    Texture2D img_background;
    Texture2D img_player;
    Texture2D img_shot;
};

