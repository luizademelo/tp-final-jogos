//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Goomba.h"

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime)
{
    // --------------
    // TODO - PARTE 5
    // --------------

    // TODO 6 (~4-6 linhas): Verifique se a distância horizontal entre o jogador e esse
    //  objeto spawner é menor do que `mSpawnDistance`. Se for, crie um novo goomba e altere sua posição
    //  para ser igual a posição desse spawner. Por fim, destrua esse objeto spawner.
    if (abs(GetGame()->GetMario()->GetPosition().x - GetPosition().x) < mSpawnDistance) {
        auto goomba = new Goomba(GetGame());
        goomba->SetPosition(GetPosition());
        mState = ActorState::Destroy;
    }
}