//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string &text, class UIFont* font, int pointSize, const unsigned wrapLength,
               const Vector2 &pos, const Vector2 &size, const Vector3 &color)
   :UIElement(pos, size, color)
   ,mFont(font)
   ,mPointSize(pointSize)
   ,mWrapLength(wrapLength)
   ,mTextTexture(nullptr)
{
    SetText(text);
}

UIText::~UIText()
{

}

void UIText::SetText(const std::string &text)
{
    // Clear out previous title texture if it exists
    if (mTextTexture)
    {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }

    // Create texture for title
    mTextTexture = mFont->RenderText(text, mColor, mPointSize, mWrapLength);
}

void UIText::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    SDL_Rect titleQuad = {static_cast<int>(screenPos.x + mPosition.x),
                          static_cast<int>(screenPos.y + mPosition.y),
                          static_cast<int>(mSize.x),
                          static_cast<int>(mSize.y)};

    SDL_RenderCopyEx(renderer, mTextTexture, nullptr, &titleQuad, .0f, nullptr, SDL_FLIP_NONE);
}