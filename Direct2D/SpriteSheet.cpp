#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(wchar_t* filename, Graphics* gfx, HRESULT* res)
{
	this->gfx = gfx;
	bmp = NULL;
	HRESULT hr;
	IWICImagingFactory *wicFactory = NULL;
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&wicFactory);
	if (hr != S_OK) { if (res != NULL) { *res = hr; } return; }

	IWICBitmapDecoder *wicDecoder = NULL;
	hr = wicFactory->CreateDecoderFromFilename(
		filename, NULL, GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&wicDecoder);
	if (hr != S_OK) { if (res != NULL) { *res = hr; } return; }

	IWICBitmapFrameDecode* wicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (hr != S_OK) { if (res != NULL) { *res = hr; } return; }

	IWICFormatConverter *wicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&wicConverter);
	if (hr != S_OK) { if (res != NULL) { *res = hr; } return; }

	hr = wicConverter->Initialize(wicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);
	if (hr != S_OK) { if (res != NULL) { *res = hr; } return; }

	hr = gfx->GetRenderTarget()->CreateBitmapFromWicBitmap(wicConverter, NULL, &bmp);
	if (hr != S_OK) { if (res != NULL) { *res = hr; } return; }
	
	if (wicFactory) wicFactory->Release();
	if (wicDecoder) wicDecoder->Release();
	if (wicConverter) wicConverter->Release();
	if (wicFrame) wicFrame->Release();

	spriteWidth = bmp->GetSize().width;
	spriteHeight = bmp->GetSize().height;
	spritesAccross = 1;
}

SpriteSheet::SpriteSheet(wchar_t* filename, Graphics* gfx, int spriteWidth, int spriteHeight, HRESULT* res):
SpriteSheet(filename, gfx, res)
{
	this->spriteHeight = spriteHeight;
	this->spriteWidth = spriteWidth;
	if (bmp)
	{
		this->spritesAccross = (int)bmp->GetSize().width / spriteWidth;
	}
	else this->spritesAccross = 0;
}


SpriteSheet::~SpriteSheet()
{
	if (bmp) bmp->Release();
}

void SpriteSheet::Draw()
{
	gfx->GetRenderTarget()->DrawBitmap(bmp, D2D1::RectF(0.0f, 0.0f, bmp->GetSize().width, bmp->GetSize().height), 1.0f,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0.0f, 0.0f, bmp->GetSize().width, bmp->GetSize().height));
}

void SpriteSheet::Draw(int index, float x, float y)
{
	D2D_RECT_F src = D2D1::RectF(
		(float)((index % spritesAccross) * spriteWidth),
		(float)((index / spritesAccross) * spriteHeight),
		(float)((index % spritesAccross) * spriteWidth) + spriteWidth,
		(float)((index / spritesAccross) * spriteHeight) + spriteHeight);

	D2D_RECT_F dst = D2D1::RectF(
		x, y,
		x + spriteWidth,
		y + spriteHeight);

	gfx->GetRenderTarget()->DrawBitmap(bmp, dst, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
}