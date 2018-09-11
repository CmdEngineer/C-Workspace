#include "GameController.h"
#include "Graphics.h"
#include "MainScreen.h"

void MainScreen::Load()
{
	xPos = 0.0f;
	yPos = 0.0f;
	sprites = new SpriteSheet(L"test.png", gfx, 64, 64, NULL);
}

void MainScreen::Unload() { 
	delete sprites;
}

void MainScreen::Update(float timeTotal, float timeDelta)
{
	xPos += (100*timeDelta);
	if (xPos > 800) xPos -= 800;
}

void MainScreen::Render()
{
	gfx->BeginDraw();
	gfx->ClearScreen(212.0f / 255.0f, 208.0f / 255.0f, 200.0f / 255.0f);
	gfx->SetBrush(0.0f, 0.0f, 0.0f, 1.0f);
	gfx->DrawTextA(L"Wow this shit actually works.", 0, 0, 50, L"Verdana");
	
	//sprites->Draw(0, xPos, yPos);
	gfx->EndDraw();
}