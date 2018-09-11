#pragma once

#include "GameScreen.h"

class MainScreen : public GameScreen
{
	float xPos, yPos;
	float y;
	float ySpeed;
	int frame;

	SpriteSheet* sprites;

public:
	void Load() override;
	void Unload() override;
	void Render() override;
	void Update(float timeTotal, float timeDelta) override;
};