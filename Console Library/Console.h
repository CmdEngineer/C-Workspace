#pragma once
/*
Console.h is a I/O high-level library to create eazy I/O using Windows.h
Console.h is created and maintained by CmdEnginner (Also known as Yarin Alfassi)

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
#include <future>
#include <functional>
#include <list>
#include <vector>
#include <stdlib.h>
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

namespace TypeFace
{
	namespace size3x3 {
		static int all[45][9] = {
			// space bar
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			// number
			{ 1, 1, 1,  1, 0, 1,  1, 1, 1 },
			{ 1, 1, 0,  0, 1, 0,  1, 1, 1 },
			{ 1, 1, 0,  0, 1, 0,  0, 1, 1 },
			{ 1, 1, 1,  0, 1, 1,  1, 1, 1 },
			{ 1, 0, 1,  1, 1, 1,  0, 0, 1 },
			{ 0, 1, 1,  0, 1, 0,  1, 1, 0 },
			{ 1, 0, 0,  1, 1, 1,  1, 1, 1 },
			{ 1, 1, 1,  0, 0, 1,  0, 0, 1 },
			{ 1, 1, 1,  1, 1, 1,  1, 1, 1 },
			{ 1, 1, 1,  1, 1, 1,  0, 0, 1 },
			// character
			{ 0, 1, 0,  1, 1, 1,  1, 0, 1 },
			{ 1, 1, 0,  1, 1, 1,  1, 1, 0 },
			{ 1, 1, 1,  1, 0, 0,  1, 1, 1 },
			{ 1, 1, 0,  1, 0, 1,  1, 1, 0 },
			{ 1, 0, 0,  1, 1, 0,  1, 1, 1 },
			{ 1, 1, 1,  1, 1, 0,  1, 0, 0 },
			{ 1, 1, 0,  1, 1, 1,  1, 1, 1 },
			{ 1, 0, 1,  1, 1, 1,  1, 0, 1 },
			{ 1, 1, 1,  0, 1, 0,  1, 1, 1 },
			{ 0, 1, 1,  0, 0, 1,  1, 1, 0 },
			{ 1, 0, 1,  1, 1, 0,  1, 0, 1 },
			{ 1, 0, 0,  1, 0, 0,  1, 1, 1 },
			{ 1, 1, 1,  1, 1, 1,  1, 0, 1 },
			{ 1, 1, 0,  1, 0, 1,  1, 0, 1 },
			{ 0, 1, 0,  1, 0, 1,  0, 1, 0 },
			{ 1, 1, 1,  1, 1, 1,  1, 0, 0 },
			{ 0, 1, 0,  1, 0, 1,  0, 1, 1 },
			{ 1, 1, 0,  1, 1, 0,  1, 0, 1 },
			{ 0, 1, 1,  0, 1, 0,  1, 1, 0 },
			{ 1, 1, 1,  0, 1, 0,  0, 1, 0 },
			{ 1, 0, 1,  1, 0, 1,  1, 1, 1 },
			{ 1, 0, 1,  1, 0, 1,  0, 1, 0 },
			{ 1, 0, 1,  1, 1, 1,  1, 1, 1 },
			{ 1, 0, 1,  0, 1, 0,  1, 0, 1 },
			{ 1, 0, 1,  0, 1, 0,  0, 1, 0 },
			{ 1, 1, 0,  0, 1, 0,  0, 1, 1 }
		};
		static int space_bar[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		static int number[10][9] = {
			{ 1, 1, 1,  1, 0, 1,  1, 1, 1 },
			{ 1, 1, 0,  0, 1, 0,  1, 1, 1 },
			{ 1, 1, 0,  0, 1, 0,  0, 1, 1 },
			{ 1, 1, 1,  0, 1, 1,  1, 1, 1 },
			{ 1, 0, 1,  1, 1, 1,  0, 0, 1 },
			{ 0, 1, 1,  0, 1, 0,  1, 1, 0 },
			{ 1, 0, 0,  1, 1, 1,  1, 1, 1 },
			{ 1, 1, 1,  0, 0, 1,  0, 0, 1 },
			{ 1, 1, 1,  1, 1, 1,  1, 1, 1 },
			{ 1, 1, 1,  1, 1, 1,  0, 0, 1 },
		};
		static int character[26][9] =
		{
			{ 0, 1, 0,  1, 1, 1,  1, 0, 1 },
			{ 1, 1, 0,  1, 1, 1,  1, 1, 0 },
			{ 1, 1, 1,  1, 0, 0,  1, 1, 1 },
			{ 1, 1, 0,  1, 0, 1,  1, 1, 0 },
			{ 1, 0, 0,  1, 1, 0,  1, 1, 1 },
			{ 1, 1, 1,  1, 1, 0,  1, 0, 0 },
			{ 1, 1, 0,  1, 1, 1,  1, 1, 1 },
			{ 1, 0, 1,  1, 1, 1,  1, 0, 1 },
			{ 1, 1, 1,  0, 1, 0,  1, 1, 1 },
			{ 0, 1, 1,  0, 0, 1,  1, 1, 0 },
			{ 1, 0, 1,  1, 1, 0,  1, 0, 1 },
			{ 1, 0, 0,  1, 0, 0,  1, 1, 1 },
			{ 1, 1, 1,  1, 1, 1,  1, 0, 1 },
			{ 1, 1, 0,  1, 0, 1,  1, 0, 1 },
			{ 0, 1, 0,  1, 0, 1,  0, 1, 0 },
			{ 1, 1, 1,  1, 1, 1,  1, 0, 0 },
			{ 0, 1, 0,  1, 0, 1,  0, 1, 1 },
			{ 1, 1, 0,  1, 1, 0,  1, 0, 1 },
			{ 0, 1, 1,  0, 1, 0,  1, 1, 0 },
			{ 1, 1, 1,  0, 1, 0,  0, 1, 0 },
			{ 1, 0, 1,  1, 0, 1,  1, 1, 1 },
			{ 1, 0, 1,  1, 0, 1,  0, 1, 0 },
			{ 1, 0, 1,  1, 1, 1,  1, 1, 1 },
			{ 1, 0, 1,  0, 1, 0,  1, 0, 1 },
			{ 1, 0, 1,  0, 1, 0,  0, 1, 0 },
			{ 1, 1, 0,  0, 1, 0,  0, 1, 1 }
		};
	}
	namespace size5x5 {
		static int space_bar[25] = { 0 };
		static int underscore[25] = { 0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  1, 1, 1, 1, 1 };
		static int number[10][25] =
		{
			{ 1, 1, 1, 1, 1,  1, 0, 0, 1, 1,  1, 0, 1, 0, 1,  1, 1, 0, 0, 1,  1, 1, 1, 1, 1 },
			{ 0, 0, 1, 0, 0,  0, 1, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  1, 1, 1, 1, 1 },
			{ 1, 1, 1, 1, 0,  0, 0, 0, 0, 1,  0, 1, 1, 1, 1,  1, 0, 0, 0, 0,  1, 1, 1, 1, 1 },
			{ 1, 1, 1, 1, 1,  0, 0, 0, 0, 1,  0, 1, 1, 1, 0,  0, 0, 0, 0, 1,  1, 1, 1, 1, 1 },
			{ 0, 0, 0, 1, 0,  0, 0, 1, 1, 0,  0, 1, 0, 1, 0,  1, 1, 1, 1, 1,  0, 0, 0, 1, 0 },
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 0,  1, 1, 1, 1, 0,  0, 0, 0, 0, 1,  1, 1, 1, 1, 0 },
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 0,  1, 1, 1, 1, 0,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1 },
			{ 1, 1, 1, 1, 1,  0, 0, 0, 0, 1,  0, 0, 0, 1, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0 },
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1 },
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1,  0, 0, 0, 0, 1,  1, 1, 1, 1, 1 }
		};
		static int character[26][25] = {
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1,  1, 0, 0, 0, 1 }, // A
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 0,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1 }, // B
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 0,  1, 0, 0, 0, 0,  1, 0, 0, 0, 0,  1, 1, 1, 1, 1 }, // C
			{ 1, 1, 1, 1, 0,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 0 }, // D
			{ 1, 1, 1, 1, 0,  1, 0, 0, 0, 0,  1, 1, 1, 1, 0,  1, 0, 0, 0, 0,  1, 1, 1, 1, 0 }, // E
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 0,  1, 1, 1, 1, 0,  1, 0, 0, 0, 0,  1, 0, 0, 0, 0 }, // F
			{ 1, 1, 1, 1, 0,  1, 0, 0, 0, 0,  1, 0, 1, 1, 0,  1, 0, 0, 1, 0,  1, 1, 1, 1, 0 }, // G
			{ 1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1 }, // H
			{ 1, 1, 1, 1, 1,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  1, 1, 1, 1, 1 }, // I
			{ 0, 0, 1, 1, 1,  0, 0, 0, 0, 1,  0, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1 }, // J
			{ 1, 0, 0, 0, 1,  1, 0, 0, 1, 0,  1, 1, 1, 0, 0,  1, 0, 0, 1, 0,  1, 0, 0, 0, 1 }, // K
			{ 1, 0, 0, 0, 0,  1, 0, 0, 0, 0,  1, 0, 0, 0, 0,  1, 0, 0, 0, 0,  1, 1, 1, 1, 1 }, // L
			{ 1, 0, 0, 0, 1,  1, 1, 0, 1, 1,  1, 0, 1, 0, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1 }, // M
			{ 1, 0, 0, 0, 1,  1, 1, 0, 0, 1,  1, 0, 1, 0, 1,  1, 0, 0, 1, 1,  1, 0, 0, 0, 1 }, // N
			{ 0, 1, 1, 1, 0,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  0, 1, 1, 1, 0 }, // O
			{ 1, 1, 1, 1, 0,  1, 0, 0, 0, 1,  1, 1, 1, 1, 0,  1, 0, 0, 0, 0,  1, 0, 0, 0, 0 }, // P
			{ 1, 1, 1, 1, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1,  0, 0, 1, 0, 0 }, // Q
			{ 1, 1, 1, 1, 0,  1, 0, 0, 0, 1,  1, 1, 1, 1, 0,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1 }, // R
			{ 0, 1, 1, 1, 1,  1, 0, 0, 0, 0,  0, 1, 1, 1, 0,  0, 0, 0, 0, 1,  1, 1, 1, 1, 0 }, // S
			{ 1, 1, 1, 1, 1,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0 }, // T
			{ 1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 1, 1, 1 }, // U
			{ 1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  0, 1, 0, 1, 0,  0, 1, 0, 1, 0,  0, 0, 1, 0, 0 }, // V
			{ 1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 0, 1, 0, 1,  1, 0, 1, 0, 1,  0, 1, 0, 1, 0 }, // W
			{ 1, 0, 0, 0, 1,  0, 1, 0, 1, 0,  0, 0, 1, 0, 0,  0, 1, 0, 1, 0,  1, 0, 0, 0, 1 }, // X
			{ 1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  0, 1, 1, 1, 0,  0, 0, 1, 0, 0,  0, 0, 1, 0, 0 }, // Y
			{ 1, 1, 1, 1, 1,  0, 0, 0, 1, 0,  0, 0, 1, 0, 0,  0, 1, 0, 0, 0,  1, 1, 1, 1, 1 }  // Z
		};
	}
}

namespace console
{
	static HWND consoleWindow = GetConsoleWindow();
	static HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	static HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	static POINT cursorPos;
	static DWORD cWritten, status;
	static Brush _textAttribute = DEFAULT_BRUSH;
	static struct {
		bool ADVANCE_CURSOR = true;
	} flags;

	int error(bool condition, STR title, STR content, bool exitProgram = true);

	void clear(Point from);
	void clear();
	void maximize();
	void allowInput(bool flag);
	HANDLE getInput();
	HANDLE getOutput();

	Region getScreenRegion();
	CONSOLE_SCREEN_BUFFER_INFO getScreenInfo();
	CONSOLE_FONT_INFOEX getFont();

	void drawPixel(CHAR character, Point pos, Brush color);
	void drawPixel(Pixel pixel);

	void drawRegion(Region r, BrushEx brush);
	void drawRegion(Region r, Brush color);

	void drawTexture(Texture t, Point pos, bool italics = false);

	Pixel getPixel(Point pos);
	void getPixels(Region region, Pixel* buffer, int bufferLen);

	void setAttribute(Point pos, Brush color = BACKGROUND_WHITE, int length = 1);
	void setAttribute(Brush color = BACKGROUND_WHITE, int length = 1);
	void setTextAttribute(Brush color);

	void setCharacter(Point pos, CHAR character = SPACE, int length = 1);
	void setCharacter(CHAR character = SPACE, int length = 1);

	//Console operator>>(STR str);
	void printInRegion(STR string, Point relative, bool vertical, Region textBox, Vector stepOffset = { 0, 0 });
	void print(STR string, Point pos, bool vertical = false, Vector stepOffset = {0, 0});
	void print(STR string);
	
	//Console operator<<(STR str);
	void insert(STR string);

	void setFontSize(Point size, bool maximized);
	void setFont(WSTR fontName, bool maximized);
	Point getSize(bool maximized);
	void setCursorPos(Point pos);
	Point getCursorPos();
	Point getCursorPoint();

	INPUT_RECORD charAsInput(CHAR chr);

	//Console() { }
};

namespace events
{
	enum MouseButton { ANY_MB, LEFT_MB, RIGHT_MB };
	enum MouseWheel { ANY_MW, HORIZONTAL_MW, VERTICAL_MW };

	static DWORD eventsRead;
	static INPUT_RECORD eventsBuffer[128];
	void getEvents();
	void parseKeyEvent(KEY_EVENT_RECORD e);
	void parseMouseEvent(MOUSE_EVENT_RECORD e);

	class Event
	{
	private:
		static Event* instance;
		Event() {};
	public:
		static Event* getInstance();
		virtual EVENT void KeyEvent(KEY_EVENT_RECORD e);
		virtual EVENT void MouseEvent(MOUSE_EVENT_RECORD e);
		virtual EVENT void KeyPressedEvent(WORD key);
		virtual EVENT void KeyReleasedEvent(WORD key);
		virtual EVENT void MouseMoveEvent(Point pos);
		virtual EVENT void MousePressedEvent(MouseButton button, Point pos);
		virtual EVENT void MouseDoubleClickEvent(MouseButton button, Point pos);
		virtual EVENT void MouseWheeledEvent(MouseWheel wheel, Point pos);
		virtual EVENT void WindowResizedEvent(Point size);
	};
	class EventHandler
	{
	private:
		std::shared_future<void> thread;
	public:
		virtual void onMouseEvent(MOUSE_EVENT_RECORD e) {};
		virtual void onKeyEvent(KEY_EVENT_RECORD e) {};
		virtual void onKeyPressedEvent(WORD key) {};
		virtual void onKeyReleasedEvent(WORD key) {};
		virtual void onMouseMoveEvent(Point pos) {};
		virtual void onMousePressedEvent(MouseButton button, Point pos) {};
		virtual void onMouseDoubleClickEvent(MouseButton button, Point pos) {};
		virtual void onMouseWheeledEvent(MouseWheel wheel, Point pos) {};
		virtual void onWindowResizedEvent(Point size) {};
		void loop() { while (true) {} };
		EventHandler();
		~EventHandler();
	};
}

namespace graphics
{
	void drawText(STR str, Point pos);

	void drawLine(Point p1, Point p2, BrushEx brush);

	void drawRect(Point c1, Point c2, size_t borderSize, BrushEx brush);
	void fillRect(Point c1, Point c2, BrushEx brush);

	void drawCircle(Point mid, size_t radius, Brush brush, int minDegree = 0, int maxDegree = 360);
	void fillCircle(Point mid, size_t radius, Brush brush, int minDegree = 0, int maxDegree = 360);

	class Sprite
	{
	private:
		Point pos;
		Palette palette;
		std::vector<Texture> textures;
		size_t index;
	public:
		Sprite(Point pos, Palette palette);
		void setPalette(Palette palette);
		void addFrame(Texture t, size_t at);
		void draw();
		void forward();
		void backward();
		void animate(time_t milliseconds, bool repeat);
		void animate(size_t to, time_t milliseconds, bool repeat);
		void animate(size_t from, size_t to, time_t milliseconds, bool repeat);
	};
}

namespace io
{

	class Element
	{
	public:
		Region region;
		void(*onclick)(Point);
		virtual void draw() {};
	};

	static std::list<Element*> elements = {};

	class IoEventHandler : public events::EventHandler
	{
	public:
		void onMousePressedEvent(events::MouseButton button, Point pos);
	};

	static IoEventHandler ioHandler = IoEventHandler();

	class Button : public Element
	{
	public:
		void draw() override
		{
			console::drawRegion(this->region, graphicBrush);
		}
		Button(Region _region, BrushEx brush);
		~Button();
	private:
		BrushEx graphicBrush;
	};
}