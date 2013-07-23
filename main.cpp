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

		Control control = console.get_control();
		if(!game.process_control(control)) {
			break;
		}
	}

	return 0;
}

