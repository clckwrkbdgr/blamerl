#pragma once
#include <vector>
#include <string>

typedef int Sprite;

struct Cell {
    Sprite sprite;
    bool passable;
	std::string name;
    Cell(const Sprite & _sprite = ' ', bool _passable = false, const std::string & cell_name = std::string());
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
	Cell & cell(int x, int y);
	const Cell & cell(int x, int y) const;
};

