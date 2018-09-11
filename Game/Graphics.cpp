#include "Graphics.h"

Graphics::Graphics()
{
	factory = NULL;
	writeFactory = NULL;
	renderTarget = NULL;
	brush = NULL;
}

Graphics::~Graphics()
{
	if (factory) factory->Release();
	if (renderTarget) renderTarget->Release();
	if (brush) brush->Release();
	if (writeFactory) writeFactory->Release();
}

bool Graphics::Init(HWND windowHandle)
{
	// Render Target:
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (res != S_OK) return false;

	RECT rect;
	GetClientRect(windowHandle, &rect);
	
	res = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			windowHandle, D2D1::SizeU(rect.right, rect.bottom)),
		&renderTarget);
	if (res != S_OK) return false;

	// Brush:
	res = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);
	if (res != S_OK) return false;

	// Write Factory:
	res = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
	if (res != S_OK) return false;

	return true;
}

void Graphics::ClearScreen(float r, float g, float b)
{
	renderTarget->Clear(D2D1::ColorF(r, g, b));
}

void Graphics::DrawCircle(float x, float y, float radius, float r, float g, float b, float a)
{
	brush->SetColor(D2D1::ColorF(r, g, b, a));
	renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush, 3.0f);
}

void Graphics::SetBrush(float r, float g, float b, float a)
{
	if (brush) brush->SetColor(D2D1::ColorF(r, g, b, a));
}

HRESULT Graphics::DrawTextA(const WCHAR* string, float x, float y, float size, Font font, const WCHAR* locale)
{
	HRESULT res = S_OK;
	IDWriteTextFormat* textFormat;
	if (!locale) locale = L"";

	res = writeFactory->CreateTextFormat(font.name, NULL, font.weight, font.style, font.stretch, size, locale, &textFormat);
	if (res != S_OK) return res;

	renderTarget->DrawTextA(string, wcslen(string), textFormat,
		D2D1::RectF(x, y, renderTarget->GetSize().width, renderTarget->GetSize().height), brush);
	if (textFormat) textFormat->Release();
	return res;
}

HRESULT Graphics::DrawTextA(const WCHAR* string, float x, float y, float size, const WCHAR* font)
{
	Font newFont = {font, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL};
	return DrawTextA(string, x, y, size, newFont, NULL);
}