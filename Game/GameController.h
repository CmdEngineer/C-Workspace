#pragma once

#include "GameScreen.h"
#include "HPTimer.h"
class GameController
{
	GameController() { }
	static HPTimer* hpTimer;
	static GameScreen* currentScreen;
public:
	static bool loading;

	static void Init();
	static void LoadInitialScreen(GameScreen* screen);
	static void SwitchScreen(GameScreen* screen);

	static void Render();
	static void Update();
};