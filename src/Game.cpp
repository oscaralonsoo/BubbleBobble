#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <stdio.h>

Game::Game()
{
    state = GameState::MAIN_TITLE;
    scene = nullptr;
    img_menu = nullptr;
    img_title = nullptr;


    target = {};
    src = {};
    dst = {};
}
Game::~Game()
{
    if (scene != nullptr)
    {
        scene->Release();
        delete scene;
        scene = nullptr;
    }
}
AppStatus Game::Initialise(float scale)
{
    float w, h;
    w = WINDOW_WIDTH * scale;
    h = WINDOW_HEIGHT * scale;

    //Initialise window
    InitWindow((int)w, (int)h, "Bubble Bobble");

    //Initialise audio
    /*InitAudioDevice();
    Sound  music = LoadSound("Music/introduction.ogg");
    PlaySound(music);*/

    //Render texture initialisation, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (target.id == 0)
    {
        LOG("Failed to create render texture");
        return AppStatus::ERROR;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    src = { 0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT };
    dst = { 0, 0, w, h };

    //Load resources
    if (LoadResources() != AppStatus::OK)
    {
        LOG("Failed to load resources");
        return AppStatus::ERROR;
    }

    //Initialise the fade in effect
    fade_transition.Set(GameState::MAIN_TITLE, 60, dst);

    //Set the target frame rate for the application
    SetTargetFPS(60);
    //Disable the escape key to quit functionality
    SetExitKey(0);

    return AppStatus::OK;
}
AppStatus Game::LoadResources()
{
    ResourceManager& data = ResourceManager::Instance();

    if (data.LoadTexture(Resource::IMG_TITLE, "images/titlescreen.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_title = data.GetTexture(Resource::IMG_TITLE);

    if (data.LoadTexture(Resource::IMG_LOOSE, "images/gameover.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_loose = data.GetTexture(Resource::IMG_LOOSE);
    
    if (data.LoadTexture(Resource::IMG_MENU, "images/menu.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_menu = data.GetTexture(Resource::IMG_MENU);
    
    return AppStatus::OK;

}
AppStatus Game::BeginPlay()
{
    scene = new Scene();
    if (scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return AppStatus::ERROR;
    }
    if (scene->Init() != AppStatus::OK)
    {
        LOG("Failed to initialise Scene");
        return AppStatus::ERROR;
    }

    return AppStatus::OK;
}
void Game::FinishPlay()
{
    scene->Release();
    delete scene;
    scene = nullptr;
}
AppStatus Game::Update()
{
    //Check if user attempts to close the window, either by clicking the close button or by pressing Alt+F4
    if(WindowShouldClose()) return AppStatus::QUIT;

    if (fade_transition.IsActive())
    {
        GameState prev_frame = state;
        state = fade_transition.Update();

        //Begin play and finish play are delayed due to the fading transition effect
        if (prev_frame == GameState::MAIN_MENU && state == GameState::PLAYING)
        {
            if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
        }
        else if (prev_frame == GameState::PLAYING && state == GameState::MAIN_MENU)
        {
            FinishPlay();
        }
    }
    else 
    {
        switch (state)
        {
        case GameState::MAIN_TITLE:
            if (IsKeyPressed(KEY_ESCAPE)) return AppStatus::QUIT;
            if (IsKeyPressed(KEY_SPACE))
            {
                //"state = GameState::PLAYING;" but not until halfway through the transition
                fade_transition.Set(GameState::MAIN_TITLE, 60, GameState::MAIN_MENU, 60, dst);
            }
            break;

        case GameState::MAIN_MENU:
            if (IsKeyPressed(KEY_ESCAPE)) return AppStatus::QUIT;
            if (IsKeyPressed(KEY_SPACE))
            {
                if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
                state = GameState::PLAYING;
            }
            break;

        case GameState::PLAYING:
            if (IsKeyPressed(KEY_ESCAPE))
            {
                FinishPlay();
                state = GameState::MAIN_MENU;
            }
            else if (scene->player->GetLifes() <= 0)
            {
                FinishPlay();
                state = GameState::LOOSE;
            }
            else
            {
                //Game logic
                scene->Update();
            }
            break;
        case GameState::LOOSE:
            if (IsKeyPressed(KEY_ESCAPE)) return AppStatus::QUIT;
            if (IsKeyPressed(KEY_SPACE))
            {
                state = GameState::MAIN_MENU;
            }
            break;
        }
    }
    
    return AppStatus::OK;
}
void Game::Render()
{
    //Draw everything in the render texture, note this will not be rendered on screen, yet
    BeginTextureMode(target);
    ClearBackground(BLACK);
    
    switch (state)
    {
        case GameState::MAIN_TITLE:
            DrawTexture(*img_title, 0, 0, WHITE);
            break;

        case GameState::MAIN_MENU:
            DrawTexture(*img_menu, 0, 0, WHITE);
            break;

        case GameState::PLAYING:
            scene->Render();
            break;

        case GameState::LOOSE:
            DrawTexture(*img_loose, 0, 0, WHITE);
            break;
    }
    
    EndTextureMode();

    //Draw render texture to screen, properly scaled
    BeginDrawing();
    DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    if (fade_transition.IsActive()) fade_transition.Render();
    EndDrawing();
}
void Game::Cleanup()
{
    UnloadResources();
    CloseAudioDevice();
    CloseWindow();
}
void Game::UnloadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    data.ReleaseTexture(Resource::IMG_TITLE);
    data.ReleaseTexture(Resource::IMG_MENU);

    UnloadRenderTexture(target);
}