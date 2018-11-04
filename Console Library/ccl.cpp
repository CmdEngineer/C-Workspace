#include "ccl.h"

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

bool Region::contains(Point pos)
{
	return (pos >= leftTop && pos <= rightBottom);
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
	chr = DEFAULT_CHARACTER;
	pos = console::getCursorPoint();
	color = DEFAULT_BRUSH;
}
Pixel::Pixel(bool flag)
{
	valid = false;
}
Pixel::Pixel(CHAR _character)
{
	chr = _character;
	pos = console::getCursorPoint();
	color = DEFAULT_BRUSH;
}
Pixel::Pixel(Point _pos)
{
	chr = DEFAULT_CHARACTER;
	pos = _pos;
	color = DEFAULT_BRUSH;
}
Pixel::Pixel(Point _pos, CHAR _character)
{
	chr = _character;
	pos = _pos;
	color = DEFAULT_BRUSH;
}
Pixel::Pixel(Point _pos, CHAR _character, Brush _color)
{
	chr = _character;
	pos = _pos;
	color = _color;
}
Pixel::Pixel(Point _pos, BrushEx brush)
{
	chr = brush.character;
	pos = _pos;
	color = brush.brush;
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
	return{ (SHORT)x, (SHORT)y };
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
		data[i] = BrushEx(BACKGROUND_WHITE, SPACE, false);
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