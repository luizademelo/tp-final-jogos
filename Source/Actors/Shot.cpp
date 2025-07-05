//
// Created by Pc on 28/06/2025.
//
#include "Shot.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Shot::Shot(Game* game, const Vector2& velocity, ColliderLayer layer, std::string spriteSheetPath, std::string spriteSheetData)
    : Actor(game)
{

    mDrawComponent = new DrawAnimatedComponent(this, spriteSheetPath, spriteSheetData);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(velocity);
    mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   Game::TILE_SIZE, Game::TILE_SIZE,
                                                   layer);
    mDrawComponent->AddAnimation("Normal", {0});
    mDrawComponent->SetAnimation("Normal");
    mDrawComponent->SetAnimFPS(5.0f);
    mColliderLayer = layer;
}

void Shot::OnUpdate(float deltaTime)
{
    mTimer += deltaTime;
    if (mTimer > mLivenessTime) {
        mState = ActorState::Destroy;
    }


}


void Shot::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if (this->mColliderLayer == ColliderLayer::Player && other->GetLayer() == ColliderLayer::Enemy) {
        other->GetOwner()->Kill();
    }
    if (this->mColliderLayer == ColliderLayer::Enemy && other->GetLayer() == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    }
    mState = ActorState::Destroy;
}
void Shot::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    // if (other->GetLayer() != ColliderLayer::Blocks && other->GetLayer() != ColliderLayer::Pole) {
    //     mState = ActorState::Destroy;
    // }
}

