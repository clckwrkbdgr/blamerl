#include "console.h"
#include <ncurses.h>
#include "game.h"
#include "log.h"

enum { HUD_WIDTH = 20, MESSAGE_HEIGHT = 1 };

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
    int message_pan_y = height - MESSAGE_HEIGHT;
    int hud_x = width - HUD_WIDTH;

	for(int x = 0; x < game.width(); ++x) {
		for(int y = 0; y < game.height(); ++y) {
			Sprite sprite = game.sprite(x, y);
			mvaddch(y, x, sprite);
		}
	}

	mvaddch(game.player.y, game.player.x, '@');

	mvprintw(message_pan_y, 0, "%s", std::string(width, ' ').c_str());
	mvprintw(message_pan_y, 0, "%s", game.message.c_str());

    mvprintw(0, hud_x, "%s", format("World: ({0}, {1})").arg(game.world_x).arg(game.world_y).str().substr(0, HUD_WIDTH).c_str());

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
        case 'Q': return Control::SUICIDE;
        default: return Control::UNKNOWN;
    }
	return Control::UNKNOWN;
}

