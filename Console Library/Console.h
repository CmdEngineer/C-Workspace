#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#define BACKGROUND_WHITE BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
#define BACKGROUND_DEFAULT 0
#define FOREGROUND_WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define FOREGROUND_DEFAULT FOREGROUND_WHITE
#define DEFAULT_BRUSH BACKGROUND_DEFAULT | FOREGROUND_DEFAULT

#define CODE_ERROR -1
#define CODE_OK 0
#define STR LPCWSTR
#define CHAR WCHAR
#define LEN(str) wcslen(str)
#define SPACE L' '
#define DEFAULT_PIXEL Pixel(SPACE, {0, 0}, BACKGROUND_DEFAULT | FOREGROUND_DEFAULT)

#define INVALID_PIXEL Pixel(false);
#define INVALID_POINT Point(false);
#define FAST_DRAW FALSE

#define POSITIVE 1
#define EQUAL 0
#define NEGATIVE -1

typedef WORD Brush;

class BrushEx { 
public:
	Brush brush;
	CHAR character;
	BrushEx() { brush = DEFAULT_BRUSH; character = SPACE; };
	BrushEx(Brush _brush) { brush = _brush; character = SPACE; };
	BrushEx(Brush _brush, CHAR _character) { brush = _brush; character = _character; };
};

class Vector
{
public:
	int x, y;
	Vector();
	Vector(int _x, int _y);

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
	static bool parsePoints(Point* buffer, int bufferLen, int* grid, int width, int height);
	bool operator==(Point other);
	bool operator>(Point other);
	bool operator<(Point other);
	bool operator>=(Point other);
	bool operator<=(Point other);
	bool IsValid() { return valid; }
private:
	bool valid;
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
	Pixel(CHAR _character, Point _pos);
	Pixel(CHAR _character, Point _pos, Brush _color);

	bool operator==(Pixel other);
	bool operator!=(Pixel other);
	bool IsValid() { return valid; }
private:
	bool valid;
};

class Region
{
public:
	Point leftTop, rightBottom;
	Region();
	Region::Region(Point corner, Point opposite);

	int getWidth();
	int getHeight();
	int getArea();
	void getPoints(Point* buffer, int bufferLen);
};

class Console
{
private:
	bool initated;
	HWND consoleWindow;
	HANDLE input, output;
	POINT cursorPos;
	DWORD cWritten, status;
public:
	CONSOLE_FONT_INFO font;
	CONSOLE_SCREEN_BUFFER_INFO screen;

	int initConsole();

	int error(bool condition, STR title, STR content, bool exitProgram = true);

	void clear(Point from);
	void clear();

	Pixel getPixel(Point pos);
	void getPixels(Region region, Pixel* buffer, int bufferLen);

	void drawPixel(CHAR character, Point pos, Brush color);
	void drawPixel(Pixel pixel);
	void drawRegion(Region r, BrushEx brush);
	void drawRegion(Region r, Brush color);
	
	void setAttribute(Point pos, Brush color = BACKGROUND_WHITE, int length = 1);
	void setAttribute(Brush color = BACKGROUND_WHITE, int length = 1);

	void setCharacter(Point pos, CHAR character = SPACE, int length = 1);
	void setCharacter(CHAR character = SPACE, int length = 1);

	Console operator>>(STR str);
	void print(STR string, Point pos);
	void print(STR string);
	
	Console operator<<(STR str);
	void insert(STR string);

	void setCursorPos(Point pos);
	Point getCursorPos();
	Point getCursorPoint();

	INPUT_RECORD charAsInput(CHAR chr);

	Console() { initConsole(); }
};