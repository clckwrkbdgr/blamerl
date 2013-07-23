#include "game.h"

Game::Game(int w, int h)
	: width(w), height(h), map(width * height, 0),
	player_x(width / 2), player_y(height / 2)
{
}

bool Game::valid(int x, int y) const
{
	return (x >= 0 && x < width && y >= 0 && y < height);
}

int & Game::get(int x, int y)
{
	static int empty_cell = int();
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
}

const int & Game::get(int x, int y) const
{
	static int empty_cell = int();
	if(!valid(x, y)) {
		return empty_cell;
	}
	return map[x + y * width];
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
	if(player_x >= width) {
		player_x = width - 1;
	}
	if(player_y < 0) {
		player_y = 0;
	}
	if(player_y >= height) {
		player_y = height - 1;
	}
	return true;
}

