#pragma once

#include "SpriteSheet.h"

class GameScreen
{
protected:
	static Graphics* gfx;
public:
	static void Init(Graphics* graphics) { gfx = graphics; }

	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void Render() = 0;
	virtual void Update(float timeTotal, float timeDelta) = 0;
};