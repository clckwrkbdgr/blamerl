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

Door::Door(int _x, int _y)
	: x(_x), y(_y), opened(false), sprite('+')
{
	name = "a door";
}

void Door::open()
{
	opened = true;
	sprite = '-';
}

void Door::close()
{
	opened = false;
	sprite = '+';
}

//------------------------------------------------------------------------------

Player::Player(int player_x, int player_y)
	: x(player_x), y(player_y), sight(5)
{
}

//------------------------------------------------------------------------------

Game::Game()
	: state(MOVING), do_save(true), examining(false)
{
}

void Game::generate()
{
	map = Map(80, 23);

	Cell wall = Cell::register_type(CellType('#', false, "a wall"));
	Cell brick_wall = Cell::register_type(CellType('#', false, "a brick wall"));
	Cell wooden_wall = Cell::register_type(CellType('#', false, "a wooden wall"));
	Cell doorway = Cell::register_type(CellType('.', true, "a doorway"));

    for(int i = 0; i < 10; ++i) {
        int x = rand() % map.width;
        int y = rand() % map.height;
        map.cell(x, y) = wall;
    }
    for(int i = 0; i < 5; ++i) {
        int x1 = rand() % (map.width / 2);
        int y = rand() % map.height;
        int x2 = map.width / 2 + rand() % (map.width / 2);
        for(int x = x1; x <= x2; ++x) {
            map.cell(x, y) = brick_wall;
        }
		int door_x = x1 + rand() % (x2 - x1);
		map.cell(door_x, y) = doorway;
		doors.push_back(Door(door_x, y));
    }
    for(int i = 0; i < 5; ++i) {
        int x = rand() % (map.width / 2);
        int y1 = rand() % map.height;
        int y2 = map.height / 2 + rand() % (map.height / 2);
        for(int y = y1; y <= y2; ++y) {
            map.cell(x, y) = wooden_wall;
        }
		int door_y = y1 + rand() % (y2 - y1);
		map.cell(x, door_y) = doorway;
		doors.push_back(Door(x, door_y));
    }

	player.x = map.width / 2;
	player.y = map.height / 2;
}

bool Game::move_by(int shift_x, int shift_y)
{
    if(!map.valid(player.x + shift_x, player.y + shift_y)) {
        return false;
    }
    if(!passable(player.x + shift_x, player.y + shift_y)) {
        return false;
    }
    player.x += shift_x;
    player.y += shift_y;
	static int STEP = Cell::register_type(CellType('*', true, "your step"));
	map.cell(player.x, player.y) = STEP;
    return true;
}

class MapPassabilityDetector : public PassabilityDetector {
public:
	MapPassabilityDetector(const Game & _game) : game(_game) {}
	virtual bool is_passable(int x, int y) const {
		return game.map.valid(x, y) && game.passable(x, y);
	}
private:
	const Game & game;
};

bool Game::move_to(int target_x, int target_y)
{
	if(!map.valid(target_x, target_y) || !passable(target_x, target_y)) {
		return false;
	}

	MapPassabilityDetector detector(*this);
	Pathfinder pathfinder(player.x, player.y, target_x, target_y);
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
	message = format("You see {0}.").arg(name(cursor_x, cursor_y));
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
			cursor_x = player.x;
			cursor_y = player.y;
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
		case Control::OPEN: state = OPENING; return true;
		case Control::CLOSE: state = CLOSING; return true;
		case Control::SUICIDE:
		{
			state = SUICIDING;
			message = "Are you really want to commit suicide? (Y/N)";
			return true;
		}
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

bool Game::process_suicide(const Control & control)
{
	int shift_x = 0;
	int shift_y = 0;
	log("Suicide: {0}").arg(do_save);
	log("Control is UP_LEFT: {0}").arg(control.value == Control::UP_LEFT);
	switch(control.value) {
		case Control::CANCEL:
			state = MOVING;
			message = "Well okay then.";
			break;
		case Control::UP_LEFT: do_save = false; return false;
		default: break;
	}
	log("Suicide: {0}").arg(do_save);

	state = MOVING;
	message = "Well okay then.";
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
		open_at(player.x + shift_x, player.y + shift_y);
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
		close_at(player.x + shift_x, player.y + shift_y);
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
		case SUICIDING: return process_suicide(control);
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
	static Sprite CANNOT_SEE = ' ';
	int dx = std::abs(x - player.x);
	int dy = std::abs(y - player.y);
	int distance = int(std::sqrt(dx * dx + dy * dy));
	bool can_see = distance <= player.sight;
	if(can_see) {
		int deltax = x - player.x;
		int deltay = y - player.y;
		double error = 0.0;
		int iy = deltay > 0 ? 1 : -1;
		int ix = deltax > 0 ? 1 : -1;
		if(dx > dy) {
			double delta_error = std::abs(double(deltay) / double(deltax));
			int cy = player.y;
			for(int cx = player.x; cx != x; cx += ix) {
				if(!passable(cx, cy)) {
					can_see = false;
					break;
				}

				error += delta_error;
				if(error > 0.5) {
					cy += iy;
					error -= 1.0;
				}
			}
		} else {
			double delta_error = std::abs(double(deltax) / double(deltay));
			int cx = player.x;
			for(int cy = player.y; cy != y; cy += iy) {
				if(!passable(cx, cy)) {
					can_see = false;
					break;
				}

				error += delta_error;
				if(error > 0.5) {
					cx += ix;
					error -= 1.0;
				}
			}
		}
	}

	if(!can_see) {
		return CANNOT_SEE;
	}
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->sprite;
		}
	}
	return map.cell(x, y).sprite();
}

const std::string & Game::name(int x, int y) const
{
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->name;
		}
	}
	return map.cell(x, y).name();
}

bool Game::passable(int x, int y) const
{
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->opened;
		}
	}
	return map.cell(x, y).passable();
}

void Game::open_at(int x, int y)
{
	for(std::vector<Door>::iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			if(!door->opened) {
				door->open();
			}
		}
	}
}

void Game::close_at(int x, int y)
{
	for(std::vector<Door>::iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			if(door->opened) {
				door->close();
			}
		}
	}
}

