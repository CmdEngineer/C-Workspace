#include "GameController.h"
#include "Graphics.h"
#include "MainScreen.h"
#include <string> 
using namespace std;

void MainScreen::Load()
{

}

void MainScreen::Unload()
{

}

void MainScreen::Update(float timeTotal, float timeDelta)
{
	frame += 1;
}

void MainScreen::Render()
{
	gfx->BeginDraw();
	gfx->ClearScreen(212.0f / 255.0f, 208.0f / 255.0f, 200.0f / 255.0f);
	gfx->SetBrush(0.0f, 0.0f, 0.0f, 1.0f);
	gfx->DrawCircle(0, 0, 10);
	gfx->DrawTextA(L"NITZAN", 0 + cos(frame) * 50, 0 + sin(frame) * 50, 20, L"Arial");
	gfx->EndDraw();
}