//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"

UIButton::UIButton(const std::string& text, class UIFont* font, std::function<void()> onClick,
                    const Vector2& pos, const Vector2& size, const Vector3& color,
                    int pointSize , unsigned wrapLength,
                    const Vector2 &textPos, const Vector2 &textSize, const Vector3& textColor)
        :UIElement(pos, size, color)
        ,mOnClick(onClick)
        ,mHighlighted(false)
        ,mText(text, font, pointSize, wrapLength, textPos, textSize, textColor)
{

}

UIButton::~UIButton()
{

}


void UIButton::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    SDL_Rect titleQuad = {static_cast<int>(screenPos.x + mPosition.x),
                          static_cast<int>(screenPos.y + mPosition.y),
                          static_cast<int>(mSize.x),
                          static_cast<int>(mSize.y)};

    // Draw filled rect as button background
    if (mHighlighted)
    {
        SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
        SDL_RenderFillRect(renderer, &titleQuad);
    }

    // Draw text from the center of the button
    mText.Draw(renderer, screenPos + mPosition + mSize * 0.5f - mText.GetSize() * 0.5f);
}

void UIButton::OnClick()
{
    // Call attached handler, if it exists
    if (mOnClick) {
        mOnClick();
    }
}