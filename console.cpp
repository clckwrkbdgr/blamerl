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

    if(game.examining) {
        curs_set(1);
        move(game.cursor_y, game.cursor_x);
    } else {
        curs_set(0);
    }

	refresh();
}

Control Console::get_control()
{
	switch(getch()) {
        case 'h': return Control::LEFT;
        case 'j': return Control::DOWN;
        case 'k': return Control::UP;
        case 'l': return Control::RIGHT;
        case 'b': return Control::DOWN_LEFT;
        case 'n': return Control::DOWN_RIGHT;
        case 'y': return Control::UP_LEFT;
        case 'u': return Control::UP_RIGHT;

        case 'H': return Control::RUN_LEFT;
        case 'J': return Control::RUN_DOWN;
        case 'K': return Control::RUN_UP;
        case 'L': return Control::RUN_RIGHT;
        case 'B': return Control::RUN_DOWN_LEFT;
        case 'N': return Control::RUN_DOWN_RIGHT;
        case 'Y': return Control::RUN_UP_LEFT;
        case 'U': return Control::RUN_UP_RIGHT;

        case 'x': return Control::EXAMINE;
        case 'q': return Control::QUIT;
        default: return Control::UNKNOWN;
    }
}

