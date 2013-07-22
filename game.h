#pragma once

class Game {
public:
	int width, height;
	int x, y;
	Game(int w, int h);
	bool process_control(int ch);
};

