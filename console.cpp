#include "console.h"
#include <ncurses.h>
#include "game.h"

Console::Console()
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);

	getmaxyx(stdscr, height, width);
}

Console::~Console()
{
	cbreak();
	echo();
	curs_set(1);
	endwin();
}

void Console::draw_game(const Game & game)
{
	int width, height;
	getmaxyx(stdscr, height, width);
	for(int x = 0; x < game.width; ++x) {
		for(int y = 0; y < game.height; ++y) {
			int sprite = ' ';
			switch(game.get(x, y)) {
				case 0: sprite = '.'; break;
				default: sprite = ' '; break;
			}
			mvaddch(y, x, sprite);
		}
	}

	mvaddch(game.player_y, game.player_x, '@');
	refresh();
}

int Console::get_control()
{
	return getch();
}

