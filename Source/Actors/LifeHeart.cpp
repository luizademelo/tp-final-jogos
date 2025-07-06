//
// Created by Pc on 06/07/2025.
//
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "LifeHeart.h"
LifeHeart::LifeHeart(Game *game, Vector2 position) : Actor(game) {
    mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Heart/texture.png", "../Assets/Sprites/Heart/texture.json");
    mDrawComponent->AddAnimation("Full", {2});
    mDrawComponent->AddAnimation("Half", {0});
    mDrawComponent->AddAnimation("Empty", {1});
    mDrawComponent->SetAnimation("Full");
}

void LifeHeart::OnUpdate(float deltaTime) {

}

void LifeHeart::SetAnimation(std::string animation) {
    mDrawComponent->SetAnimation(animation);
}
