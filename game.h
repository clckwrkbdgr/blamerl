#pragma once
#include "map.h"

class Control {
public:
    enum Type { UNKNOWN, CANCEL,
        LEFT, RIGHT, DOWN, UP, DOWN_LEFT, DOWN_RIGHT, UP_LEFT, UP_RIGHT,
        EXAMINE, TARGET, OPEN, CLOSE, SUICIDE,
        QUIT
    };

    int value;
    bool run;
    Control(int v = UNKNOWN, bool running = false);
};


struct Door {
	int x, y;
	bool opened;
	Sprite sprite;
	std::string name;
	Door(int _x, int _y);
	void open();
	void close();
};

struct Player {
	int x, y;
	Player(int player_x = 0, int player_y = 0);
};

class Game {
public:
	enum State { MOVING, EXAMINING, OPENING, CLOSING, SUICIDING } state;
	Map map;
	std::vector<Door> doors;
	bool do_save;
    bool examining;
    int cursor_x, cursor_y;
	Player player;
	std::string message;

	Game();
	void generate();

	bool process_control(const Control & control);
	int width() const;
	int height() const;
	const Sprite & sprite(int x, int y) const;
	const std::string & name(int x, int y) const;
	bool passable(int x, int y) const;
	bool show_cursor() const;

	void open_at(int x, int y);
	void close_at(int x, int y);

    bool move_by(int shift_x, int shift_y);
    bool move_cursor_by(int shift_x, int shift_y);
    bool move_to(int target_x, int target_y);

	bool process_moving(const Control & control);
	bool process_examining(const Control & control);
	bool process_opening(const Control & control);
	bool process_closing(const Control & control);
	bool process_suicide(const Control & control);
};

