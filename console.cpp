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
	for(int x = 0; x < game.width(); ++x) {
		for(int y = 0; y < game.height(); ++y) {
			Sprite sprite = game.sprite(x, y);
			mvaddch(y, x, sprite);
		}
	}

	mvaddch(game.player_y, game.player_x, '@');
	refresh();
}

Control Console::get_control()
{
	switch(getch()) {
        case 'h': return Control::LEFT;
        case 'j': return Control::DOWN;
        case 'k': return Control::UP;
        case 'l': return Control::RIGHT;
        case 'H': return Control::RUN_LEFT;
        case 'J': return Control::RUN_DOWN;
        case 'K': return Control::RUN_UP;
        case 'L': return Control::RUN_RIGHT;
        case 'q': return Control::QUIT;
        default: return Control::UNKNOWN;
    }
}

