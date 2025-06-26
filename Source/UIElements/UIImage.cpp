//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"

UIImage::UIImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &size, const Vector3 &color)
    : UIElement(pos, size, color),
    mTexture(nullptr)
{
    // Load the image texture
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (surface == nullptr) {
        SDL_Log("Failed to load image %s: %s", imagePath.c_str(), IMG_GetError());
        return;
    }

    // Create texture from surface
    mTexture = SDL_CreateTextureFromSurface(SDL_GetRenderer(SDL_GetWindowFromID(1)), surface);
    if (mTexture == nullptr) {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
    }

    SDL_FreeSurface(surface); // Free the surface after creating the texture
}

UIImage::~UIImage()
{
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void UIImage::Draw(SDL_Renderer* renderer, const Vector2 &screenPos)
{
    if (mTexture == nullptr) {
        return;
    }

    SDL_Rect destRect = {static_cast<int>(screenPos.x + mPosition.x),
                          static_cast<int>(screenPos.y + mPosition.y),
                          static_cast<int>(mSize.x),
                          static_cast<int>(mSize.y)};

    SDL_RenderCopy(renderer, mTexture, nullptr, &destRect);
}