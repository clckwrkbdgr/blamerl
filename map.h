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

struct Door {
	int x, y;
	bool opened;
	Sprite sprite;
	std::string name;
	Door(int _x, int _y);
	void open();
	void close();
};

class Map {
public:
	int width, height;
	std::vector<Cell> map;
	std::vector<Door> doors;

	Map(int w, int h);

	bool valid(int x, int y) const;
	bool passable(int x, int y) const;
	const Sprite & sprite(int x, int y) const;
	const std::string & name(int x, int y) const;

	void open_at(int x, int y);
	void close_at(int x, int y);

	void generate();
private:
	Cell default_cell;
	Cell & cell(int x, int y);
	const Cell & cell(int x, int y) const;
};

