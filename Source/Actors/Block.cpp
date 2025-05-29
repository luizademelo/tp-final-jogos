//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Block.h"

#include <iostream>

#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include <unistd.h>
Block::Block(Game* game, const std::string &texturePath)
        :Actor(game)
{
    // --------------
    // TODO - PARTE 1
    // --------------

    // TODO 2 (1 linha): Crie o `DrawSpriteComponent` com textura `texturePath` (passada com parâmetro) e
    //  dimensões `(GAME::TILE_SIZE,GAME::TILE_SIZE)`.
    new DrawSpriteComponent(this, texturePath, Game::TILE_SIZE, Game::TILE_SIZE);

    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 6 (1 linha): Crie o `AABBColliderComponent` com dimensões `(0,0,GAME::TILE_SIZE, GAME::TILE_SIZE)`
    //  e tipo de colisão `ColliderLayer::Blocks`. Marque o collider como estático.
    new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks, true);

    mRigidBodyComponent = new RigidBodyComponent(this);
    mRigidBodyComponent->SetApplyGravity(false);
}

void Block::OnVerticalCollision(const float minOverlap, AABBColliderComponent *other) {
    if (this->type == BlockType::BRICK && other->GetOwner()->GetPosition().y >= this->GetPosition().y) {
        mOriginalPosition = this->GetPosition();
        mRigidBodyComponent->SetVelocity(Vector2(0, -100));
    }
}

void Block::OnUpdate(float deltaTime) {
    Vector2 pos = GetPosition();

    if (mRigidBodyComponent->GetVelocity().y < 0 && pos.y <= mOriginalPosition.y - 10.0f) {
        mRigidBodyComponent->SetVelocity(Vector2(0, 100));
    } else if (mRigidBodyComponent->GetVelocity().y > 0 && pos.y >= mOriginalPosition.y) {
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
        SetPosition(mOriginalPosition);
    }
}


