//
// Created by Pc on 28/06/2025.
//
#include "Shot.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Shot::Shot(Game* game, const Vector2& velocity, ColliderLayer layer)
    : Actor(game)
{

    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Shots/ToiletPaper/texture.png", "../Assets/Sprites/Shots/ToiletPaper/texture.json");
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f);
    mRigidBodyComponent->SetVelocity(velocity);
    mColliderComponent = new AABBColliderComponent(this, 0, 0,
                                                   Game::TILE_SIZE, Game::TILE_SIZE,
                                                   layer);
    mDrawComponent->AddAnimation("Normal", {0});
    mDrawComponent->SetAnimation("Normal");
    mDrawComponent->SetAnimFPS(5.0f);
}

void Shot::OnUpdate(float deltaTime)
{
    mTimer += deltaTime;
    if (mTimer > mLivenessTime) {
        mState = ActorState::Destroy;
    }


}


void Shot::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    // if (other->GetLayer() != ColliderLayer::Blocks && other->GetLayer() != ColliderLayer::Pole) {
    //     mState = ActorState::Destroy;
    // }
}
void Shot::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    // if (other->GetLayer() != ColliderLayer::Blocks && other->GetLayer() != ColliderLayer::Pole) {
    //     mState = ActorState::Destroy;
    // }
}

