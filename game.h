#pragma once
#include "map.h"

class Control {
public:
    enum Type { UNKNOWN, CANCEL,
        LEFT, RIGHT, DOWN, UP, DOWN_LEFT, DOWN_RIGHT, UP_LEFT, UP_RIGHT,
        EXAMINE, TARGET, OPEN, CLOSE, 
        QUIT
    };

    int value;
    bool run;
    Control(int v = UNKNOWN, bool running = false);
};


class Game {
public:
	enum State { MOVING, EXAMINING, OPENING, CLOSING } state;
	Map map;
    bool examining;
    int cursor_x, cursor_y;
	int player_x, player_y;
	std::string message;

	Game(int w, int h);
	bool process_control(const Control & control);
	int width() const;
	int height() const;
	const Sprite & sprite(int x, int y) const;
	bool show_cursor() const;

    bool move_by(int shift_x, int shift_y);
    bool move_cursor_by(int shift_x, int shift_y);
    bool move_to(int target_x, int target_y);

	bool process_moving(const Control & control);
	bool process_examining(const Control & control);
	bool process_opening(const Control & control);
	bool process_closing(const Control & control);
};

