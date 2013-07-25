#pragma once
#include <vector>

typedef int Sprite;

struct Cell {
    Sprite sprite;
    bool passable;
    Cell(const Sprite & _sprite = ' ', bool _passable = false) : sprite(_sprite), passable(_passable) {}
};

struct Door {
	int x, y;
	bool opened;
	Sprite sprite;
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

	void open_at(int x, int y);
	void close_at(int x, int y);

	void generate();
private:
	Cell & cell(int x, int y);
	const Cell & cell(int x, int y) const;
};

