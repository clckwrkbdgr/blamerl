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
    int new_player_x = player_x;
    int new_player_y = player_y;

	switch(ch) {
		case 'h': new_player_x--; break;
		case 'j': new_player_y++; break;
		case 'k': new_player_y--; break;
		case 'l': new_player_x++; break;
		case 'q': return false;
		default: break;
	}
    if(map.valid(new_player_x, new_player_y) && map.get(new_player_x, new_player_y).passable) {
        player_x = new_player_x;
        player_y = new_player_y;
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

