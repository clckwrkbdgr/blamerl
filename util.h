#pragma once
#include <vector>
#include <cstdlib>
#include <list>
class Control;

class PassabilityDetector {
public:
	virtual ~PassabilityDetector() {}
	virtual bool is_passable(int x, int y) const = 0;
};

std::list<Control> find_path(int _player_x, int _player_y, int _target_x, int _target_y, PassabilityDetector * detector);

