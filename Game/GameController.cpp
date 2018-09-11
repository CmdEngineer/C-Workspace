#include "GameController.h"
#include "Graphics.h"

GameScreen* GameController::currentScreen;
bool GameController::loading;
HPTimer* GameController::hpTimer;

void GameController::Init()
{
	loading = true;
	currentScreen = 0;
	hpTimer = new HPTimer();
}

void GameController::LoadInitialScreen(GameScreen* screen)
{
	loading = true;
	currentScreen = screen;
	currentScreen->Load();
	loading = false;
}

void GameController::SwitchScreen(GameScreen* screen)
{
	loading = true;
	currentScreen->Unload();
	screen->Load();
	delete currentScreen;
	currentScreen = screen;
	loading = false;
}

void GameController::Render()
{
	if (loading) return;
	currentScreen->Render();
}

void GameController::Update()
{
	if (loading) return;
	hpTimer->Update();
	currentScreen->Update(hpTimer->GetTimeTotal(), hpTimer->GetTimeDelta());
}