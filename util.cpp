#include "util.h"

Pathfinder::Pathfinder(int _player_x, int _player_y, int _target_x, int _target_y)
	: player_x(_player_x), player_y(_player_y), target_x(_target_x), target_y(_target_y), step(best_path.end())
{
}

bool Pathfinder::next()
{
	if(step != best_path.end()) {
		++step;
		return true;
	}
	return (step != best_path.end());
}

int Pathfinder::shift_x() const
{
	if(step == best_path.end()) {
		return 0;
	}
	return step->x - (step - 1)->x;
}

int Pathfinder::shift_y() const
{
	if(step == best_path.end()) {
		return 0;
	}
	return step->y - (step - 1)->y;
}
