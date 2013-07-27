#include "map.h"
#include <cstdlib>

CellType::CellType(const Sprite & _sprite, bool _passable, const std::string & cell_name)
	: sprite(_sprite), passable(_passable), name(cell_name)
{
}

Cell::Cell(int cell_type)
	: type(cell_type)
{
}

std::vector<CellType> & Cell::types()
{
	static std::vector<CellType> map;
	return map;
}

const Sprite & Cell::sprite() const
{
	return types()[type].sprite;
}

bool Cell::passable() const
{
	return types()[type].passable;
}

const std::string & Cell::name() const
{
	return types()[type].name;
}

int Cell::register_type(const CellType & cell_type)
{
	types().push_back(cell_type);
	return types().size() - 1;
}

//------------------------------------------------------------------------------

Door::Door(int _x, int _y)
	: x(_x), y(_y), opened(false), sprite('+')
{
	name = "a door";
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

static Cell default_cell = Cell::register_type(CellType(' ', false, "void"));

Map::Map()
	: width(0), height(0)
{
}

Map::Map(int w, int h)
	: width(w), height(h),
	map(width * height, Cell::register_type(CellType('.', true, "floor")))
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
	return cell(x, y).passable();
}

const Sprite & Map::sprite(int x, int y) const
{
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->sprite;
		}
	}
	return cell(x, y).sprite();
}

const std::string & Map::name(int x, int y) const
{
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->name;
		}
	}
	return cell(x, y).name();
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
	if(!valid(x, y)) {
		return default_cell;
	}
	return map[x + y * width];
}

const Cell & Map::cell(int x, int y) const
{
	if(!valid(x, y)) {
		return default_cell;
	}
	return map[x + y * width];
}

void Map::generate()
{
	Cell wall = Cell::register_type(CellType('#', false, "a wall"));
	Cell brick_wall = Cell::register_type(CellType('#', false, "a brick wall"));
	Cell wooden_wall = Cell::register_type(CellType('#', false, "a wooden wall"));
	Cell doorway = Cell::register_type(CellType('.', true, "a doorway"));

    for(int i = 0; i < 10; ++i) {
        int x = rand() % width;
        int y = rand() % height;
        cell(x, y) = wall;
    }
    for(int i = 0; i < 5; ++i) {
        int x1 = rand() % (width / 2);
        int y = rand() % height;
        int x2 = width / 2 + rand() % (width / 2);
        for(int x = x1; x <= x2; ++x) {
            cell(x, y) = brick_wall;
        }
		int door_x = x1 + rand() % (x2 - x1);
		cell(door_x, y) = doorway;
		doors.push_back(Door(door_x, y));
    }
    for(int i = 0; i < 5; ++i) {
        int x = rand() % (width / 2);
        int y1 = rand() % height;
        int y2 = height / 2 + rand() % (height / 2);
        for(int y = y1; y <= y2; ++y) {
            cell(x, y) = wooden_wall;
        }
		int door_y = y1 + rand() % (y2 - y1);
		cell(x, door_y) = doorway;
		doors.push_back(Door(x, door_y));
    }
}

void Map::update_cell_type(int x, int y, int new_type)
{
	if(valid(x, y)) {
		cell(x, y).type = new_type;
	}
}
