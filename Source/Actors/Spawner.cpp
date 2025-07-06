//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Hero.h"
#include "Enemy.h"

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime)
{
    if (abs(GetGame()->GetHero()->GetPosition().x - GetPosition().x) < mSpawnDistance)
    {
        auto enemy = new Enemy(GetGame());
        enemy->SetPosition(GetPosition()  - Vector2(Game::TILE_SIZE, Game::TILE_SIZE));
        mState = ActorState::Destroy;
    }
}