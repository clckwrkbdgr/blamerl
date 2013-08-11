#include "game.h"
#include "log.h"
#include "util.h"
#include <cmath>
#include <cstdlib>

enum { MAP_WIDTH = 60, MAP_HEIGHT = 23 };

Control::Control(int v, bool running)
    : value(v), run(running)
{
}

//------------------------------------------------------------------------------

Player::Player(int player_x, int player_y)
	: x(player_x), y(player_y), sight(5)
{
}

//------------------------------------------------------------------------------

Game::Game()
	: state(MOVING), do_save(true), examining(false), world_x(0), world_y(0)
{
}

void Game::generate()
{
    log("Generating new map...");

	map.generate(MAP_WIDTH, MAP_HEIGHT);

    world_x = 0;
    world_y = 0;

	player.x = map.width / 2;
	player.y = map.height / 2;
	invalidate_fov();
    log("FOV invalidated.");
}

bool Game::move_by(int shift_x, int shift_y)
{
    if(!map.valid(player.x + shift_x, player.y + shift_y)) {
        int old_player_x = player.x + shift_x;
        int old_player_y = player.y + shift_y;

        map.generate(MAP_WIDTH, MAP_HEIGHT);

        while(old_player_x >= map.width)  { old_player_x = old_player_x - map.width;  ++world_x; }
        while(old_player_x < 0)           { old_player_x = old_player_x + map.width;  --world_x; }
        while(old_player_y >= map.height) { old_player_y = old_player_y - map.height; ++world_y; }
        while(old_player_y < 0)           { old_player_y = old_player_y + map.height; --world_y; }
        player.x = old_player_x;
        player.y = old_player_y;
        log("Player entered the scene at {0}, {1}.").arg(player.x).arg(player.y);
        invalidate_fov();
        log("FOV invalidated.");

        state = MOVING;
        message = "You have moved to a new place.";
        auto_control = Control::UNKNOWN;
        auto_control_list.clear();
        return true;
    }
    if(!passable(player.x + shift_x, player.y + shift_y)) {
        return false;
    }
    player.x += shift_x;
    player.y += shift_y;
	invalidate_fov();
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
    auto_control_list = find_path(player.x, player.y, target_x, target_y, &detector);
    if(!auto_control_list.empty()) {
        state = RUNNING;
    }
    log("State: {0} == {1}").arg(state).arg(RUNNING);
    auto_control = Control::UNKNOWN;

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

bool Game::has_auto_control() const
{
    return !(auto_control_list.empty() && auto_control.value == Control::UNKNOWN);
}

Control Game::get_auto_control() const
{
    if(!auto_control_list.empty()) {
        return auto_control_list.front();
    }
    return auto_control;
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
            auto_control = control.value;
            state = RUNNING;
		} else {
			move_by(shift_x, shift_y);
		}
	}
	return true;
}

bool Game::process_running(const Control & control)
{
	message = "";
	int shift_x = 0;
	int shift_y = 0;
	switch(control.value) {
		case Control::LEFT: shift_x = -1; shift_y = 0; break;
		case Control::DOWN: shift_x = 0; shift_y = 1; break;
		case Control::UP: shift_x = 0; shift_y = -1; break;
		case Control::RIGHT: shift_x = 1; shift_y = 0; break;
		case Control::DOWN_LEFT: shift_x = -1; shift_y = 1; break;
		case Control::DOWN_RIGHT: shift_x = 1; shift_y = 1; break;
		case Control::UP_LEFT: shift_x = -1; shift_y = -1; break;
		case Control::UP_RIGHT: shift_x = 1; shift_y = -1; break;
		default:
            state = MOVING;
            auto_control = Control::UNKNOWN;
            return true;
	}

	if(shift_x != 0 || shift_y != 0) {
        if(move_by(shift_x, shift_y)) {
            if(!auto_control_list.empty()) {
                auto_control_list.pop_front();
                if(!has_auto_control()) {
                    state = MOVING;
                    auto_control = Control::UNKNOWN;
                }
                return true;
            } else if(!has_auto_control()) {
                state = MOVING;
                auto_control = Control::UNKNOWN;
            }
        } else {
            state = MOVING;
            auto_control = Control::UNKNOWN;
        }
    } else {
        state = MOVING;
        auto_control = Control::UNKNOWN;
	}

	return true;
}

bool Game::process_suicide(const Control & control)
{
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
		invalidate_fov();
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
		invalidate_fov();
	}
	state = MOVING;
	return true;
}

bool Game::process_control(const Control & control)
{
	switch(state) {
		case MOVING: return process_moving(control);
		case RUNNING: return process_running(control);
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

void Game::invalidate_fov()
{
	for(int x = player.x - player.sight; x <= player.x + player.sight; ++x) {
		for(int y = player.y - player.sight; y <= player.y + player.sight; ++y) {
            if(!map.valid(x, y)) {
                continue;
            }
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
						if(!transparent(cx, cy)) {
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
						if(!transparent(cx, cy)) {
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
			map.cell(x, y).visible = can_see;
			if(can_see) {
				map.cell(x, y).seen = true;
			}
		}
	}
}

const Sprite & Game::sprite(int x, int y) const
{
    return map.sprite(x, y);
}

const std::string & Game::name(int x, int y) const
{
    return map.name(x, y);
}

bool Game::transparent(int x, int y) const
{
    return map.transparent(x, y);
}

bool Game::passable(int x, int y) const
{
    return map.passable(x, y);
}

void Game::open_at(int x, int y)
{
    map.open_at(x, y);
}

void Game::close_at(int x, int y)
{
    map.close_at(x, y);
}

