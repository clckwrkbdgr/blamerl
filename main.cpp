#include <ncurses.h>

int main(int argc, char ** argv)
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);

	int width, height;
	getmaxyx(stdscr, height, width);

	int x = width / 2;
	int y = height / 2;

	mvaddch(y, x, '@');

	int ch;
	while((ch = getch()) != 'q') {
		int oldX = x, oldY = y;

		switch(ch) {
			case 'h': x--; break;
			case 'j': y++; break;
			case 'k': y--; break;
			case 'l': x++; break;
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

		mvaddch(oldY, oldX, '.');
		mvaddch(y, x, '@');
		refresh();
	}

	cbreak();
	echo();
	curs_set(1);
	endwin();
	return 0;
}
