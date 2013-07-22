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
	move(0, 0);
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			addch(' ');
		}
		addch('\n');
	}

	mvaddch(game.y, game.x, '@');
	refresh();
}

int Console::get_control()
{
	return getch();
}

