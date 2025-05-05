//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Block.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Block::Block(Game* game, const std::string &texturePath)
        :Actor(game)
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 2 (1 linha): Crie o `DrawSpriteComponent` com textura `texturePath` (passada com parâmetro) e
    //  dimensões `(GAME::TILE_SIZE,GAME::TILE_SIZE)`.
    new DrawSpriteComponent(this, texturePath, Game::TILE_SIZE, Game::TILE_SIZE, 10);

    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 6 (1 linha): Crie o `AABBColliderComponent` com dimensões `(0,0,GAME::TILE_SIZE, GAME::TILE_SIZE)`
    //  e tipo de colisão `ColliderLayer::Blocks`. Marque o collider como estático.
    new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks, true);
}
