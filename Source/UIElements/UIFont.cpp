#include "UIFont.h"
#include <vector>
#include <SDL_image.h>

UIFont::UIFont(SDL_Renderer* renderer)
    :mRenderer(renderer)
{

}

UIFont::~UIFont()
{

}

bool UIFont::Load(const std::string& fileName)
{
	// We support these font sizes
	std::vector<int> fontSizes = {8,  9,  10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
								  34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64, 68, 72};

	for (auto& size : fontSizes)
	{
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (font == nullptr)
		{
			SDL_Log("Failed to load font %s in size %d: %s", fileName.c_str(), size, TTF_GetError());
			return false;
		}
		mFontData.emplace(size, font);
	}

	return true;
}

void UIFont::Unload()
{
	for (auto& font : mFontData)
    {
		TTF_CloseFont(font.second);
	}
    mFontData.clear();
}

SDL_Texture* UIFont::RenderText(const std::string& text, const Vector3& color /*= Color::White*/,
                                int pointSize /*= 24*/, unsigned wrapLength /*= 900*/)
{
    if(!mRenderer)
    {
        SDL_Log("Renderer is null. Can't Render Text!");
        return nullptr;
    }

	// Convert to SDL_Color
	SDL_Color sdlColor;

	// Swap red and blue so we get RGBA instead of BGRA
	sdlColor.b = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.r = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	// Find the font data for this point size
	auto iter = mFontData.find(pointSize);
	if (iter != mFontData.end())
	{
		TTF_Font* font = iter->second;
		// Draw this to a surface (blended for alpha)
		SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), sdlColor, wrapLength);
		if (!surf)
		{
			SDL_Log("Failed to create surface for text: %s", text.c_str());
            return nullptr;
		}

        // Create texture from surface
        SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surf);
        SDL_FreeSurface(surf);
        if (!texture)
        {
            SDL_Log("Failed to render surface to texture");
            return nullptr;
        }

        return texture;
	}
	else
	{
		SDL_Log("Point size %d is unsupported", pointSize);
	}

    return nullptr;
}
