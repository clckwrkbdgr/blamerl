#include "map.h"
#include <cstdlib>

CellType::CellType(const Sprite & _sprite, bool _passable, bool _transparent, const std::string & cell_name)
	: sprite(_sprite), passable(_passable), transparent(_transparent), name(cell_name)
{
}

CellType::CellType()
	: sprite(' '), passable(false), transparent(false)
{
}

Cell::Cell(int cell_type)
	: type(cell_type), visible(false), seen(false)
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

bool Cell::transparent() const
{
	return types()[type].transparent;
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

static Cell default_cell = Cell::register_type(CellType(' ', false, true, "void"));

Map::Map()
	: width(0), height(0)
{
}

Map::Map(int w, int h)
	: width(w), height(h),
	map(width * height, Cell::register_type(CellType('.', true, true, "floor")))
{
}

bool Map::valid(int x, int y) const
{
	return (x >= 0 && x < width && y >= 0 && y < height);
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

