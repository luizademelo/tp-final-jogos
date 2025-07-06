//
// Created by Pc on 06/07/2025.
//

#include "Coffee.h"

#include "Hero.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Coffee::Coffee(Game* game, const bool isStatic) : Actor(game) {
    mDrawComponent = new DrawAnimatedComponent(this,
                                                 "../Assets/Sprites/Collectables/Coffee/texture.png",
                                                 "../Assets/Sprites/Collectables/Coffee/texture.json");
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Coffee, isStatic);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
    mDrawComponent->AddAnimation("Static", {0});
    mDrawComponent->SetAnimation("Static");
    mDrawComponent->SetAnimFPS(5.0f);
    mColliderLayer = ColliderLayer::Coffee;
}

void Coffee::OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other) {

}


