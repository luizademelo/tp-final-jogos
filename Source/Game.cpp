// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "CSV.h"
#include "Random.h"
#include "Game.h"
#include "HUD.h"
#include "SpatialHashing.h"
#include "Actors/Actor.h"
#include "Actors/Hero.h"
#include "Actors/Block.h"
#include "Actors/Coffee.h"
#include "Actors/Spawner.h"
#include "UIElements/UIScreen.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/DrawComponents/DrawSpriteComponent.h"
#include "Components/DrawComponents/DrawPolygonComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mHero(nullptr)
        ,mHUD(nullptr)
        ,mBackgroundColor(0, 0, 0)
        ,mModColor(255, 255, 255)
        ,mCameraPos(Vector2::Zero)
        ,mAudio(nullptr)
        ,mGameTimer(0.0f)
        ,mGameTimeLimit(0)
        ,mSceneManagerTimer(0.0f)
        ,mSceneManagerState(SceneManagerState::None)
        ,mGameScene(GameScene::MainMenu)
        ,mNextScene(GameScene::Level1)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundSize(Vector2::Zero)
        ,mBackgroundPosition(Vector2::Zero)
        ,mMenuCursorIndex(0)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("TP4: Super Hero Bros", 100, 100, mWindowWidth, mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Start random number generator
    Random::Init();

    // Initialize game systems
    mAudio = new AudioSystem();

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    // Init all game actors
    SetGameScene(GameScene::MainMenu);

    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime)
{
    // Scene Manager FSM: using if/else instead of switch
    if (mSceneManagerState == SceneManagerState::None)
    {
        if (scene == GameScene::MainMenu || scene == GameScene::Level1 || scene == GameScene::Level2 || scene == GameScene::Level3 || scene == GameScene::Level4 ||
            scene == GameScene::HowToPlay || scene == GameScene::GameOver || scene == GameScene::Victory)
        {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
        }
        else {
            SDL_Log("Invalid game scene: %d", static_cast<int>(scene));
            return;
        }
    }
    else
    {
        SDL_Log("Scene Manager is already in a transition state");
        return;
    }
}

void Game::ResetGameScene(float transitionTime)
{
    SetGameScene(mGameScene, transitionTime);
}


void Game::LoadVictoryScreen()
{
    UIScreen* victoryScreen = new UIScreen(this, "../Assets/Fonts/SMB.ttf");
    
    // victoryScreen->AddImage("../Assets/Sprites/VictoryScreen.png",
    //                       Vector2(0, 0),
    //                       Vector2(mWindowWidth, mWindowHeight));
    SetBackgroundImage("../Assets/Sprites/VictoryScreen.png", Vector2(0, 0), Vector2(mWindowWidth, mWindowHeight));
    
    victoryScreen->AddButton("Voltar ao Menu", 
                           Vector2(mWindowWidth/2.0f - 150.0f, 600),
                           Vector2(300.0f, 40.0f),
                           [this]() { 
                               SetGameScene(GameScene::MainMenu); 
                           });
    
    mAudio->PlaySound("victory.wav");
}

void Game::LoadGameOverScreen()
{
    UIScreen* gameOverScreen = new UIScreen(this, "../Assets/Fonts/SMB.ttf");

    SetBackgroundImage("../Assets/Sprites/DefeatScreen.png",
                          Vector2(0, 0),
                          Vector2(mWindowWidth, mWindowHeight));
    
    gameOverScreen->AddButton("Voltar ao Menu", 
                           Vector2(mWindowWidth/2.0f - 150.0f, 600),
                           Vector2(300.0f, 40.0f),
                           [this]() { 
                               SetGameScene(GameScene::MainMenu); 
                           });
    
    mAudio->PlaySound("defeat_chime.wav");
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset game timer
    mGameTimer = 0.0f;

    // Reset gameplau state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu)
    {
        // Reset menu cursor to first option
        mMenuCursorIndex = 0;
        
        // Set background color
        mBackgroundColor.Set(0.0f, 0.0f, 0.0f);

        // Set background image
        SetBackgroundImage("../Assets/Sprites/LogoBackground.png", Vector2(0,0), Vector2(mWindowWidth, mWindowHeight));

        // Initialize main menu actors
        LoadMainMenu();
    }

    else if (mNextScene == GameScene::HowToPlay) {
        mBackgroundColor.Set(0.0f, 0.0f, 0.0f);
        LoadHowToPlay();
    }

    else if (mNextScene == GameScene::Level1)
    {
        // Start Music
        mMusicHandle = mAudio->PlaySound("level1_background.wav", true);

        // Create HUD
        mHUD = new HUD(this, "../Assets/Fonts/Movistar.ttf");

        // Reset HUD
        mGameTimeLimit = 400;
        mHUD->SetTime(mGameTimeLimit);
        mHUD->SetLevelName("Nivel 1");

        SetStairsPosition(5680);

        // Set background
        SetBackgroundImage("../Assets/Sprites/BackgroundLevel1.png", Vector2(0,0), Vector2(6000,600));

        // Initialize actors
        LoadLevel("../Assets/Levels/level1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }

    else if (mNextScene == GameScene::Level2)
    {
        // Start Music
        mAudio->StopSound(mMusicHandle);
        mMusicHandle = mAudio->PlaySound("level2_background.wav", true);

        // Create HUD
        mHUD = new HUD(this, "../Assets/Fonts/Movistar.ttf");

        // Reset HUD
        mGameTimeLimit = 400;
        mHUD->SetTime(mGameTimeLimit);
        mHUD->SetLevelName("Nivel 2");

        SetStairsPosition(5680);

        // Set background
        SetBackgroundImage("../Assets/Sprites/BackgroundLevel2.png", Vector2(0,0), Vector2(6000,600));

        // Initialize actors
        LoadLevel("../Assets/Levels/level2.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }

    else if (mNextScene == GameScene::Level3)
    {
        // Start Music
        mAudio->StopSound(mMusicHandle);
        mMusicHandle = mAudio->PlaySound("level3_background.wav", true);

        // Create HUD
        mHUD = new HUD(this, "../Assets/Fonts/Movistar.ttf");

        // Reset HUD
        mGameTimeLimit = 400;
        mHUD->SetTime(mGameTimeLimit);
        mHUD->SetLevelName("Nivel 3");

        SetStairsPosition(5680);

        // Set background
        SetBackgroundImage("../Assets/Sprites/BackgroundLevel3.png", Vector2(0,0), Vector2(6000,600));

        // Initialize actors
        LoadLevel("../Assets/Levels/level3.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }

    else if (mNextScene == GameScene::Level4)
    {
        // Start Music
        mAudio->StopSound(mMusicHandle);
        mMusicHandle = mAudio->PlaySound("level4_background.wav", true);

        // Create HUD
        mHUD = new HUD(this, "../Assets/Fonts/Movistar.ttf");

        // Reset HUD
        mGameTimeLimit = 400;
        mHUD->SetTime(mGameTimeLimit);
        mHUD->SetLevelName("Nivel 4");

        SetStairsPosition(5680);

        // Set background
        SetBackgroundImage("../Assets/Sprites/BackgroundLevel4.png", Vector2(0,0), Vector2(6000,600));

        // Initialize actors
        LoadLevel("../Assets/Levels/level4.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    }

    else if (mNextScene == GameScene::Victory)
    {
        // Load victory screen
        LoadVictoryScreen();
    }

    else if (mNextScene == GameScene::GameOver)
    {
        // Load game over screen
        LoadGameOverScreen();
    }

    // Set new scene
    mGameScene = mNextScene;
}

void Game::LoadMainMenu()
{
    // Load font
    UIScreen* mainMenu = new UIScreen(this, "../Assets/Fonts/SMB.ttf");

    // Add menu buttons side by side at the bottom
    const Vector2 buttonSize = Vector2(180.0f, 40.0f);
    const float buttonSpacing = 60.0f; // Increased spacing between buttons
    const float totalButtonWidth = (buttonSize.x * 2) + buttonSpacing;
    const Vector2 button1Pos = Vector2(mWindowWidth/2.0f - totalButtonWidth/2.0f, mWindowHeight - 150.0f);
    const Vector2 button2Pos = Vector2(button1Pos.x + buttonSize.x + buttonSpacing, button1Pos.y);

    // Add text labels instead of buttons to avoid background styling
    mainMenu->AddText("Iniciar", Vector2(button1Pos.x + 20.0f, button1Pos.y + 10.0f), Vector2(buttonSize.x - 40.0f, 20.0f));
    mainMenu->AddText("Como Jogar", Vector2(button2Pos.x + 20.0f, button2Pos.y + 10.0f), Vector2(buttonSize.x - 40.0f, 20.0f));

    // Add cursor indicators
    mainMenu->AddText(mMenuCursorIndex == 0 ? ">" : " ", Vector2(button1Pos.x - 30.0f, button1Pos.y + 10.0f), Vector2(20.0f, 20.0f));
    mainMenu->AddText(mMenuCursorIndex == 1 ? ">" : " ", Vector2(button2Pos.x - 30.0f, button2Pos.y + 10.0f), Vector2(20.0f, 20.0f));

}

void Game::LoadHowToPlay() {
    UIScreen* menu = new UIScreen(this, "../Assets/Fonts/SMB.ttf");
    menu->AddText("A -> Andar para esquerda", Vector2((GetWindowWidth() / 2) - 150, 100), Vector2(300, 40));
    menu->AddText("D -> Andar para direita", Vector2((GetWindowWidth() / 2) - 150, 200), Vector2(300, 40));
    menu->AddText("Espaço -> Pular", Vector2((GetWindowWidth() / 2) - 150, 300), Vector2(300, 40));
    menu->AddText("X -> Atirar", Vector2((GetWindowWidth() / 2) - 150, 400), Vector2(300, 40));

    menu->AddButton("Voltar", Vector2((GetWindowWidth() / 2) - 150, 600), Vector2(300, 40), [this]() {SetGameScene(GameScene::MainMenu);});
}


void Game::LoadLevel(const std::string& levelName, const int levelWidth, const int levelHeight)
{
    // Load level data
    int **mLevelData = ReadLevelData(levelName, levelWidth, levelHeight);

    if (!mLevelData) {
        SDL_Log("Failed to load level data");
        return;
    }

    // Instantiate level actors
    BuildLevel(mLevelData, levelWidth, levelHeight);
}

void Game::BuildLevel(int** levelData, int width, int height)
{

    // Const map to convert tile ID to block type
    const std::map<int, const std::string> tileMap = {
        {10, "../Assets/Sprites/Blocks/Floor.png"},
        {11, "../Assets/Sprites/Blocks/Level1/chair.png"},
        {12, "../Assets/Sprites/Blocks/Level1/filingCabinet.png"},
        {13, "../Assets/Sprites/Blocks/Level1/plant.png"},
        {14, "../Assets/Sprites/Blocks/Level1/workTable.png"},
        {15, "../Assets/Sprites/Blocks/Level2/coffeeMaker.png"},
        {16, "../Assets/Sprites/Blocks/Level2/fountain.png"},
        {17, "../Assets/Sprites/Blocks/Level2/picnicTable.png"},
        {18, "../Assets/Sprites/Blocks/Level2/vendingMachine.png"},
        {19, "../Assets/Sprites/Blocks/Level2/wasteCan.png"},
        {20, "../Assets/Sprites/Blocks/Level3/desktop.png"},
        {21, "../Assets/Sprites/Blocks/Level3/netcape.png"},
        {22, "../Assets/Sprites/Blocks/Level3/server.png"},
        {23, "../Assets/Sprites/Blocks/Level3/toolbox.png"},
        {24, "../Assets/Sprites/Blocks/Level4/couch.png"},
        {25, "../Assets/Sprites/Blocks/Level4/dumpster.png"},
        {26, "../Assets/Sprites/Blocks/Level4/IdCounter.png"},
        {27, "../Assets/Sprites/Blocks/Level4/ratchet.png"},
    };

    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < LEVEL_WIDTH; ++x)
        {
            int tile = levelData[y][x];

            if(tile == 99) // Hero
            {
                mHero = new Hero(this);
                mHero->SetPosition(Vector2(x * TILE_SIZE - TILE_SIZE, y * TILE_SIZE - TILE_SIZE));
            }
            else if(tile == 98) // Spawner
            {
                Spawner* spawner = new Spawner(this, SPAWN_DISTANCE);
                spawner->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 97) // Collectable
            {
                Coffee* coffee = new Coffee(this);
                coffee->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else // Blocks
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    // Create a block actor
                    Block* block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }
}

int **Game::ReadLevelData(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i) {
        levelData[i] = new int[width];
    }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof()) {
        std::getline(file, line);

        if(!line.empty()) {
            auto tiles = CSVHelper::Split(line);

            if (tiles.size() != width) {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) {
                levelData[row][i] = tiles[i];
            }
        }

        ++row;
    }

    // Close the file
    file.close();

    return levelData;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                // Handle main menu navigation
                if (mGameScene == GameScene::MainMenu)
                {
                    if (event.key.keysym.sym == SDLK_a)
                    {
                        // Move cursor left
                        mMenuCursorIndex = 0;
                        UpdateMainMenuCursor();
                    }
                    else if (event.key.keysym.sym == SDLK_d)
                    {
                        // Move cursor right
                        mMenuCursorIndex = 1;
                        UpdateMainMenuCursor();
                    }
                    else if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        // Execute selected option
                        if (mMenuCursorIndex == 0)
                        {
                            SetGameScene(GameScene::Level1);
                        }
                        else if (mMenuCursorIndex == 1)
                        {
                            SetGameScene(GameScene::HowToPlay);
                        }
                    }
                }
                
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym, event.key.repeat == 0);

                // Check if the Return key has been pressed to pause/unpause the game
                if (event.key.keysym.sym == SDLK_RETURN && mGameScene != GameScene::MainMenu)
                {
                    TogglePause();
                }
                break;
        }
    }

    ProcessInputActors();
}

void Game::ProcessInputActors()
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isHeroOnCamera = false;
        for (auto actor: actorsOnCamera)
        {
            actor->ProcessInput(state);

            if (actor == mHero) {
                isHeroOnCamera = true;
            }
        }

        // If Hero is not on camera, process input for him
        if (!isHeroOnCamera && mHero) {
            mHero->ProcessInput(state);
        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed)
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        // Handle key press for actors
        bool isMarioOnCamera = false;
        for (auto actor: actorsOnCamera) {
            actor->HandleKeyPress(key, isPressed);

            if (actor == mHero) {
                isMarioOnCamera = true;
            }
        }

        // If Hero is not on camera, handle key press for him
        if (!isMarioOnCamera && mHero) {
            mHero->HandleKeyPress(key, isPressed);
        }
    }

}

void Game::TogglePause()
{
    if (mGameScene != GameScene::MainMenu)
    {
        if (mGamePlayState == GamePlayState::Playing)
        {
            mGamePlayState = GamePlayState::Paused;
            mAudio->PlaySound("Click.mp3");
            mAudio->PauseSound(mMusicHandle);
        }
        else if (mGamePlayState == GamePlayState::Paused)
        {
            mGamePlayState = GamePlayState::Playing;
            mAudio->PlaySound("Click.mp3");
            mAudio->ResumeSound(mMusicHandle);
        }
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if(mGamePlayState != GamePlayState::Paused && mGamePlayState != GamePlayState::GameOver)
    {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }

    // Reinsert audio system
    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // ---------------------
    // Game Specific Updates
    // ---------------------
    if(mGameScene != GameScene::MainMenu && mGameScene != GameScene::HowToPlay && mGameScene != GameScene::GameOver && mGameScene != GameScene::Victory && mGamePlayState == GamePlayState::Playing)
    {
        // Reinsert level time
        UpdateLevelTime(deltaTime);

        UpdateLivesCount(mHero->GetLivesCount());
        mHUD->SetPowerUp(mHero->HasPowerUp());
    }

    UpdateSceneManager(deltaTime);
    UpdateCamera();
}

void Game::UpdateSceneManager(float deltaTime)
{
    if(mSceneManagerState == SceneManagerState::Entering)
    {
        mSceneManagerTimer -= deltaTime;
        if(mSceneManagerTimer <= 0.0f)
        {
            mSceneManagerTimer = TRANSITION_TIME;
            mSceneManagerState = SceneManagerState::Active;
        }
    }
    else if(mSceneManagerState == SceneManagerState::Active)
    {
        mSceneManagerTimer -= deltaTime;
        if(mSceneManagerTimer <= 0.0f)
        {
            ChangeScene();
            mSceneManagerState = SceneManagerState::None;
        }
    }
}

void Game::UpdateLivesCount(float livesCount) {
    mHUD->SetLifeCount(livesCount);
}


void Game::UpdateLevelTime(float deltaTime)
{
    // Reinsert game timer
    mGameTimer += deltaTime;
    if (mGameTimer >= 0.5f)
    {
        mGameTimer = 0.0f;
        mGameTimeLimit--;

        if (mGameTimeLimit > 0) {
            mHUD->SetTime(mGameTimeLimit);
        }
        else
        {
            // Kill Hero if time limit is reached
            // mHUD->SetTime(mGameTimeLimit);
            // mHero->Kill();
            SetGameScene(GameScene::GameOver, 1.0f);
            mSceneManagerState = SceneManagerState::Exiting;

        }
    }
}

void Game::UpdateCamera()
{
    if (!mHero) return;

    float horizontalCameraPos = mHero->GetPosition().x - (mWindowWidth / 2.0f);
    //float verticalCameraPos = mHero ->GetPosition().y - (mWindowHeight / 2.0f);

    if (horizontalCameraPos > mCameraPos.x)
    {
        // Limit camera to the right side of the level
        float maxCameraPos = (LEVEL_WIDTH * TILE_SIZE) - mWindowWidth;
        horizontalCameraPos = Math::Clamp(horizontalCameraPos, 0.0f, maxCameraPos);

        mCameraPos.x = horizontalCameraPos;
    }
    // if (verticalCameraPos > mCameraPos.y) {
    //     mCameraPos.y = verticalCameraPos;
    // }
}

void Game::UpdateActors(float deltaTime)
{
    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
        mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    bool isMarioOnCamera = false;
    for (auto actor : actorsOnCamera)
    {
        actor->Update(deltaTime);
        if (actor == mHero) {
            isMarioOnCamera = true;
        }
    }

    // If Hero is not on camera, update him (player should always be updated)
    if (!isMarioOnCamera && mHero) {
         mHero->Update(deltaTime);
    }

    for (auto actor : actorsOnCamera)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            delete actor;
            if (actor == mHero) {
                mHero = nullptr;
            }
        }
    }
}

void Game::AddActor(Actor* actor)
{
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor)
{
    mSpatialHashing->Remove(actor);
}
void Game::Reinsert(Actor* actor)
{
    mSpatialHashing->Reinsert(actor);
}

std::vector<Actor *> Game::GetNearbyActors(const Vector2& position, const int range)
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent *> Game::GetNearbyColliders(const Vector2& position, const int range)
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y, mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    // Draw background texture considering camera position
    if (mBackgroundTexture)
    {
        SDL_Rect dstRect = { static_cast<int>(mBackgroundPosition.x - mCameraPos.x),
                             static_cast<int>(mBackgroundPosition.y - mCameraPos.y),
                             static_cast<int>(mBackgroundSize.x),
                             static_cast<int>(mBackgroundSize.y) };

        SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &dstRect);
    }

    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera)
    {
        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    // Draw all drawables
    for (auto drawable : drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    // Draw all UI screens
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    // Draw transition rect
    if (mSceneManagerState == SceneManagerState::Active)
    {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
        SDL_Rect rect = { 0, 0, mWindowWidth, mWindowHeight };
        SDL_RenderFillRect(mRenderer, &rect);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string& texturePath, const Vector2 &position, const Vector2 &size)
{
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}


UIFont* Game::LoadFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        return iter->second;
    }
    else
    {
        UIFont* font = new UIFont(mRenderer);
        if (font->Load(fileName))
        {
            mFonts.emplace(fileName, font);
        }
        else
        {
            font->Unload();
            delete font;
            font = nullptr;
        }
        return font;
    }
}

void Game::UnloadScene()
{
    // Delete actors
    delete mSpatialHashing;

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    UnloadScene();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    delete mAudio;
    mAudio = nullptr;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::UpdateMainMenuCursor()
{
    if (mGameScene == GameScene::MainMenu && !mUIStack.empty())
    {
        // Remove the current menu and recreate it with updated cursor
        delete mUIStack.back();
        mUIStack.pop_back();
        LoadMainMenu();
    }
}
