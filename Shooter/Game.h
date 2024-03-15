#pragma once
#include "Level.h"

enum class GameState { MAIN_MENU, PLAYING, SETTINGS, CREDITS };

class Game
{
public:
    Game();
    ~Game();

    bool Initialise();
    bool Update();
    void Render();
    void Cleanup();

private:
    bool BeginPlay();
    void FinishPlay();

    bool LoadTextures();
    void UnloadTextures();

    GameState state;
    Level *level;
    Texture2D img_menu;
    Texture2D img_settings;
    Texture2D img_credits;
    int stateIndex = 0;
};