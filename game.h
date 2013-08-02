#pragma once
#include "map.h"
#include <list>

class Control {
public:
    enum Type { UNKNOWN, CANCEL,
        LEFT = 'h', RIGHT = 'l', DOWN = 'j', UP = 'k', DOWN_LEFT = 'b', DOWN_RIGHT = 'n', UP_LEFT = 'y', UP_RIGHT = 'u',
        EXAMINE = 'x', TARGET = '.', OPEN = 'o', CLOSE = 'c', SUICIDE = 'Q',
        QUIT = 'q'
    };

    int value;
    bool run;
    Control(int v = UNKNOWN, bool running = false);
};

struct Player {
	int x, y;
	int sight;
	Player(int player_x = 0, int player_y = 0);
};

class Game {
public:
	enum State { MOVING, RUNNING, EXAMINING, OPENING, CLOSING, SUICIDING } state;
	Map map;
	bool do_save;
    bool examining;
    int cursor_x, cursor_y;
    int world_x, world_y;
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
	bool transparent(int x, int y) const;
	bool show_cursor() const;

    bool has_auto_control() const;
    Control auto_control;
    std::list<Control> auto_control_list;
    Control get_auto_control() const;

	void open_at(int x, int y);
	void close_at(int x, int y);

    bool move_by(int shift_x, int shift_y);
    bool move_cursor_by(int shift_x, int shift_y);
    bool move_to(int target_x, int target_y);

	void invalidate_fov();

	bool process_moving(const Control & control);
	bool process_running(const Control & control);
	bool process_examining(const Control & control);
	bool process_opening(const Control & control);
	bool process_closing(const Control & control);
	bool process_suicide(const Control & control);
};

