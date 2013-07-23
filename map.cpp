#include "map.h"

Map::Map(int w, int h)
	: width(w), height(h), map(width * height, Cell('.', true))
{
}

bool Map::valid(int x, int y) const
{
	return (x >= 0 && x < width && y >= 0 && y < height);
}

Cell & Map::get(int x, int y)
{
	static Cell empty_cell;
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
}

const Cell & Map::get(int x, int y) const
{
	static Cell empty_cell;
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
}

