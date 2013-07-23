#pragma once
#include <vector>

typedef int Sprite;

struct Cell {
    Sprite sprite;
    bool passable;
    Cell(const Sprite & _sprite = ' ', bool _passable = false) : sprite(_sprite), passable(_passable) {}
};

class Map {
public:
	int width, height;
	std::vector<Cell> map;

	Map(int w, int h);

	bool valid(int x, int y) const;
	Cell & cell(int x, int y);
	const Cell & cell(int x, int y) const;
};

