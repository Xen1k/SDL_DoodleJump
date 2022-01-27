#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "utils.h"

extern SDL_Renderer *renderer;

class GameObject
{
protected:
	SDL_Texture *texture;
	SDL_Rect clip;
public:
	float xPos = 0;
	float yPos = 0;

	bool facesRight = true;

	GameObject() {}

	GameObject(float xPos, float yPos, std::string texturePath, int clipRow = 0, int clipColumn = 0,
		int clipHeight = 1, int clipWidth = 1)
	{
		this->xPos = xPos;
		this->yPos = yPos;
		clip.h = tileSize * clipHeight;
		clip.w = tileSize * clipWidth;
		clip.x = clipRow * tileSize;
		clip.y = clipColumn * tileSize;
		texture = LoadTexture(texturePath, renderer);
	}

	bool IsHovering()
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		if (mouseX > xPos && mouseX < (xPos + GetSpriteWidth()) * sizeInc && mouseY > yPos && mouseY < (yPos + GetSpriteHeight()) * sizeInc)
			return true;
		return false;
	}

	int GetSpriteHeight()
	{
		return clip.h;
	}

	int GetSpriteWidth()
	{
		return clip.w;
	}

	void DestroyTexture()
	{
		SDL_DestroyTexture(texture);
	}
	void Render(int sizeInc_ = sizeInc)
	{
		RenderTexture(texture, renderer, xPos, yPos, sizeInc_, !facesRight, &clip);
	}
};