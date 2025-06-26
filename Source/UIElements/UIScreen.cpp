// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game* game, const std::string& fontName)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mSelectedButtonIndex(-1)
{
	// Add to UI Stack
	mGame->PushUI(this);

    // Load default font
    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    for (auto t : mTexts) {
        delete t;
    }
    mTexts.clear();

    for (auto b : mButtons) {
        delete b;
    }
	mButtons.clear();

    for (auto img : mImages) {
        delete img;
    }
    mImages.clear();
}

void UIScreen::Update(float deltaTime)
{
	
}

void UIScreen::Draw(SDL_Renderer *renderer)
{
    // Draw texts
    for (auto t : mTexts) {
        t->Draw(renderer, mPos);
    }

    // Draw buttons
    for (auto b : mButtons) {
        b->Draw(renderer, mPos);
    }

    // Draw images
    for (auto img : mImages) {
        img->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys)
{

}

void UIScreen::HandleKeyPress(int key)
{
    if(key == SDLK_w)
    {
        if (mSelectedButtonIndex >= 0) {
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            mSelectedButtonIndex--;
            if(mSelectedButtonIndex < 0) {
                mSelectedButtonIndex = mButtons.size() - 1;
            }

            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
    }
    else if(key == SDLK_s)
    {
        if (mSelectedButtonIndex >= 0) {
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

            mSelectedButtonIndex++;
            if(mSelectedButtonIndex >= mButtons.size()) {
                mSelectedButtonIndex = 0;
            }

            mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        }
    }
    else if(key == SDLK_RETURN)
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < mButtons.size()) {
            mButtons[mSelectedButtonIndex]->OnClick();
        }
    }
}

void UIScreen::Close()
{
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string &name, const Vector2 &pos, const Vector2 &dims, const int pointSize, const int unsigned wrapLength)
{
    auto t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, Color::White);
    mTexts.emplace_back(t);
    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2 &pos, const Vector2& dims, std::function<void()> onClick)
{
    auto b = new UIButton(name, mFont, onClick, pos, dims);
	mButtons.emplace_back(b);

    // Set the first button as highlighted
    if(mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage* UIScreen::AddImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &dims, const Vector3 &color)
{
    auto img = new UIImage(imagePath, pos, dims, color);
    mImages.emplace_back(img);
    return img;
}