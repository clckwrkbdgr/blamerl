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

    try {
        while(true) {
            console.draw_game(game);

            Control control = game.has_auto_control() ? game.auto_control : console.get_control();
            if(!game.process_control(control)) {
                break;
            }
        }

        if(game.do_save) {
            save(game, "blamerl.save");
        }
    } catch(const Map::OutOfMapException & e) {
        log("Out of map exception: {0}, {1} is out of (0, 0, {2}, {3}) map rect.").
            arg(e.x).arg(e.y).arg(game.width()).arg(game.height());
    }

	log("Exiting...");

	return 0;
}

