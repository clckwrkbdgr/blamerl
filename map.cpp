#include "map.h"
#include <cstdlib>

CellType::CellType(const Sprite & _sprite, bool _passable, bool _transparent, const std::string & cell_name)
	: sprite(_sprite), passable(_passable), transparent(_transparent), name(cell_name)
{
}

CellType::CellType()
	: sprite(' '), passable(false), transparent(true), name("void")
{
}

Cell::Cell(int cell_type)
	: type(cell_type), visible(false), seen(false)
{
}

//------------------------------------------------------------------------------

unsigned Map::register_type(const CellType & cell_type)
{
	cell_types.push_back(cell_type);
	return cell_types.size() - 1;
}

const CellType & Map::get_cell_type(const Cell & cell) const
{
    return get_cell_type(cell.type);
}

const CellType & Map::get_cell_type(unsigned cell_type_id) const
{
    if(cell_type_id >= cell_types.size()) {
        static CellType void_cell;
        return void_cell;
    }
    return cell_types[cell_type_id];
}

Map::Map()
	: width(0), height(0)
{
}

Map::Map(int w, int h)
	: width(w), height(h), map(width * height)
{
}

bool Map::valid(int x, int y) const
{
	return (x >= 0 && x < width && y >= 0 && y < height);
}

void Map::fill(const Cell & filler)
{
    for(std::vector<Cell>::iterator c = map.begin(); c != map.end(); ++c) {
        *c = filler;
    }
}

Cell & Map::cell(int x, int y)
{
	if(!valid(x, y)) {
        throw OutOfMapException(x, y);
	}
	return map[x + y * width];
}

const Cell & Map::cell(int x, int y) const
{
	if(!valid(x, y)) {
        throw OutOfMapException(x, y);
	}
	return map[x + y * width];
}

