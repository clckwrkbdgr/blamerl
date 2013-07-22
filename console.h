#pragma once
class Game;

class Console {
public:
	int width, height;
	Console();
	~Console();

	void draw_game(const Game & game);
	int get_control();
};

