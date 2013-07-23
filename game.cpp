#include "game.h"

Game::Game(int w, int h)
	: map(w, h),
	player_x(map.width / 2), player_y(map.height / 2)
{
    for(int i = 0; i < 10; ++i) {
        int x = rand() % map.width;
        int y = rand() % map.height;
        map.get(x, y) = Cell('#', false);
    }
}

bool Game::process_control(int ch)
{
	switch(ch) {
		case 'h': player_x--; break;
		case 'j': player_y++; break;
		case 'k': player_y--; break;
		case 'l': player_x++; break;
		case 'q': return false;
		default: break;
	}
	if(player_x < 0) {
		player_x = 0;
	}
	if(player_x >= map.width) {
		player_x = map.width - 1;
	}
	if(player_y < 0) {
		player_y = 0;
	}
	if(player_y >= map.height) {
		player_y = map.height - 1;
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
	return map.get(x, y).sprite;
}

const Sprite & Game::sprite(int x, int y) const
{
	return map.get(x, y).sprite;
}

