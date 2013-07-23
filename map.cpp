#include "map.h"

Map::Map(int w, int h)
	: width(w), height(h), map(width * height, 0)
{
}

bool Map::valid(int x, int y) const
{
	return (x >= 0 && x < width && y >= 0 && y < height);
}

int & Map::get(int x, int y)
{
	static int empty_cell = int();
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
}

const int & Map::get(int x, int y) const
{
	static int empty_cell = int();
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
}


