#include "console.h"
#include "game.h"

int main(int argc, char ** argv)
{
	Console console;
	Game game(console.width, console.height);

	while(true) {
		console.draw_game(game);

		int ch = console.get_control();
		if(!game.process_control(ch)) {
			break;
		}
	}

	return 0;
}

