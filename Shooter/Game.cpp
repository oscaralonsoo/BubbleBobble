#include "Game.h"
#include "Globals.h"
#include <stdio.h>

Game::Game()
{
    state = GameState::MAIN_MENU;

    level = nullptr;
    img_menu = {};
    img_settings = {};
    img_credits = {};
}
Game::~Game()
{
    if (level != nullptr)
    {
        level->Release();
        delete level;
        level = nullptr;
    }
}
bool Game::Initialise()
{
    //Initialise windows
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spaceship: arrow keys + space, f1: god mode");

    //Load resources
    if (!LoadTextures())
    {
        LOG("Failed to load textures");
        return false;
    }

    //Set the target frame rate for the application
    SetTargetFPS(60);

    return true;
}
bool Game::LoadTextures()
{
    img_menu = LoadTexture("images/menu.png");
    if (img_menu.id == 0)
    {
        LOG("Failed to load texture images/menu.png");
        return false;
    }

    img_settings = LoadTexture("images/settings.png");
    if (img_settings.id == 0)
    {
        LOG("Failed to load texture images/settings.png");
        return false;
    }

    img_credits = LoadTexture("images/credits.png");
    if (img_credits.id == 0)
    {
        LOG("Failed to load texture images/credits.png");
        return false;
    }

    return true;
}
bool Game::BeginPlay()
{
    level = new Level(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (level == nullptr)
    {
        LOG("Failed to allocate memory for Level");
        return false;
    }
    if (!level->Init())
    {
        LOG("Failed to initialise Level");
        return false;
    }

    return true;
}
void Game::FinishPlay()
{
    level->Release();
    delete level;
    level = nullptr;
}

bool Game::Update()
{
    switch (state)
    {
    case GameState::MAIN_MENU:
        if (IsKeyPressed(KEY_UP) && stateIndex > 0)
        {
            stateIndex -= 1;
        }
        if (IsKeyPressed(KEY_DOWN) && stateIndex < 3)
        {
            stateIndex += 1;
        }
        if (IsKeyPressed(KEY_SPACE))
        {
            switch (stateIndex)
            {
            case 0:
                if (!BeginPlay()) return false;
                state = GameState::PLAYING;
                break;
            case 1:
                if (!BeginPlay()) return false;
                state = GameState::SETTINGS;
                break;
            case 2:
                if (!BeginPlay()) return false;
                state = GameState::CREDITS;
                break;
            case 3:
                return false;
            default:
                break;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) return false;

        break;

    case GameState::PLAYING:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            FinishPlay();
            state = GameState::MAIN_MENU;
        }
        else
        {
            //Process Input
            level->HandleInputPlayer();
            //Game logic
            level->Update();
        }
        break;

    case GameState::SETTINGS:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            state = GameState::MAIN_MENU;
        }
        break;
    case GameState::CREDITS:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            state = GameState::MAIN_MENU;
        }
        break;
    }
    return true;
}

void Game::Render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    switch (state)
    {
    case GameState::MAIN_MENU:
        //Background image
        DrawTexture(img_menu, 0, 0, WHITE);
        //Cursor
        DrawRectangle(BUTTON_PLAY_POS_X, BUTTON_PLAY_POS_Y + (stateIndex * CURSOR_HEIGHT), CURSOR_WIDTH, CURSOR_HEIGHT, CURSOR_COLOR);
        break;

    case GameState::SETTINGS:
        //Background image
        DrawTexture(img_settings, 0, 0, WHITE);
        break;

    case GameState::PLAYING:
        level->Render();
        break;

    case GameState::CREDITS:
        //Background image
        DrawTexture(img_credits, 0, 0, WHITE);
        break;
    }

    EndDrawing();
}
void Game::Cleanup()
{
    UnloadTextures();
    CloseWindow();
}
void Game::UnloadTextures()
{
    UnloadTexture(img_menu);
    UnloadTexture(img_settings);
    UnloadTexture(img_credits);
}