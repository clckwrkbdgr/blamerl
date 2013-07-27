#include "game.h"
#include "log.h"
#include "util.h"
#include <cmath>
#include <cstdlib>

Control::Control(int v, bool running)
    : value(v), run(running)
{
}

// --------------------------------------------------------------------------

Game::Game()
	: state(MOVING), examining(false), player_x(0), player_y(0)
{
}

void Game::generate()
{
	map = Map(80, 23);
	map.generate();
	player_x = map.width / 2;
	player_y = map.height / 2;
}

bool Game::move_by(int shift_x, int shift_y)
{
    if(!map.valid(player_x + shift_x, player_y + shift_y)) {
        return false;
    }
    if(!map.passable(player_x + shift_x, player_y + shift_y)) {
        return false;
    }
    player_x += shift_x;
    player_y += shift_y;
	static int STEP = Cell::register_type(CellType('*', true, "your step"));
	map.update_cell_type(player_x, player_y, STEP);
    return true;
}

class MapPassabilityDetector : public PassabilityDetector {
public:
	MapPassabilityDetector(const Map & game_map) : map(game_map) {}
	virtual bool is_passable(int x, int y) const {
		return map.valid(x, y) && map.passable(x, y);
	}
private:
	const Map & map;
};

bool Game::move_to(int target_x, int target_y)
{
	if(!map.valid(target_x, target_y) || !map.passable(target_x, target_y)) {
		return false;
	}

	MapPassabilityDetector detector(map);
	Pathfinder pathfinder(player_x, player_y, target_x, target_y);
	bool found = pathfinder.find_path(&detector);

	if(found) {
		while(pathfinder.next()) {
            move_by(pathfinder.shift_x(), pathfinder.shift_y());
		}
	}

    return true;
}

bool Game::move_cursor_by(int shift_x, int shift_y)
{
    if(!map.valid(cursor_x + shift_x, cursor_y + shift_y)) {
        return false;
    }
    cursor_x += shift_x;
    cursor_y += shift_y;
	message = format("You see {0}.").arg(map.name(cursor_x, cursor_y));
    return true;
}

bool Game::show_cursor() const
{
	return (state == EXAMINING);
}

bool Game::process_moving(const Control & control)
{
	message = "";
	int shift_x = 0;
	int shift_y = 0;
	switch(control.value) {
		case Control::EXAMINE:
			state = EXAMINING;
			cursor_x = player_x;
			cursor_y = player_y;
			message = "You see yourself.";
			break;
		case Control::LEFT: shift_x = -1; shift_y = 0; break;
		case Control::DOWN: shift_x = 0; shift_y = 1; break;
		case Control::UP: shift_x = 0; shift_y = -1; break;
		case Control::RIGHT: shift_x = 1; shift_y = 0; break;
		case Control::DOWN_LEFT: shift_x = -1; shift_y = 1; break;
		case Control::DOWN_RIGHT: shift_x = 1; shift_y = 1; break;
		case Control::UP_LEFT: shift_x = -1; shift_y = -1; break;
		case Control::UP_RIGHT: shift_x = 1; shift_y = -1; break;
		case Control::OPEN: state = OPENING; break;
		case Control::CLOSE: state = CLOSING; break;
		case Control::QUIT: return false;
		default: break;
	}

	if(shift_x != 0 || shift_y != 0) {
		if(control.run) {
			bool running = true;
			while(running) {
				running = move_by(shift_x, shift_y);
			}
		} else {
			move_by(shift_x, shift_y);
		}
	}
	return true;
}

bool Game::process_examining(const Control & control)
{
	int shift_x = 0;
	int shift_y = 0;
	switch(control.value) {
		case Control::CANCEL:
			state = MOVING;
			break;
		case Control::EXAMINE:
			state = MOVING;
			break;
		case Control::LEFT: shift_x = -1; shift_y = 0; break;
		case Control::DOWN: shift_x = 0; shift_y = 1; break;
		case Control::UP: shift_x = 0; shift_y = -1; break;
		case Control::RIGHT: shift_x = 1; shift_y = 0; break;
		case Control::DOWN_LEFT: shift_x = -1; shift_y = 1; break;
		case Control::DOWN_RIGHT: shift_x = 1; shift_y = 1; break;
		case Control::UP_LEFT: shift_x = -1; shift_y = -1; break;
		case Control::UP_RIGHT: shift_x = 1; shift_y = -1; break;
		case Control::TARGET: {
			move_to(cursor_x, cursor_y);
			state = MOVING;
			break;
		}
		default: break;
	}

	if(shift_x != 0 || shift_y != 0) {
		move_cursor_by(shift_x, shift_y);
	}
	return true;
}

bool Game::process_opening(const Control & control)
{
	int shift_x = 0;
	int shift_y = 0;
	switch(control.value) {
		case Control::CANCEL:
			state = MOVING;
			break;
		case Control::LEFT: shift_x = -1; shift_y = 0; break;
		case Control::DOWN: shift_x = 0; shift_y = 1; break;
		case Control::UP: shift_x = 0; shift_y = -1; break;
		case Control::RIGHT: shift_x = 1; shift_y = 0; break;
		case Control::DOWN_LEFT: shift_x = -1; shift_y = 1; break;
		case Control::DOWN_RIGHT: shift_x = 1; shift_y = 1; break;
		case Control::UP_LEFT: shift_x = -1; shift_y = -1; break;
		case Control::UP_RIGHT: shift_x = 1; shift_y = -1; break;
		default: break;
	}

	if(shift_x != 0 || shift_y != 0) {
		map.open_at(player_x + shift_x, player_y + shift_y);
	}
	state = MOVING;
	return true;
}

bool Game::process_closing(const Control & control)
{
	int shift_x = 0;
	int shift_y = 0;
	switch(control.value) {
		case Control::CANCEL:
			state = MOVING;
			break;
		case Control::LEFT: shift_x = -1; shift_y = 0; break;
		case Control::DOWN: shift_x = 0; shift_y = 1; break;
		case Control::UP: shift_x = 0; shift_y = -1; break;
		case Control::RIGHT: shift_x = 1; shift_y = 0; break;
		case Control::DOWN_LEFT: shift_x = -1; shift_y = 1; break;
		case Control::DOWN_RIGHT: shift_x = 1; shift_y = 1; break;
		case Control::UP_LEFT: shift_x = -1; shift_y = -1; break;
		case Control::UP_RIGHT: shift_x = 1; shift_y = -1; break;
		default: break;
	}

	if(shift_x != 0 || shift_y != 0) {
		map.close_at(player_x + shift_x, player_y + shift_y);
	}
	state = MOVING;
	return true;
}

bool Game::process_control(const Control & control)
{
	switch(state) {
		case MOVING: return process_moving(control);
		case EXAMINING: return process_examining(control);
		case OPENING: return process_opening(control);
		case CLOSING: return process_closing(control);
		default: return true;
	}
	return true;
}

int Game::width() const
{
	return map.width;
}

int Game::height() const
{
	return map.height;
}

const Sprite & Game::sprite(int x, int y) const
{
	return map.sprite(x, y);
}


