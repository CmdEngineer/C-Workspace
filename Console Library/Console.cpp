#include "Console.h"

/// CONSOLE CLASS:
#ifdef TRUE
	extern HWND consoleWindow;
	extern HANDLE input;
	extern HANDLE output;
	extern POINT cursorPos;
	extern DWORD cWritten, status;
	extern Brush __textAttributee;
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
		return Pixel(ci.Char.AsciiChar, pos, ci.Attributes);
#else
		return Pixel(ci.Char.UnicodeChar, pos, ci.Attributes);
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
		drawPixel(Pixel(character, pos, color));
	}
	void console::drawPixel(Pixel pixel)
	{
		if (!FAST_DRAW || getPixel(pixel.pos) != pixel)
		{
			Point prevPos = getCursorPos();
			FillConsoleOutputCharacter(output, pixel.chr, 1, pixel.pos.toCoord(), &status);
			FillConsoleOutputAttribute(output, pixel.color, 1, pixel.pos.toCoord(), &status);
			setCursorPos(prevPos);
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

	void console::drawTexture(Texture t, Point pos)
	{
		BrushEx  tempBrush;
		for (size_t y = 0; y < t.height; y++)
		{
			for (size_t x = 0; x < t.width; x++)
			{
				tempBrush = t.At({ x, y });
				drawPixel(tempBrush.character, pos + Point(x, y), tempBrush.brush);
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
		return Point(cursorPos.x / font.dwFontSize.X, cursorPos.y / font.dwFontSize.Y);
	}
#endif
/// REGION CLASS:
#ifdef TRUE
	Region::Region()
	{
		leftTop = { 0, 0 };
		rightBottom = { 0, 0 };
	}
	Region::Region(int flag)
	{
		valid = flag > 0;
	}
	Region::Region(Point corner, Point opposite)
	{
		Vector diff = (corner.getVector(opposite)).normalize();
		if (diff.x == POSITIVE && diff.y == POSITIVE)
		{
			leftTop = corner;
			rightBottom = opposite;
		}
		else if (diff.x == NEGATIVE && diff.y == NEGATIVE)
		{
			leftTop = opposite;
			rightBottom = corner;
		}
		else if (diff.x == POSITIVE && diff.y == NEGATIVE)
		{
			leftTop = { corner.x, opposite.y };
			rightBottom = { opposite.x, corner.y };
		}
		else if (diff.x == NEGATIVE && diff.y == POSITIVE)
		{
			leftTop = { opposite.x, corner.y };
			rightBottom = { corner.x, opposite.y };
		}
	}

	int Region::getWidth() { return ((rightBottom.x + 1) - leftTop.x); }
	int Region::getHeight() { return ((rightBottom.y + 1) - leftTop.y); }
	int Region::getArea()
	{
		return getWidth() * getHeight();
	}

	void Region::getPoints(Point* buffer, int bufferLen)
	{
		int i = 0;
		int width = getWidth();
		int height = getHeight();
		int diffX, diffY = 0;
		for (i = 0; i < bufferLen && i < (width*height); i++)
		{
			diffX = i % width;
			diffY = i / width;
			buffer[i] = Point(leftTop.x + diffX, leftTop.y + diffY);
		}
	}
#endif
/// PIXEL CLASS:
#ifdef TRUE
	Pixel::Pixel()
	{
		chr = DEFAULT_PIXEL.chr;
		pos = DEFAULT_PIXEL.pos;
		color = DEFAULT_PIXEL.color;
	}
	Pixel::Pixel(bool flag)
	{
		valid = false;
	}
	Pixel::Pixel(CHAR _character)
	{
		chr = _character;
		pos = DEFAULT_PIXEL.pos;
		color = DEFAULT_PIXEL.color;
	}
	Pixel::Pixel(Point _pos)
	{
		chr = DEFAULT_PIXEL.chr;
		pos = _pos;
		color = DEFAULT_PIXEL.color;
	}
	Pixel::Pixel(CHAR _character, Point _pos)
	{
		chr = _character;
		pos = _pos;
		color = DEFAULT_PIXEL.color;
	}
	Pixel::Pixel(CHAR _character, Point _pos, Brush _color)
	{
		chr = _character;
		pos = _pos;
		color = _color;
	}

	bool Pixel::operator==(Pixel other)
	{
		return (chr == other.chr && pos == other.pos && color == other.color);
	}
	bool Pixel::operator!=(Pixel other)
	{
		return !(*this == other);
	}
#endif
/// POINT CLASS:
#ifdef TRUE
	Point::Point()
	{
		x = 0; y = 0;
	}
	Point::Point(bool flag) { // Enable nullifaction of Point.
		valid = flag;
	}
	Point::Point(size_t _x, size_t _y)
	{
		x = _x;
		y = _y;
	}
	Point::Point(COORD coord)
	{
		x = coord.X;
		y = coord.Y;
	}

	COORD Point::toCoord()
	{
		return { (SHORT)x, (SHORT)y };
	}

	Vector Point::getVector(Point other)
	{
		return Vector(other.x - x, other.y - y);
	}
	Point Point::forceApply(Vector vector)
	{
		return Point(x + vector.x * vector.m, y + vector.y * vector.m);
	}

	bool Point::parsePoints(Point* buffer, int bufferLen, int* grid, int width, int height)
	{
		bool success = false;
		int bufferIndex = 0;
		int _x, _y = 0;
		for (_y = 0; _y < height; _y++)
		{
			for (_x = 0; _x < width; _x++)
			{
				if (bufferIndex < bufferLen)
				{
					if (*(grid + _y * width + _x) == POSITIVE)
					{
						buffer[bufferIndex] = Point(_x, _y);
						success = true;
					}
					else
					{
						buffer[bufferIndex] = INVALID_POINT;
					}
					bufferIndex++;
				}
			}
		}
		return success;
	}

	bool Point::operator==(Point other)
	{
		return (x == other.x && y == other.y);
	}
	bool Point::operator>(Point other)
	{
		return (x > other.x && y > other.y);
	}
	bool Point::operator<(Point other)
	{
		return (x < other.x && y < other.y);
	}
	bool Point::operator>=(Point other)
	{
		return (x >= other.x && y >= other.y);
	}
	bool Point::operator<=(Point other)
	{
		return (x <= other.x && y <= other.y);
	}
	Point Point::operator+(Point other)
	{
		return Point(x + other.x, y + other.y);
	}
	Point Point::operator-(Point other)
	{
		return Point(x - other.x, y - other.y);
	}
#endif
/// VECTOR CLASS:
#ifdef TRUE
	Vector::Vector()
	{
		x = 0; y = 0; m = 0;
	}
	Vector::Vector(int _x, int _y)
	{
		x = _x; y = _y; m = 0;
	}
	Vector::Vector(int _x, int _y, int _m)
	{
		x = _x; y = _y; m = _m;
	}
	Vector Vector::normalize()
	{
		return Vector((x < 0) ? -1 : (x > 0), (y < 0) ? -1 : (y > 0), 1);
	}
#endif
/// PALETTE CLASS:
#ifdef TRUE
	Palette::Palette() : Palette(DEFAULT_PALETTE_SIZE)
	{ }
	Palette::Palette(size_t _size)
	{
		size = _size;
		data = (BrushEx*)malloc(sizeof(BrushEx) * _size);
		for (size_t i = 0; i < _size; i++)
		{
			data[i] = DEFAULT_BRUSH_EX;
		}
	}
	void Palette::Destroy()
	{
		free(data);
	}

	bool Palette::SetEx(int id, BrushEx brush)
	{
		if (id < (int)size)
		{
			data[id] = brush;
			return true;
		}
		return false;
	}

	bool Palette::Set(int id, Brush brush)
	{
		return SetEx(id, BrushEx(brush, SPACE));
	}

	bool Palette::Exists(int id)
	{
		for (size_t i = 0; i < size; i++)
		{
			if (data[i] != DEFAULT_BRUSH_EX)
			{
				return true;
			}
		}
		return false;
	}
	bool Palette::Exists(BrushEx brush)
	{

		for (size_t i = 0; i < size; i++)
		{
			if (data[i] == brush)
			{
				return true;
			}
		}
		return false;
	}

	bool Palette::Exists(Brush brush)
	{
		return Exists(BrushEx(brush, SPACE));
	}

	bool Palette::Remove(int id)
	{
		if (id < (int)size && !Exists(id))
		{
			data[id] = DEFAULT_BRUSH_EX;
			return true;
		}
		return false;
	}

	Brush Palette::Get(int id)
	{
		return data[id].brush;
	}

	BrushEx Palette::GetEx(int id)
	{
		return data[id];
	}

#endif
/// TEXTURE CLASS:
#ifdef TRUE
//
// Programming notes:
// data[height][width] = {id}
//
	Texture::Texture() : Texture(DEFAULT_TEXTURE_WIDTH, DEFAULT_TEXTURE_HEIGHT, DEFAULT_PALETTE_SIZE)
	{ }

	Texture::Texture(size_t _width, size_t _height, size_t paletteSize)
	{
		width = _width;
		height = _height;
		palette = (Palette*)malloc(sizeof(Palette));
		*palette = Palette(paletteSize);
		initData();
	}

	void Texture::initData()
	{
		data = (int**)malloc(height * sizeof(int*));
		for (size_t i = 0; i < height; i++)
		{
			data[i] = (int*)malloc(width * sizeof(int));
			for (size_t j = 0; j < width; j++)
			{
				data[i][j] = 0;
			}
		}
	}

	void Texture::fromArray(int* arr)
	{
		size_t k = 0;
		for (size_t y = 0; y < height; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				data[y][x] = *(arr + y * width + x);
			}
		}
	}

	BrushEx Texture::At(Point p)
	{
		int value = 0;
		if (p < Point(width, height))
		{
			value = data[p.y][p.x];
			if (palette->Exists(value))
			{
				return palette->GetEx(value);
			}
			else return DEFAULT_BRUSH_EX;
		}
		return DEFAULT_BRUSH_EX;
	}

	void Texture::Destroy()
	{
		for (size_t i = 0; i < height; i++)
		{
			free(data[i]);
		}
		free(data);
		palette->Destroy();
		free(palette);
	}
#endif
/// GRAPHICS CLASS:
#ifdef TRUE
	Graphics::Graphics(bool flag)
	{
		valid = flag;
	}

	void Graphics::DrawLine(Point p1, Point p2, BrushEx brush)
	{
		Point start = Point((p1.x < p2.x) ? p1.x : p2.x, (p1.y < p2.y) ? p1.y : p2.y);
		int distance = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
		float slope = (((float)p1.y - p2.y)) / (((float)p1.x - p2.x));
		int prevHeight = start.y;
		for (size_t i = 0; i <= abs((int)p2.x - (int)p1.x); i++)
		{
			Point pos = Point(start.x + i, slope * i + start.y);
			console::drawPixel(brush.character, pos, brush.brush);
			for (size_t j = 0; j < (pos.y - prevHeight); j++)
			{
				console::drawPixel(brush.character, Point(pos.x, pos.y - j), brush.brush);
			}
			prevHeight = pos.y;
		}
		if (p1.x == p2.x)
		{
			for (size_t j = 0; j <= abs((int)p2.y - (int)p1.y); j++)
			{
				Point pos = Point(start.x, start.y + j);
				console::drawPixel(brush.character, pos, brush.brush);
			}
		}
	}

	void Graphics::DrawRect(Point c1, Point c2, size_t borderSize, BrushEx brush)
	{
		for (size_t borderOffset = 0; borderOffset < borderSize; borderOffset++)
		{
			Region region = Region(c1 + Point(borderOffset, borderOffset), c2 - Point(borderOffset, borderOffset));
			DrawLine(region.leftTop, region.leftTop + Point(region.getWidth(), 0), BrushEx(BACKGROUND_WHITE));
			DrawLine(region.leftTop, region.leftTop + Point(0, region.getHeight()), BrushEx(BACKGROUND_WHITE));
			DrawLine(region.rightBottom, region.leftTop - Point(region.getWidth(), 0), BrushEx(BACKGROUND_WHITE));
			DrawLine(region.rightBottom, region.leftTop - Point(0, region.getHeight()), BrushEx(BACKGROUND_WHITE));
		}
	}
	void Graphics::FillRect(Point c1, Point c2, BrushEx brush)
	{

	}

	void Graphics::DrawCircle()
	{

	}
	void Graphics::FillCircle()
	{

	}

#endif