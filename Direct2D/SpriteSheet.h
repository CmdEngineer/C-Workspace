#pragma once

#include <wincodec.h>
#include "Graphics.h"

class SpriteSheet
{
	Graphics* gfx;
	ID2D1Bitmap* bmp;
	int spriteWidth, spriteHeight;
	int spritesAccross;
public:
	SpriteSheet(wchar_t* filename, Graphics* gfx, HRESULT* res);
	SpriteSheet(wchar_t* filename, Graphics* gfx, int spriteWidth, int spriteHeight, HRESULT* res);
	~SpriteSheet();

	void Draw();
	void Draw(int index, float x, float y);
};