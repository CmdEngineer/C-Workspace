#include <Windows.h>
#include "Graphics.h"
#include "GameController.h"
#include "MainScreen.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Graphics* graphics;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow)
{
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.lpszClassName = "MainWindow";
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&windowClass);

	RECT rect = { 0, 0, 800, 600 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND windowHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Woww Window!", WS_OVERLAPPEDWINDOW, 
		100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, 0);
	
	if (!windowHandle) return -1;

	graphics = new Graphics();
	if (!graphics->Init(windowHandle))
	{
		delete graphics;
		return -1;
	}
	ShowWindow(windowHandle, nCmdShow);

	GameScreen::Init(graphics);
	GameController::Init();
	GameController::LoadInitialScreen(new MainScreen());
	
	MSG message;
	message.message = WM_NULL;
	 
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			DispatchMessage(&message);
		else
		{
			GameController::Update();
			GameController::Render();
		}
	}

	delete graphics;
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY) { PostQuitMessage(0); return 0; }
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
