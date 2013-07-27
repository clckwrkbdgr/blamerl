#include "console.h"
#include "game.h"
#include "log.h"
#include "savefile.h"
#include <cstdlib>
#include <cstdio>

int main(int argc, char ** argv)
{
    srand(time(NULL));
	Console console;
	Game game;
	if(load(game, "blamerl.save")) {
		remove("blamerl.save");
	} else {
		game.generate();
	}
    log("Game started.");

	while(true) {
		console.draw_game(game);

		Control control = console.get_control();
		if(!game.process_control(control)) {
			break;
		}
	}

	if(game.do_save) {
		save(game, "blamerl.save");
	}
	log("Exiting...");

	return 0;
}

