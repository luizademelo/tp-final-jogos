//
// Created by Pc on 06/07/2025.
//


#pragma once
#include "Actor.h"
#include <string>

class DrawAnimatedComponent;


class LifeHeart : public Actor {
public:
    LifeHeart(Game* game, Vector2 position);
    void OnUpdate(float deltaTime) override;
    float GetLifePercentage(){return mLifePercentage; }
    void SetLifePercentage(float percentage){mLifePercentage = percentage;}
    void SetAnimation(std::string animation);

private:
    DrawAnimatedComponent* mDrawComponent;
    float mLifePercentage = 1;
};
