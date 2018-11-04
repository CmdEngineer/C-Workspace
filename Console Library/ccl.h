/*

			CommandEngineer's Console Library.
			C				  C		  L


CCL is a I/O high-level library to create eazy I/O using Windows.h
CCL is created and maintained by CmdEnginner (Also known as Yarin Alfassi)

Current Version: 1.1.0

Main-Class: Console
Sub-Classes: BrushEx, Vector, Point, Pixel, Region
Special-Classes: Texture, Palette.
Background-Brush: BACKGROUND_{COLOR} | BACKGROUND_{COLOR} | BACKGROUND_{COLOR} | BACKGROUND_INTENSITY
Foreground-Brush: FOREGROUND_{COLOR} | FOREGROUND_{COLOR} | FOREGROUND_{COLOR} | FOREGROUND_INTENSITY

Notes:
To pass to STR and CHAR use TEXT(quote)
Always remember to .Destroy() special classes.
When a special class is attached to a different class it will be Destroyed automaticaly when the base class is Destroyed.
*/

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <future>
#include <functional>
#include <list>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

/// Program flags:
// FAST_DRAW -> When enabled will check each pixel before drawing to not waste time.
#define FAST_DRAW FALSE

#define CODE_ERROR -1
#define CODE_OK 0

// Some computers run Windows.h based on unicode not ascii, I made it so my program runs both. use TEXT(quote) to make sure your text translates correctly.
#ifndef UNICODE
#define STR LPCSTR
#define WSTR LPCWSTR
#define CHAR CHAR
#define LEN(str) strlen(str)
#define SPACE ' '
#else
#define STR LPCWSTR
#define WSTR STR
#define CHAR WCHAR
#define LEN(str) wcslen(str)
#define SPACE L' '
#endif

/// Color defines
#define BACKGROUND_WHITE BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
#define BACKGROUND_DEFAULT 0
#define FOREGROUND_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_DEFAULT FOREGROUND_WHITE
#define DEFAULT_CHARACTER SPACE
#define DEFAULT_BRUSH BACKGROUND_DEFAULT | FOREGROUND_DEFAULT
#define DEFAULT_BRUSH_EX BrushEx(BACKGROUND_DEFAULT | FOREGROUND_DEFAULT, SPACE)
#define DEFAULT_PALETTE_SIZE 1
#define DEFAULT_TEXTURE_WIDTH 16
#define DEFAULT_TEXTURE_HEIGHT 16
#define INVALID_PIXEL Pixel(false);
#define INVALID_POINT Point(false);
#define INVALID_REGION Region(false);

// Event Handles:
#define HOOK __hook
#define UNHOOK __unhook
#define EVENT __event
#define RAISE __raise

#define POSITIVE 1
#define EQUAL 0
#define NEGATIVE -1

typedef WORD Brush;

class BrushEx {
public:
	Brush brush;
	CHAR character;
	BrushEx() { brush = DEFAULT_BRUSH; character = SPACE; valid = true; };
	BrushEx(Brush _brush) { brush = _brush; character = SPACE; valid = true; };
	BrushEx(Brush _brush, CHAR _character) { brush = _brush; character = _character; valid = true; };
	BrushEx(Brush _brush, CHAR _character, bool flag) : BrushEx() { brush = _brush; character = _character; valid = flag; };
	bool operator==(BrushEx other) { return (brush == other.brush && character == other.character); };
	bool operator!=(BrushEx other) { return !(*this == other); };
	bool isValid() { return valid; }
private:
	bool valid;
};

class Vector
{
public:
	int x, y, m;
	Vector();
	Vector(int _x, int _y);
	Vector(int _x, int _y, int _m);

	Vector normalize();
};

class Point
{
public:
	size_t x, y;
	Point();
	Point(bool flag);
	Point(size_t _x, size_t _y);
	Point(COORD coord);
	COORD toCoord();
	Vector getVector(Point other);
	Point forceApply(Vector vector);
	static bool parsePoints(Point* buffer, int bufferLen, int* grid, int width, int height);
	bool operator==(Point other);
	bool operator>(Point other);
	bool operator<(Point other);
	bool operator>=(Point other);
	bool operator<=(Point other);
	Point operator+(Point other);
	Point operator-(Point other);
	bool IsValid() { return valid; }
private:
	bool valid = true;
};

class Pixel
{
public:
	CHAR chr;
	Point pos;
	Brush color;
	Pixel();
	Pixel(bool flag);
	Pixel(CHAR _character);
	Pixel(Point _pos);
	Pixel(Point _pos, CHAR _character);
	Pixel(Point _pos, CHAR _character, Brush _color);
	Pixel(Point _pos, BrushEx brush);

	bool operator==(Pixel other);
	bool operator!=(Pixel other);
	bool IsValid() { return valid; }
private:
	bool valid = true;
};

class Region
{
public:
	Point leftTop, rightBottom;
	Region();
	Region(int flag);
	Region(Point corner, Point opposite);

	int getWidth();
	int getHeight();
	int getArea();
	bool contains(Point pos);
	void getPoints(Point* buffer, int bufferLen);
	bool isValid() { return valid; }
private:
	bool valid = true;
};

class Palette
{
public:
	size_t size;
	Palette();
	Palette(size_t size);
	void Destroy();
	bool Set(int id, Brush brush);
	bool SetEx(int id, BrushEx brush);
	bool Exists(int id);
	bool Exists(Brush brush);
	bool Exists(BrushEx brush);
	bool Remove(int id);
	Brush Get(int id);
	BrushEx GetEx(int id);
private:
	BrushEx* data;
};

class Texture
{
public:
	Palette* palette;
	size_t width, height;
	Texture();
	Texture(size_t _width, size_t _height, size_t paletteSize);
	void Destroy();
	void fromArray(int* arr);
	BrushEx At(Point p);
private:
	int** data;
	void initData();
};

// Include functions namespaces && headers:
#include "cclConsole.h"
#include "cclGraphics.h"
#include "cclIO.h"
