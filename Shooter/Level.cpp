#include "Level.h"
#include <stdio.h>
#include "Globals.h"

Level::Level(int width, int height)
{
    window_width = width;
    window_height = height;

    Player = nullptr;
    for (int i = 0; i < MAX_SHOTS; ++i)
    {
        Shots[i] = nullptr;
    }
    Scene = nullptr;

    idx_shot = 0;
    god_mode = false;

    img_background = {};
    img_player = {};
    img_shot = {};
}
Level::~Level()
{
    if (Player != nullptr)
    {
        delete Player;
        Player = nullptr;
    }
    for (int i = 0; i < MAX_SHOTS; ++i)
    {
        if (Shots[i] != nullptr)
        {
            delete Shots[i];
            Shots[i] = nullptr;
        }
    }
    if (Scene != nullptr)
    {
        delete Scene;
        Scene = nullptr;
    }
}
bool Level::Init()
{
    //Load resources
    if (!LoadTextures())
    {
        LOG("Failed to load textures");
        return false;
    }

    //Initialise player
    Player = new Entity({ 20, window_height / 2.0f }, {0, 0}, 104, 82, PLAYER_SPEED);
    if (Player == nullptr)
    {
        LOG("Failed to allocate memory for Player");
        return false;
    }

    //Initialise shots
    idx_shot = 0;
    for (int i = 0; i < MAX_SHOTS; ++i)
    {
        Shots[i] = new Entity();
        if (Shots[i] == nullptr)
        {
            LOG("Failed to allocate memory for Shots[%d]", i);
            return false;
        }
    }

    //Initialise scene
    Scene = new Entity({ 0, 0 }, {-1, 0}, img_background.width, window_height, SCENE_SPEED);
    if (Scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return false;
    }

    //Initialise control variables
    god_mode = false;

    return true;
}
void Level::HandleInputPlayer()
{
    Vector2 p;
    Vector2 dir = { 0, 0 };

    if (IsKeyPressed(KEY_F1))       god_mode = !god_mode;
        
    if (IsKeyDown(KEY_UP))          dir.y = -1;
    else if (IsKeyDown(KEY_DOWN))   dir.y = 1;
    if (IsKeyDown(KEY_LEFT))        dir.x = -1;
    else if (IsKeyDown(KEY_RIGHT))  dir.x = 1;
    
    Player->SetDirection(dir);

    if (IsKeyPressed(KEY_SPACE))
    {
        p = Player->GetPosition();

        Shots[idx_shot]->Init({ p.x + SHOT_POS_X, p.y + SHOT_POS_Y1 }, {1, 0}, 56, 20, SHOOT_SPEED);
        idx_shot++;
        idx_shot %= MAX_SHOTS;

        Shots[idx_shot]->Init({ p.x + SHOT_POS_X, p.y + SHOT_POS_Y2 }, { 1, 0 }, 56, 20, SHOOT_SPEED);
        idx_shot++;
        idx_shot %= MAX_SHOTS;
    }
}
void Level::Update()
{
    Vector2 p;

    //Scene scroll
    Scene->Update();
    p = Scene->GetPosition();
    if (p.x <= -Scene->GetWidth()) Scene->SetPosition({ 0, p.y });

    //Player movement
    Player->Update();

    //Shots management
    for (int i = 0; i < MAX_SHOTS; ++i)
    {
        if (Shots[i]->IsAlive())
        {
            Shots[i]->Update();
            p = Shots[i]->GetPosition();
            if (p.x > window_width)
                Shots[i]->SetAlive(false);
        }
    }
}
void Level::Render()
{
    //Scene
    Scene->Render(img_background);
    Scene->Render(img_background, { (float)Scene->GetWidth(), 0 });

    //Player
    Player->Render(img_player);
    if (god_mode)   Player->RenderDebug(RED);

    //Shots
    for (int i = 0; i < MAX_SHOTS; ++i)
    {
        if (Shots[i]->IsAlive())
        {
            Shots[i]->Render(img_shot);
            if (god_mode)   Shots[i]->RenderDebug(RED);
        }
    }
}
bool Level::LoadTextures()
{
    img_background = LoadTexture("images/background.png");
    if (img_background.id == 0)
    {
        LOG("Failed to load texture images/background.png");
        return false;
    }
    img_player = LoadTexture("images/spaceship.png");
    if (img_player.id == 0)
    {
        LOG("Failed to load texture images/spaceship.png");
        return false;
    }
    img_shot = LoadTexture("images/shot.png");
    if (img_shot.id == 0)
    {
        LOG("Failed to load texture images/shot.png");
        return false;
    }

    return true;
}
void Level::Release()
{
    UnloadTextures();
}
void Level::UnloadTextures()
{
    UnloadTexture(img_background);
    UnloadTexture(img_player);
    UnloadTexture(img_shot);
}