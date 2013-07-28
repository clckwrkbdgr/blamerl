#include "savefile.h"
#include "game.h"
#include "log.h"
#include <fstream>

enum { VERSION = 3 };

bool save(const Game & game, const std::string & filename)
{
	log("Saving...");
	std::ofstream out(filename.c_str());
	if(!out) {
		log("Cannot open {0} for writing!").arg(filename);
		return false;
	}
	out << VERSION << '\n';

	out << game.player.x << ' ' << game.player.y << '\n';

	out << Cell::types().size() << '\n';
	for(std::vector<CellType>::const_iterator cell_type = Cell::types().begin(); cell_type != Cell::types().end(); ++cell_type) {
		out << int(cell_type->sprite) << ' ' << (cell_type->passable ? 1 : 0) << ' ' << (cell_type->transparent ? 1 : 0) << ' ' << cell_type->name << '\n';
	}

	out << game.map.width << ' ' << game.map.height << '\n';
	for(int y = 0; y < game.map.height; ++y) {
		for(int x = 0; x < game.map.width; ++x) {
			const Cell & cell = game.map.cell(x, y);
			out << cell.type << ' ' << (cell.seen ? 1 : 0) << ' ';
		}
		out << '\n';
	}

	out << game.doors.size() << '\n';
	for(std::vector<Door>::const_iterator door = game.doors.begin(); door != game.doors.end(); ++door) {
		out << door->x << ' ' << door->y << ' ' << (door->opened ? 1 : 0) << ' ' << int(door->sprite) << ' ' << door->name << '\n';
	}

	log("Game is successfully saved.");
	return true;
}

bool load(Game & game, const std::string & filename)
{
	log("Loading...");
	std::ifstream in(filename.c_str());
	if(!in) {
		log("Cannot open {0} for reading!").arg(filename);
		return false;
	}
	game = Game();
	int version;
	in >> version;
	if(version < 1) {
		log("Invalid save file version {0}").arg(version);
		return false;
	}

	in >> game.player.x >> game.player.y;

	Cell::types().clear();
	int cell_types_count = 0;
	in >> cell_types_count;
	while(cell_types_count--) {
		int sprite = 0, passable = 0, transparent = 0;
		if(version <= 2) {
			in >> sprite >> passable;
		} else {
			in >> sprite >> passable >> transparent;
		}
		std::string name;
		char c;
		in >> std::noskipws >> c;
		getline(in, name, '\n');
		in >> std::skipws;
		Cell::types().push_back(CellType(sprite, passable == 1, transparent == 1, name));
		log("Cell type: sprite '{0}' pass:{1} transp:{2}, name='{3}'").arg(sprite).arg(passable).arg(transparent).arg(name);
	}

	in >> game.map.width >> game.map.height;
	game.map.map.clear();
	for(int y = 0; y < game.map.height; ++y) {
		for(int x = 0; x < game.map.width; ++x) {
			if(version <= 1) {
				int type;
				in >> type;
				game.map.map.push_back(Cell(type));
			} else {
				int type, seen;
				in >> type >> seen;
				Cell cell = Cell(type);
				cell.seen = seen;
				game.map.map.push_back(cell);
			}
		}
	}

	game.doors.clear();
	int door_count = 0;
	in >> door_count;
	while(door_count--) {
		int x, y, opened, sprite;
		std::string name;
		in >> x >> y >> opened >> sprite;
		char c;
		in >> std::noskipws >> c;
		getline(in, name, '\n');
		in >> std::skipws;

		Door door(x, y);
		door.opened = opened == 1;
		door.sprite = sprite;
		door.name = name;
		game.doors.push_back(door);
	}

	game.invalidate_fov();
	log("Game is successfully loaded.");
	return true;
}
