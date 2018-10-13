#pragma once
#pragma comment(lib, "Dwrite")

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

class Graphics
{
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* renderTarget;
	ID2D1SolidColorBrush* brush;
	IDWriteFactory* writeFactory;

public: 
	Graphics();
	~Graphics();

	typedef struct Font {
		const WCHAR* name; DWRITE_FONT_WEIGHT weight; DWRITE_FONT_STYLE style; DWRITE_FONT_STRETCH stretch;
	} Font;

	bool Init(HWND windowHandle);
	bool Graphics::InitRenderTarget(HWND handle);
	void Graphics::UpdateVariables(HWND windowHandle);

	ID2D1RenderTarget* GetRenderTarget() { return renderTarget; }
	void BeginDraw() { renderTarget->BeginDraw(); }
	void EndDraw() { renderTarget->EndDraw(); }

	void ClearScreen(float r, float g, float b);
	
	void SetBrush(float r, float g, float b, float a);
	ID2D1SolidColorBrush* GetBrush();
	
	void DrawCircle(float x, float y, float radius, float r, float g, float b, float a);
	void DrawCircle(float x, float y, float radius);
	void FillCircle(float x, float y, float radius, float r, float g, float b, float a);
	
	HRESULT DrawTextA(const WCHAR* string, float x, float y, float size, Font font, const WCHAR* locale);
	HRESULT DrawTextA(const WCHAR* string, float x, float y, float size, const WCHAR* font);
};