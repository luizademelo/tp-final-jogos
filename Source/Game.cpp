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
#include "CSV.h"
#include "Random.h"
#include "Game.h"
#include "Actors/Actor.h"
#include "Actors/Mario.h"
#include "Actors/Block.h"
#include "Actors/Spawner.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mUpdatingActors(false)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("TP3: Super Mario Bros", 0, 0, mWindowWidth, mWindowHeight, 0);
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

    Random::Init();

    mTicksCount = SDL_GetTicks();

    // Init all game actors
    InitializeActors();

    return true;
}

void Game::InitializeActors()
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 7.1 (~1 linha): Utilize a função LoadLevel para carregar o primeiro nível (Level1.txt) do jogo.
    //  Note que a classe Game tem constantes LEVEL_WIDTH e LEVEL_HEIGHT que definem a largura e altura
    mLevelData = LoadLevel("../Assets/Levels/Level1-1/level1-1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);

    // TODO 7.2 (~4 linhas): Verifique se a matriz de tiles foi carregada corretamente. Se não, retorne.
    //  Se foi, chame a função BuildLevel passando a matriz de tiles, a largura e altura do nível.
    if (!mLevelData) {
        SDL_Log("Failed to load level data");
        return;
    }

    BuildLevel(mLevelData, LEVEL_WIDTH, LEVEL_HEIGHT);
}

void Game::BuildLevel(int** levelData, int width, int height)
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 6: Implemente o método BuildLevel para percorrer a matriz de tiles carregada no item anterior e instanciar
    //  game objects para o mario, os canos e os blocos. O Mario deve ser instanciado na posição
    //  correspondente ao tile 16. Os blocos devem ser instanciados na posição correspondente aos tiles
    //  0, 1, 2, 4, 6, 8, 9 e 12. Utilize a função SetPosition para definir a posição de cada game object.

    // Const map to convert tile ID to block type
    const std::map<int, std::string> tileMap = {
        {0, "../Assets/Sprites/Blocks/BlockA.png"},
        {1, "../Assets/Sprites/Blocks/BlockC.png"},
        {2, "../Assets/Sprites/Blocks/BlockF.png"},
        {4, "../Assets/Sprites/Blocks/BlockB.png"},
        {6, "../Assets/Sprites/Blocks/BlockI.png"},
        {8, "../Assets/Sprites/Blocks/BlockD.png"},
        {9, "../Assets/Sprites/Blocks/BlockH.png"},
        {12, "../Assets/Sprites/Blocks/BlockG.png"}
    };

    for (int y = 0; y < LEVEL_HEIGHT; ++y) {
        for (int x = 0; x < LEVEL_WIDTH; ++x) {
            int tile = mLevelData[y][x];

          if(tile == 16) {
                // Mario
                mMario = new Mario(this);
                mMario->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
          }
          else if(tile == 10) {
                // Spawner
                Spawner* spawner = new Spawner(this, SPAWN_DISTANCE);
                spawner->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
          }
          else {
                // Blocks
                auto it = tileMap.find(tile);
                if (it != tileMap.end()) {
                    Block* block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }

    // --------------
    // TODO - PARTE 5
    // --------------

    // TODO 1: Modifique o método BuildLevel para instanciar spawners na posição correspondente ao tile 10.
    //  Assim como os outros objetos, posicione os spawnwers no mundo conforme as coordenadas i e j na matriz
    //  de tiles. Note que a classe game tem uma constante `SPAWN_DISTANCE = 600` que você pode usar para
    //  configurar a distância de gatilho do spawner.
}

int **Game::LoadLevel(const std::string& fileName, int width, int height)
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 5: Implemente essa função para carregar o nível a partir do arquivo CSV. Ela deve retornar um
    //  ponteiro para uma matriz 2D de inteiros. Cada linha do arquivo CSV representa uma linha
    //  do nível. Cada número inteiro representa o tipo de bloco que deve ser criado. Utilize a função CSVHelper::Split
    //  para dividir cada linha do arquivo CSV em números inteiros. A função deve retornar nullptr se o arquivo não
    //  puder ser carregado ou se o número de colunas for diferente do esperado.
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
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
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
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

    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
}

void Game::UpdateCamera()
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1 (~5 linhas): Calcule a posição horizontal da câmera subtraindo metade da largura da janela
    //  da posição horizontal do jogador. Isso fará com que a câmera fique sempre centralizada no jogador.
    //  No SMB, o jogador não pode voltar no nível, portanto, antes de atualizar
    //  a posição da câmera, verifique se a posição horizontal calculada é maior do que a posição horizontal
    //  atual da câmera. Além disso, limite a posição horizontal para que a câmera fique entre 0 e o limite
    //  horizontal máximo do nível. Para calcular o limite horizontal máximo do nível, utilize as constantes
    //  `LEVEL_WIDTH` e `TILE_SIZE`.
    float horizontalCameraPos = mMario->GetPosition().x - (mWindowWidth / 2.0f);

    if (horizontalCameraPos > mCameraPos.x) {
        // Limite a posição da câmera para que ela não ultrapasse o limite superior do nível
        float maxCameraPos = (LEVEL_WIDTH * TILE_SIZE) - mWindowWidth;
        horizontalCameraPos = Math::Clamp(horizontalCameraPos, 0.0f, maxCameraPos);

        mCameraPos.x = horizontalCameraPos;
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Set draw color to black
    SDL_SetRenderDrawColor(mRenderer, 107, 140, 255, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    for (auto drawable : mDrawables)
    {
        if (drawable->IsVisible())
        {
            drawable->Draw(mRenderer);
        }
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath) {
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 4.1 (~4 linhas): Utilize a função `IMG_Load` para carregar a imagem passada como parâmetro
    //  `texturePath`. Esse função retorna um ponteiro para `SDL_Surface*`. Retorne `nullptr` se a
    //  imagem não foi carregada com sucesso.
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    // TODO 4.2 (~6 linhas): Utilize a função `SDL_CreateTextureFromSurface` para criar uma textura a partir
    //  da imagem carregada anteriormente. Essa função retorna um ponteiro para `SDL_Texture*`. Logo após criar
    //  a textura, utilize a função `SDL_FreeSurface` para liberar a imagem carregada. Se a textura foi carregada
    //  com sucesso, retorne o ponteiro para a textura. Caso contrário, retorne `nullptr`.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

void Game::Shutdown()
{
    // Delete actors
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    // Delete level data
    for (int i = 0; i < LEVEL_HEIGHT; ++i) {
        delete[] mLevelData[i];
    }
    delete[] mLevelData;

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
