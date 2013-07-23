#pragma once
class Game;
class Control;

class Console {
public:
	int width, height;
	Console();
	~Console();

	void draw_game(const Game & game);
	Control get_control();
};

