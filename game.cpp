#include "game.h"

Control::Control(int v)
    : value(v), run(false)
{
    run = (v == RUN_LEFT || v == RUN_RIGHT || v == RUN_UP || v == RUN_DOWN);
}

// --------------------------------------------------------------------------

Game::Game(int w, int h)
	: map(w, h), examining(false),
	player_x(map.width / 2), player_y(map.height / 2)
{
    for(int i = 0; i < 10; ++i) {
        int x = rand() % map.width;
        int y = rand() % map.height;
        map.cell(x, y) = Cell('#', false);
    }
}

bool Game::move_by(int shift_x, int shift_y)
{
    if(!map.valid(player_x + shift_x, player_y + shift_y)) {
        return false;
    }
    if(!map.cell(player_x + shift_x, player_y + shift_y).passable) {
        return false;
    }
    player_x += shift_x;
    player_y += shift_y;
    map.cell(player_x, player_y).sprite = '*';
    return true;
}

bool Game::move_cursor_by(int shift_x, int shift_y)
{
    if(!map.valid(cursor_x + shift_x, cursor_y + shift_y)) {
        return false;
    }
    cursor_x += shift_x;
    cursor_y += shift_y;
    return true;
}

bool Game::process_control(const Control & control)
{
    if(control.run) {
        bool running = true;
        while(running) {
            switch(control.value) {
                case Control::RUN_LEFT: running = move_by(-1, 0); break;
                case Control::RUN_DOWN: running = move_by(0, 1); break;
                case Control::RUN_UP: running = move_by(0, -1); break;
                case Control::RUN_RIGHT: running = move_by(1, 0); break;
                default: running = false;
            }
        }
        return true;
    }
	switch(control.value) {
        case Control::EXAMINE: examining = !examining; if(examining) { cursor_x = player_x; cursor_y = player_y; } break;
        case Control::LEFT: examining ? move_cursor_by(-1, 0) : move_by(-1, 0); break;
        case Control::DOWN: examining ? move_cursor_by(0, 1) : move_by(0, 1); break;
        case Control::UP: examining ? move_cursor_by(0, -1) : move_by(0, -1); break;
        case Control::RIGHT: examining ? move_cursor_by(1, 0) : move_by(1, 0); break;
        case Control::QUIT: return false;
		default: break;
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

Sprite & Game::sprite(int x, int y)
{
	return map.cell(x, y).sprite;
}

const Sprite & Game::sprite(int x, int y) const
{
	return map.cell(x, y).sprite;
}

