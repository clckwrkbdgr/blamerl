#include "savefile.h"
#include "game.h"
#include "log.h"
#include <fstream>

bool save(const Game & game, const std::string & filename)
{
	log("Saving...");
	std::ofstream out(filename.c_str());
	if(!out) {
		log("Cannot open {0} for writing!").arg(filename);
		return false;
	}
	out << game.player_x << ' ' << game.player_y << '\n';

	out << Cell::types().size() << '\n';
	for(std::vector<CellType>::const_iterator cell_type = Cell::types().begin(); cell_type != Cell::types().end(); ++cell_type) {
		out << int(cell_type->sprite) << ' ' << (cell_type->passable ? 1 : 0) << ' ' << cell_type->name << '\n';
	}

	out << game.map.width << ' ' << game.map.height << '\n';
	for(int y = 0; y < game.map.height; ++y) {
		for(int x = 0; x < game.map.width; ++x) {
			out << game.map.map[x + y * game.map.width].type << ' ';
		}
		out << '\n';
	}

	out << game.map.doors.size() << '\n';
	for(std::vector<Door>::const_iterator door = game.map.doors.begin(); door != game.map.doors.end(); ++door) {
		out << door->x << ' ' << door->y << ' ' << (door->opened ? 1 : 0) << ' ' << int(door->sprite) << ' ' << door->name << '\n';
	}

	log("Game is successfully saved.");
	return true;
}
