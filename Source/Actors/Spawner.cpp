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
        Enemy* enemy = nullptr;
        
        // Determinar o tipo de inimigo com base no nível atual
        switch(GetGame()->GetGameScene())
        {
            case Game::GameScene::Level1:
                enemy = new Enemy(GetGame(), Enemy::ENEMY_TYPE1);
                break;
                
            case Game::GameScene::Level2:
                enemy = new Enemy(GetGame(), Enemy::ENEMY_TYPE2);
                break;
                
            case Game::GameScene::Level3:
                enemy = new Enemy(GetGame(), Enemy::ENEMY_TYPE3);
                break;
                
            case Game::GameScene::Level4:
                enemy = new Enemy(GetGame(), Enemy::ENEMY_TYPE4);
                break;
                
            default:
                enemy = new Enemy(GetGame(), Enemy::ENEMY_TYPE1);
                break;
        }
        
        if (enemy) {
            enemy->SetPosition(GetPosition() - Vector2(Game::TILE_SIZE, Game::TILE_SIZE));
        }
        
        mState = ActorState::Destroy;
    }
}