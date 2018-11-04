#include "ccl.h"

/// CONSOLE NAMESPACE:
#ifdef TRUE
	extern HWND consoleWindow;
	extern HANDLE input;
	extern HANDLE output;
	extern POINT cursorPos;
	extern DWORD cWritten, status;
	extern Brush _textAttribute;
	int console::error(bool condition, STR title, STR content, bool exitProgram)
	{
		if (condition)
		{
			MessageBox(NULL, content, title, MB_OK);
			if (exitProgram)
			{
				exit(CODE_ERROR);
			}
			else return CODE_ERROR;
		}
		else return CODE_OK;
	}

	void console::clear(Point from)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen = getScreenInfo();
		setCharacter(from, SPACE, screen.dwSize.X * screen.dwSize.Y);
		setAttribute(from, BACKGROUND_DEFAULT, screen.dwSize.X * screen.dwSize.Y);
		setCursorPos(from);
	}
	void console::clear()
	{
		CONSOLE_SCREEN_BUFFER_INFO screen = getScreenInfo();
		setCursorPos({ 0, 0 });
		setCharacter(SPACE, screen.dwSize.X * screen.dwSize.Y);
		setAttribute(BACKGROUND_DEFAULT, screen.dwSize.X * screen.dwSize.Y);
	}
	void console::maximize()
	{
		ShowWindow(consoleWindow, SW_MAXIMIZE);
	}
	void console::allowInput(bool flag)
	{
		if (flag)
		{
			SetConsoleMode(input, ENABLE_ECHO_INPUT);
		} else SetConsoleMode(input, ~ENABLE_ECHO_INPUT);
	}
	HANDLE console::getInput() { return input; };
	HANDLE console::getOutput() { return output; };
	CONSOLE_FONT_INFOEX console::getFont()
	{
		CONSOLE_FONT_INFOEX font;
		ZeroMemory(&font, sizeof(CONSOLE_FONT_INFOEX));
		font.cbSize = sizeof(font);
		GetCurrentConsoleFontEx(output, false, &font);
		return font;
	}
	CONSOLE_SCREEN_BUFFER_INFO console::getScreenInfo()
	{
		CONSOLE_SCREEN_BUFFER_INFO screen;
		ZeroMemory(&screen, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
		GetConsoleScreenBufferInfo(output, &screen);
		return screen;
	}
	Region console::getScreenRegion()
	{
		CONSOLE_SCREEN_BUFFER_INFO screen = getScreenInfo();
		return Region({ 0, 0 }, Point(screen.dwSize.X, screen.dwSize.Y));
	}
	
	Pixel console::getPixel(Point pos)
	{
		CHAR_INFO ci;
		SMALL_RECT rect = { 0,0,0,0 };
		ReadConsoleOutput(output, &ci, { 1,1 }, pos.toCoord(), &rect);
#ifndef UNICODE
		return Pixel(pos, ci.Char.AsciiChar, ci.Attributes);
#else
		return Pixel(pos, ci.Char.UnicodeChar, ci.Attributes);
#endif
	}
	void console::getPixels(Region region, Pixel* buffer, int bufferLen)
	{
		int i = 0;
		int width = region.getWidth();
		int height = region.getHeight();
		int diffX, diffY = 0;
		for (i = 0; i < bufferLen && i < (width*height); i++)
		{
			diffX = i % width;
			diffY = i / width;
			buffer[i] = getPixel({ region.leftTop.x + diffX, region.leftTop.y + diffY });
		}
	}

	void console::drawPixel(CHAR character, Point pos, Brush color)
	{
		drawPixel(Pixel(pos, character, color));
	}
	void console::drawPixel(Pixel pixel)
	{
		if (!FAST_DRAW || console::getPixel(pixel.pos) != pixel)
		{
			Point prevPos = console::getCursorPos();
			FillConsoleOutputCharacter(output, pixel.chr, 1, pixel.pos.toCoord(), &status);
			FillConsoleOutputAttribute(output, pixel.color, 1, pixel.pos.toCoord(), &status);
			console::setCursorPos(prevPos);
		}
	}

	void console::drawRegion(Region r, BrushEx brush)
	{
		Point* buffer = (Point*)malloc(sizeof(Point) * r.getArea());
		r.getPoints(buffer, r.getArea());
		for (int i = 0; i < r.getArea(); i++)
			drawPixel(brush.character, buffer[i], brush.brush);
		free(buffer);
	}
	void console::drawRegion(Region r, Brush color)
	{
		Point* buffer = (Point*)malloc(sizeof(Point) * r.getArea());
		r.getPoints(buffer, r.getArea());
		for (int i = 0; i < r.getArea(); i++)
			drawPixel(SPACE, buffer[i], color);
		free(buffer);
	}

	void console::drawTexture(Texture t, Point pos, bool italics)
	{
		BrushEx tempBrush;
		for (size_t y = 0; y < t.height; y++)
		{
			pos.x += italics;
			for (size_t x = 0; x < t.width; x++)
			{
				tempBrush = t.At({ x, y });
				if (tempBrush.isValid())
				{
					drawPixel(tempBrush.character, pos + Point(x, y), tempBrush.brush);
				}
			}
		}
	}

	void console::setAttribute(Point pos, Brush color, int length)
	{
		Point prevPos = getCursorPos();
		SetConsoleCursorPosition(output, pos.toCoord());
		FillConsoleOutputAttribute(output, color, length, pos.toCoord(), &status);
		setCursorPos(prevPos);
	}
	void console::setAttribute(Brush color, int length)
	{
		setAttribute(getCursorPos(), color, length);
	}
	void console::setTextAttribute(Brush color)
	{
		_textAttribute = color;
		SetConsoleTextAttribute(output, color);
	}
	void console::setCharacter(Point pos, CHAR character, int length)
	{
		Point prevPos = getCursorPos();
		SetConsoleCursorPosition(output, pos.toCoord());
		FillConsoleOutputCharacter(output, character, length, pos.toCoord(), &status);
		setCursorPos(prevPos);
	}
	void console::setCharacter(CHAR character, int length)
	{
		setCharacter(getCursorPos(), character, length);
	}

	/*Console console::operator >> (STR str)
	{
		print(str);
		return *this;
	}*/
	void console::print(STR string)
	{
		print(string, getCursorPos());
	}
	void console::printInRegion(STR string, Point relative, bool vertical, Region textBox, Vector stepOffset)
	{
		Point pos;
		pos.x = relative.x + textBox.leftTop.x;
		pos.y = relative.y + textBox.leftTop.y;
		size_t i;
		for (i = 0; i < LEN(string); i++)
		{
			setCharacter(pos, string[i]);
			setAttribute(pos, _textAttribute);
			if (pos.x <= textBox.rightBottom.x && (!vertical || pos.y <= textBox.rightBottom.y))
			{
				pos.x += (!vertical) + stepOffset.x;
				pos.y += (vertical)+stepOffset.y;
			}
			else {
				if (!vertical)
				{
					pos.x = relative.x + textBox.leftTop.x;
					pos.y += 1 + stepOffset.y;
				}
				else
				{
					pos.x += 1 + stepOffset.x;
					pos.y = relative.y + textBox.leftTop.y;
				}
			}
		}
		if (flags.ADVANCE_CURSOR)
			setCursorPos(pos);
	}
	void console::print(STR string, Point pos, bool vertical, Vector stepOffset)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen = getScreenInfo();
		for (size_t i = 0; i < LEN(string); i++)
		{
			setCharacter(pos, string[i]);
			setAttribute(pos, _textAttribute);
			if (pos.x <= (size_t)screen.dwSize.X && (!vertical || pos.y <= (size_t)screen.dwSize.Y))
			{
				if (!vertical) pos.x += 1 + stepOffset.x; else pos.y += 1 + stepOffset.y;

			}
			else {
				if (!vertical)
				{
					pos.x = 0;
					pos.y += 1 + stepOffset.y;
				}
				else
				{
					pos.x += 1 + stepOffset.x;
					pos.y = 0;
				}
			}
		}
		if (flags.ADVANCE_CURSOR)
			setCursorPos(pos);
	}
	//void console::printFont(STR string, Point pos, size_t width, size_t height);
	/*Console console::operator<<(STR str)
	{
		insert(str);
		return *this;
	}*/
	void console::insert(STR string)
	{
		size_t i;
		DWORD tmp;
		INPUT_RECORD* records = (INPUT_RECORD*)malloc(LEN(string) * sizeof(INPUT_RECORD));
		for (i = 0; i < LEN(string); i++)
		{
			records[i] = charAsInput(string[i]);
		}
		WriteConsoleInput(input, records, LEN(string), &tmp);
		free(records);
	}

	INPUT_RECORD console::charAsInput(CHAR chr)
	{
		INPUT_RECORD ir;
		ir.EventType = KEY_EVENT;
		ir.Event.KeyEvent.bKeyDown = TRUE;
		ir.Event.KeyEvent.uChar.UnicodeChar = chr;
		ir.Event.KeyEvent.wRepeatCount = 1;
		return ir;
	}
	
	void console::setFontSize(Point size, bool maximized)
	{
		CONSOLE_FONT_INFOEX font = getFont();
		font.dwFontSize.X = size.x;
		font.dwFontSize.Y = size.y;
		SetCurrentConsoleFontEx(output, maximized, &font);
	}
	void console::setFont(WSTR fontName, bool maximized)
	{
		CONSOLE_FONT_INFOEX font = getFont();
		wcscpy_s(font.FaceName, fontName);
		SetCurrentConsoleFontEx(output, maximized, &font);
	}
	Point console::getSize(bool maximized)
	{
		CONSOLE_SCREEN_BUFFER_INFO sb;
		GetConsoleScreenBufferInfo(output, &sb);
		if (maximized) return sb.dwMaximumWindowSize;
		return sb.dwSize;
	}
	Point console::getCursorPos()
	{
		CONSOLE_SCREEN_BUFFER_INFO sb;
		GetConsoleScreenBufferInfo(output, &sb);
		return sb.dwCursorPosition;
	}
	void console::setCursorPos(Point pos)
	{
		SetConsoleCursorPosition(output, pos.toCoord());
	}
	Point console::getCursorPoint()
	{
		GetCursorPos(&cursorPos);
		ScreenToClient(consoleWindow, &cursorPos);
		CONSOLE_FONT_INFOEX font = getFont();
		setCursorPos({ 0, 0 });
		//printf("%d / %d, %d / %d => %d, %d                      ", cursorPos.x, font.dwFontSize.X, cursorPos.y, font.dwFontSize.Y, Point(cursorPos.x / font.dwFontSize.X, cursorPos.y / font.dwFontSize.Y).x, Point(cursorPos.x / font.dwFontSize.X, cursorPos.y / font.dwFontSize.Y).y);
		return Point(cursorPos.x / font.dwFontSize.X, cursorPos.y / font.dwFontSize.Y);
	}
#endif
