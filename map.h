#pragma once
#include <vector>
#include <string>
#include <map>

typedef int Sprite;

struct CellType {
    Sprite sprite;
    bool passable;
	bool transparent;
	std::string name;
    CellType();
    CellType(const Sprite & _sprite, bool _passable, bool _transparent, const std::string & cell_name);
};

struct Cell {
	int type;
	bool visible;
	bool seen;

	Cell(int cell_type);
	const Sprite & sprite() const;
	bool passable() const;
	bool transparent() const;
	const std::string & name() const;

	static std::vector<CellType> & types();
	static int register_type(const CellType & cell_type);
};

class Map {
public:
	int width, height;
	std::vector<Cell> map;

	Map();
	Map(int w, int h);

	bool valid(int x, int y) const;

	Cell & cell(int x, int y);
	const Cell & cell(int x, int y) const;
};

