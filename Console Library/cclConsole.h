#pragma once

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

