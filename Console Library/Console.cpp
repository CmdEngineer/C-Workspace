#include "Console.h"



/***
**
** CONSOLE CLASS:
**
***/
int Console::initConsole()
{
	ZeroMemory(&screen, sizeof(screen));
	ZeroMemory(&font, sizeof(font));
	input = GetStdHandle(STD_INPUT_HANDLE);
	output = GetStdHandle(STD_OUTPUT_HANDLE);
	consoleWindow = GetConsoleWindow();
	error((consoleWindow == INVALID_HANDLE_VALUE), TEXT("Console Error"), TEXT("GetConsoleWindow"));
	error((input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE), TEXT("Console Error"), TEXT("GetStdHandle"));
	//SetConsoleMode(input, ~ENABLE_ECHO_INPUT);
	GetConsoleScreenBufferInfo(output, &screen);
	GetCurrentConsoleFont(output, false, &font);
	// End initalization
	initated = true;
	ShowWindow(consoleWindow, SW_MAXIMIZE);
	return true;
}

int Console::error(bool condition, STR title, STR content, bool exitProgram)
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

void Console::clear(Point from)
{
	if (!initated) initConsole();
	setCharacter(from, SPACE, screen.dwSize.X * screen.dwSize.Y);
	setCursorPos(from);
}

void Console::clear()
{
	if (!initated) initConsole();
	setCursorPos({ 0, 0 });
	setCharacter(SPACE, screen.dwSize.X * screen.dwSize.Y);
}

Pixel Console::getPixel(Point pos)
{
	if (!initated) initConsole();
	CHAR_INFO ci;
	SMALL_RECT rect = { 0,0,0,0 };
	ReadConsoleOutput(output, &ci, { 1,1 }, pos.toCoord(), &rect);
	return Pixel(ci.Char.UnicodeChar, pos, ci.Attributes);
}
void Console::getPixels(Region region, Pixel* buffer, int bufferLen)
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

void Console::drawPixel(CHAR character, Point pos, Brush color)
{
	drawPixel(Pixel(character, pos, color));
}

void Console::drawPixel(Pixel pixel)
{
	if (!initated) initConsole();
	if (!FAST_DRAW || getPixel(pixel.pos) != pixel)
	{
		Point prevPos = getCursorPos();
		FillConsoleOutputCharacter(output, pixel.chr, 1, pixel.pos.toCoord(), &status);
		FillConsoleOutputAttribute(output, pixel.color, 1, pixel.pos.toCoord(), &status);
		setCursorPos(prevPos);
	}
}

void Console::drawRegion(Region r, BrushEx brush)
{
	Point* buffer = (Point*)malloc(sizeof(Point) * r.getArea());
	r.getPoints(buffer, r.getArea());
	for (int i = 0; i < r.getArea(); i++)
		drawPixel(brush.character, buffer[i], brush.brush);
	free(buffer);
}

void Console::drawRegion(Region r, Brush color)
{
	Point* buffer = (Point*)malloc(sizeof(Point) * r.getArea());
	r.getPoints(buffer, r.getArea());
	for (int i = 0; i < r.getArea(); i++)
		drawPixel(SPACE, buffer[i], color);
	free(buffer);
}

void Console::setAttribute(Point pos, Brush color, int length)
{
	if (!initated) initConsole();
	Point prevPos = getCursorPos();
	SetConsoleCursorPosition(output, pos.toCoord());
	FillConsoleOutputAttribute(output, color, length, pos.toCoord(), &status);
	setCursorPos(prevPos);
}
void Console::setAttribute(Brush color, int length)
{
	setAttribute(getCursorPos(), color, length);
}

void Console::setCharacter(Point pos, CHAR character, int length)
{
	if (!initated) initConsole();
	Point prevPos = getCursorPos();
	SetConsoleCursorPosition(output, pos.toCoord());
	FillConsoleOutputCharacter(output, character, length, pos.toCoord(), &status);
	setCursorPos(prevPos);
}
void Console::setCharacter(CHAR character, int length)
{
	setCharacter(getCursorPos(), character, length);
}

Console Console::operator >> (STR str)
{
	print(str);
	return *this;
}
void Console::print(STR string)
{
	if (!initated) initConsole();
	WriteConsoleW(output, string, LEN(string), &status, NULL);
}
void Console::print(STR string, Point pos)
{
	if (!initated) initConsole();
	size_t i;
	for (i = 0; i < LEN(string); i++)
	{
		setCharacter(pos, string[i]);
		if (pos.x <= (size_t)screen.dwSize.X)
		{
			pos.x += 1;
		}
		else {
			pos.x = 0;
			pos.y += 1;
		}
	}
}

Console Console::operator<<(STR str)
{
	insert(str);
	return *this;
}
void Console::insert(STR string)
{
	if (!initated) initConsole();
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

INPUT_RECORD Console::charAsInput(CHAR chr)
{
	INPUT_RECORD ir;
	ir.EventType = KEY_EVENT;
	ir.Event.KeyEvent.bKeyDown = TRUE;
	ir.Event.KeyEvent.uChar.UnicodeChar = chr;
	ir.Event.KeyEvent.wRepeatCount = 1;
	return ir;
}
Point Console::getCursorPos()
{
	CONSOLE_SCREEN_BUFFER_INFO sb;
	GetConsoleScreenBufferInfo(output, &sb);
	return sb.dwCursorPosition;
}

void Console::setCursorPos(Point pos)
{
	SetConsoleCursorPosition(output, pos.toCoord());
}
Point Console::getCursorPoint()
{
	GetCursorPos(&cursorPos);
	ScreenToClient(consoleWindow, &cursorPos);
	return Point(cursorPos.x / font.dwFontSize.X, cursorPos.y / font.dwFontSize.Y);
}

/***
**
** REGION CLASS:
**
***/

Region::Region()
{
	leftTop = { 0, 0 };
	rightBottom = { 0, 0 };
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
int Region::getHeight() { return ((rightBottom.y + 1) - leftTop.y);}
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

/***
**
** PIXEL CLASS:
**
***/
Pixel::Pixel()
{
	chr  = DEFAULT_PIXEL.chr;
	pos   = DEFAULT_PIXEL.pos;
	color = DEFAULT_PIXEL.color;
}
Pixel::Pixel(bool flag)
{
	valid = false;
}
Pixel::Pixel(CHAR _character)
{
	chr  = _character;
	pos   = DEFAULT_PIXEL.pos;
	color = DEFAULT_PIXEL.color;
}
Pixel::Pixel(Point _pos)
{
	chr   = DEFAULT_PIXEL.chr;
	pos   = _pos;
	color = DEFAULT_PIXEL.color;
}
Pixel::Pixel(CHAR _character, Point _pos)
{
	chr  = _character;
	pos   = _pos;
	color = DEFAULT_PIXEL.color;
}
Pixel::Pixel(CHAR _character, Point _pos, Brush _color)
{
	chr   = _character;
	pos   = _pos;
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

/***
**
** POINT CLASS:
**
***/
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

/***
**
** VECTOR CLASS:
**
***/
Vector::Vector()
{
	x = 0; y = 0;
}
Vector::Vector(int _x, int _y)
{
	x = _x; y = _y;
}
Vector Vector::normalize()
{
	return Vector({ (x < 0) ? -1 : (x > 0), (y < 0) ? -1 : (y > 0) });
}