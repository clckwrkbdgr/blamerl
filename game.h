#pragma once
#include <vector>

class Game {
public:
	int width, height;
	std::vector<int> map;

	bool valid(int x, int y) const;
	int & get(int x, int y);
	const int & get(int x, int y) const;

	int player_x, player_y;
	Game(int w, int h);
	bool process_control(int ch);
};

