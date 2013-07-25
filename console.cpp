#include "console.h"
#include <ncurses.h>
#include "game.h"
#include "log.h"

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

    if(game.show_cursor()) {
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
		case 27: {
			log("Read 27");
			timeout(0);
			int code = getch();
			notimeout(stdscr, TRUE);
			log("Read {0}").arg(code);
			if(code == ERR || code == 27) {
				log("It was an ERR and I got an ESC.");
				return Control::CANCEL; // Escape;
			} else {
				// Have an ALT+code here.
			}
			break;
		}
        case 'h': return Control::LEFT;
        case 'j': return Control::DOWN;
        case 'k': return Control::UP;
        case 'l': return Control::RIGHT;
        case 'b': return Control::DOWN_LEFT;
        case 'n': return Control::DOWN_RIGHT;
        case 'y': return Control::UP_LEFT;
        case 'u': return Control::UP_RIGHT;

        case 'H': return Control(Control::LEFT, true);
        case 'J': return Control(Control::DOWN, true);
        case 'K': return Control(Control::UP, true);
        case 'L': return Control(Control::RIGHT, true);
        case 'B': return Control(Control::DOWN_LEFT, true);
        case 'N': return Control(Control::DOWN_RIGHT, true);
        case 'Y': return Control(Control::UP_LEFT, true);
        case 'U': return Control(Control::UP_RIGHT, true);

        case 'o': return Control::OPEN;
        case 'c': return Control::CLOSE;

        case 'x': return Control::EXAMINE;
        case '.': return Control::TARGET;
        case 'q': return Control::QUIT;
        default: return Control::UNKNOWN;
    }
}

