#pragma once
#include <vector>
#include <cstdlib>

class PassabilityDetector {
public:
	virtual ~PassabilityDetector() {}
	virtual bool is_passable(int x, int y) const = 0;
};

class Pathfinder {
	struct PathStep {
		int x, y, counter;
		bool fixed;
		PathStep(int _x, int _y, int _counter) : x(_x), y(_y), counter(_counter), fixed(false) {}
	};

	int player_x, player_y, target_x, target_y;
	std::vector<PathStep> best_path;
	std::vector<PathStep>::const_iterator step;

public:
	Pathfinder(int _player_x, int _player_y, int _target_x, int _target_y);

	bool next();
	int shift_x() const;
	int shift_y() const;

	bool find_path(PassabilityDetector * detector)
	{
		if(!detector->is_passable(target_x, target_y)) {
			return false;
		}

		std::vector<PathStep> path;
		path.push_back(PathStep(target_x, target_y, 0));
		int path_counter = 0;
		for(int i = 1; i < 2000; ++i) {
			std::vector<PathStep> next_steps;
			for(std::vector<PathStep>::iterator step = path.begin(); step != path.end(); ++step) {
				if(step->fixed) {
					continue;
				}
				if(step->x == player_x && step->y == player_y) {
					path_counter = i + 1;
					break;
				}
				std::vector<PathStep> neighs;
				neighs.push_back(PathStep(step->x + 1, step->y + 1, i));
				neighs.push_back(PathStep(step->x    , step->y + 1, i));
				neighs.push_back(PathStep(step->x - 1, step->y + 1, i));
				neighs.push_back(PathStep(step->x + 1, step->y    , i));
				neighs.push_back(PathStep(step->x - 1, step->y    , i));
				neighs.push_back(PathStep(step->x + 1, step->y - 1, i));
				neighs.push_back(PathStep(step->x    , step->y - 1, i));
				neighs.push_back(PathStep(step->x - 1, step->y - 1, i));

				for(std::vector<PathStep>::iterator next_step = neighs.begin(); next_step != neighs.end(); ++next_step) {
					if(!detector->is_passable(next_step->x, next_step->y)) {
						continue;
					}

					bool exists = false;
					for(std::vector<PathStep>::iterator s = path.begin(); s != path.end(); ++s) {
						if(s->x == next_step->x && s->y == next_step->y && s->counter <= next_step->counter) {
							exists = true;
							break;
						}
					}
					if(!exists) {
						for(std::vector<PathStep>::iterator s = next_steps.begin(); s != next_steps.end(); ++s) {
							if(s->x == next_step->x && s->y == next_step->y && s->counter <= next_step->counter) {
								exists = true;
								break;
							}
						}
					}
					if(exists) {
						continue;
					}
					next_steps.push_back(*next_step);
				}

				step->fixed = true;
			}
			path.insert(path.end(), next_steps.begin(), next_steps.end());
			if(path_counter > 0) {
				break;
			}
		}
		if(path_counter > 0) {
			for(size_t i = 0; i < path.size(); ++i) {
			}

			PathStep last_step = PathStep(player_x, player_y, path_counter);
			best_path.push_back(last_step);
			while(last_step.counter > 0) {
				for(std::vector<PathStep>::iterator s = path.begin(); s != path.end(); ++s) {
					if(s->counter < last_step.counter) {
						int distance = std::abs(s->x - last_step.x) + std::abs(s->y - last_step.y);
						bool is_close_diagonal = std::abs(s->x - last_step.x) == std::abs(s->y - last_step.y) && std::abs(s->x - last_step.x) == 1;
						bool is_close = (distance == 1) || is_close_diagonal;
						if(is_close) {
							last_step = *s;
							best_path.push_back(last_step);
							break;
						}
					}

				}
			}
		}

		step = best_path.begin();
		return !best_path.empty();
	}
};


