#include "console.h"
#include "game.h"
#include <cstdlib>

int main(int argc, char ** argv)
{
    srand(time(NULL));
	Console console;
	Game game(80, 25);

	while(true) {
		console.draw_game(game);

		int ch = console.get_control();
		if(!game.process_control(ch)) {
			break;
		}
	}

	return 0;
}

