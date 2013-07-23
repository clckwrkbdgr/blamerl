#pragma once
#include "map.h"

class Control {
public:
    enum Type { UNKNOWN,
        LEFT, RIGHT, DOWN, UP, DOWN_LEFT, DOWN_RIGHT, UP_LEFT, UP_RIGHT,
        RUN_LEFT, RUN_RIGHT, RUN_DOWN, RUN_UP,
        RUN_DOWN_LEFT, RUN_DOWN_RIGHT, RUN_UP_LEFT, RUN_UP_RIGHT,
        EXAMINE,
        QUIT
    };

    int value;
    bool run;
    Control(int v = UNKNOWN);
};


class Game {
public:
	Map map;
    bool examining;
    int cursor_x, cursor_y;
	int player_x, player_y;

	Game(int w, int h);
	bool process_control(const Control & control);
	int width() const;
	int height() const;
	Sprite & sprite(int x, int y);
	const Sprite & sprite(int x, int y) const;
    bool move_by(int shift_x, int shift_y);
    bool move_cursor_by(int shift_x, int shift_y);
};

