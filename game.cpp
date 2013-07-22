#include "game.h"

Game::Game(int w, int h)
	: width(w), height(h), x(width / 2), y(height / 2)
{
}

bool Game::process_control(int ch)
{
	switch(ch) {
		case 'h': x--; break;
		case 'j': y++; break;
		case 'k': y--; break;
		case 'l': x++; break;
		case 'q': return false;
		default: break;
	}
	if(x < 0) {
		x = 0;
	}
	if(x >= width) {
		x = width - 1;
	}
	if(y < 0) {
		y = 0;
	}
	if(y >= height) {
		y = height - 1;
	}
	return true;
}

