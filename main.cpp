#include <ncurses.h>

class Game {
public:
	int width, height;
	int x, y;
	Game();
	bool process_control(int ch);
};


class Ncurses {
public:
	Ncurses();
	~Ncurses();

	void draw_game(const Game & game);
	int get_control();
};

Ncurses::Ncurses()
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
}

Ncurses::~Ncurses()
{
	cbreak();
	echo();
	curs_set(1);
	endwin();
}


Game::Game()
{
	getmaxyx(stdscr, height, width);
	x = width / 2;
	y = height / 2;
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

void Ncurses::draw_game(const Game & game)
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

int Ncurses::get_control()
{
	return getch();
}

int main(int argc, char ** argv)
{
	Ncurses ncurses;
	Game game;


	while(true) {
		ncurses.draw_game(game);

		int ch = ncurses.get_control();
		if(!game.process_control(ch)) {
			break;
		}
	}

	return 0;
}
