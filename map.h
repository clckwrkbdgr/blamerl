#pragma once
#include <vector>
#include <string>
#include <map>

typedef int Sprite;

struct CellType {
    Sprite sprite;
    bool passable;
	std::string name;
    CellType(const Sprite & _sprite = ' ', bool _passable = false, const std::string & cell_name = std::string());
};

struct Cell {
	int type;

	Cell(int cell_type);
	const Sprite & sprite() const;
	bool passable() const;
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

