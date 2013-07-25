#include "map.h"
#include <cstdlib>

Door::Door(int _x, int _y)
	: x(_x), y(_y), opened(false), sprite('+')
{
}

void Door::open()
{
	opened = true;
	sprite = '-';
}

void Door::close()
{
	opened = false;
	sprite = '+';
}

//------------------------------------------------------------------------------

Map::Map(int w, int h)
	: width(w), height(h), map(width * height, Cell('.', true))
{
}

bool Map::valid(int x, int y) const
{
	return (x >= 0 && x < width && y >= 0 && y < height);
}

bool Map::passable(int x, int y) const
{
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->opened;
		}
	}
	return cell(x, y).passable;
}

const Sprite & Map::sprite(int x, int y) const
{
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->sprite;
		}
	}
	return cell(x, y).sprite;
}

void Map::open_at(int x, int y)
{
	for(std::vector<Door>::iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			if(!door->opened) {
				door->open();
			}
		}
	}
}

void Map::close_at(int x, int y)
{
	for(std::vector<Door>::iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			if(door->opened) {
				door->close();
			}
		}
	}
}

Cell & Map::cell(int x, int y)
{
	static Cell empty_cell;
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
}

const Cell & Map::cell(int x, int y) const
{
	static Cell empty_cell;
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
}

void Map::generate()
{
    for(int i = 0; i < 10; ++i) {
        int x = rand() % width;
        int y = rand() % height;
        cell(x, y) = Cell('#', false);
    }
    for(int i = 0; i < 5; ++i) {
        int x1 = rand() % (width / 2);
        int y = rand() % height;
        int x2 = width / 2 + rand() % (width / 2);
        for(int x = x1; x <= x2; ++x) {
            cell(x, y) = Cell('#', false);
        }
		int door_x = x1 + rand() % (x2 - x1);
		cell(door_x, y) = Cell('.', true);
		doors.push_back(Door(door_x, y));
    }
    for(int i = 0; i < 5; ++i) {
        int x = rand() % (width / 2);
        int y1 = rand() % height;
        int y2 = height / 2 + rand() % (height / 2);
        for(int y = y1; y <= y2; ++y) {
            cell(x, y) = Cell('#', false);
        }
		int door_y = y1 + rand() % (y2 - y1);
		cell(x, door_y) = Cell('.', true);
		doors.push_back(Door(x, door_y));
    }
}
