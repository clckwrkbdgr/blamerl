#pragma once
#include "map.h"

class Game {
public:
	Map map;
	int player_x, player_y;

	Game(int w, int h);
	bool process_control(int ch);
	int width() const;
	int height() const;
	Sprite & sprite(int x, int y);
	const Sprite & sprite(int x, int y) const;
};

