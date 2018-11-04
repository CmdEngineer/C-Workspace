#include "ccl.h"

static size_t layers[] = { 0 };

void graphics::drawText(STR str, Point pos)
{
	Texture t = Texture(5, 5, 2);
	t.palette->Set(0, BACKGROUND_DEFAULT);
	t.palette->Set(1, BACKGROUND_WHITE);
	int diff;
	for (size_t i = 0; i < LEN(str); i++)
	{
		if (str[i] >= TEXT('a') && str[i] <= TEXT('z'))
		{
			diff = CHAR(str[i]) - TEXT('a');
			t.fromArray(TypeFace::size5x5::character[diff]);
			console::drawTexture(t, pos);
			pos.x += 6;
		}
		else if (str[i] >= TEXT('A') && str[i] <= TEXT('Z'))
		{
			diff = CHAR(str[i]) - TEXT('A');
			t.fromArray(TypeFace::size5x5::character[diff]);
			console::drawTexture(t, pos);
			pos.x += 6;
		}
		else pos.x += 6;
	}
}

void graphics::drawLine(Point p1, Point p2, BrushEx brush)
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

void graphics::drawRect(Point c1, Point c2, size_t borderSize, BrushEx brush)
{
	for (size_t i = 0; i < borderSize; i++)
	{
		Region r = Region(c1 + Point(i, i), c2 - Point(i, i));
		drawLine(r.leftTop, r.leftTop + Point(r.getWidth(), 0), brush);
		drawLine(r.leftTop, r.leftTop + Point(0, r.getHeight()), brush);
		drawLine(r.rightBottom, r.rightBottom - Point(r.getWidth(), 0), brush);
		drawLine(r.rightBottom, r.rightBottom - Point(0, r.getHeight()), brush);
	}
}
void graphics::fillRect(Point c1, Point c2, BrushEx brush)
{
	Region r = Region(c1, c2);
	console::drawRegion(r, brush);
}

void graphics::drawCircle(Point mid, size_t radius, Brush brush, int minDegree, int maxDegree)
{
	int x = 0, y = 0;
	for (size_t i = minDegree; i < maxDegree; i++)
	{
		x = radius * cos(i * M_PI / 180);
		y = radius * sin(i * M_PI / 180);
		console::drawPixel(SPACE, Point(mid.x + x, mid.y + y), brush);
	}
}
void graphics::fillCircle(Point mid, size_t radius, Brush brush, int minDegree, int maxDegree)
{
	int x = 0, y = 0;
	for (size_t j = 0; j < radius; j++)
	{
		for (size_t i = minDegree; i < maxDegree; i++)
		{
			x = j * cos(i * M_PI / 180);
			y = j * sin(i * M_PI / 180);
			console::drawPixel(SPACE, Point(mid.x + x, mid.y + y), brush);
		}
	}
}
graphics::Sprite::Sprite(Point pos, Palette palette)
{
	this->pos = pos;
	this->palette = palette;
	textures = std::vector<Texture>();
	index = 0;
}

void graphics::Sprite::setPalette(Palette palette)
{
	this->palette = palette;
}

void graphics::Sprite::draw()
{
	//console::drawTexture(textures[index], )
}

void graphics::Sprite::forward()
{

}

void graphics::Sprite::backward()
{

}

void graphics::Sprite::animate(time_t milliseconds, bool repeat)
{

}

void graphics::Sprite::animate(size_t to, time_t milliseconds, bool repeat)
{

}

void graphics::Sprite::animate(size_t from, size_t to, time_t milliseconds, bool repeat)
{

}
