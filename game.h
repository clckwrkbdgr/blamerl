#pragma once
#include <vector>

class Map {
public:
	int width, height;
	std::vector<int> map;

	Map(int w, int h);

	bool valid(int x, int y) const;
	int & get(int x, int y);
	const int & get(int x, int y) const;

};

class Game {
public:
	Map map;
	int player_x, player_y;

	Game(int w, int h);
	bool process_control(int ch);
	int width() const;
	int height() const;
	int & get(int x, int y);
	const int & get(int x, int y) const;
};

