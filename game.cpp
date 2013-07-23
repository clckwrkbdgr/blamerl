#include "game.h"

Control::Control(int v, bool running)
    : value(v), run(running)
{
    //run = (v == RUN_LEFT || v == RUN_RIGHT || v == RUN_UP || v == RUN_DOWN ||
            //v == RUN_DOWN_LEFT || v == RUN_DOWN_RIGHT || v == RUN_UP_LEFT || v == RUN_UP_RIGHT);
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

bool Game::move_to(int target_x, int target_y)
{
    if(!map.valid(target_x, target_y)) {
        return false;
    }
    if(!map.cell(target_x, target_y).passable) {
        return false;
    }
    player_x = target_x;
    player_y = target_y;
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
    int shift_x = 0;
    int shift_y = 0;
	switch(control.value) {
        case Control::EXAMINE:
            examining = !examining;
            if(examining) {
                cursor_x = player_x;
                cursor_y = player_y;
            }
            break;
        case Control::LEFT: shift_x = -1; shift_y = 0; break;
        case Control::DOWN: shift_x = 0; shift_y = 1; break;
        case Control::UP: shift_x = 0; shift_y = -1; break;
        case Control::RIGHT: shift_x = 1; shift_y = 0; break;
        case Control::DOWN_LEFT: shift_x = -1; shift_y = 1; break;
        case Control::DOWN_RIGHT: shift_x = 1; shift_y = 1; break;
        case Control::UP_LEFT: shift_x = -1; shift_y = -1; break;
        case Control::UP_RIGHT: shift_x = 1; shift_y = -1; break;
        case Control::TARGET:
            if(examining) {
                bool ok = move_to(cursor_x, cursor_y);
                if(ok) {
                    examining = false;
                }
            }
            break;
        case Control::QUIT: return false;
		default: break;
	}

    if(shift_x != 0 || shift_y != 0) {
        if(examining) {
            move_cursor_by(shift_x, shift_y);
        } else if(control.run) {
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

