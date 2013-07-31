#include "util.h"
#include "game.h"
#include "log.h"

struct PathStep {
    int x, y, counter;
    bool fixed;
    PathStep(int _x, int _y, int _counter) : x(_x), y(_y), counter(_counter), fixed(false) {}
};

std::list<Control> find_path(int player_x, int player_y, int target_x, int target_y, PassabilityDetector * detector)
{
    std::list<Control> best_path;

    if(!detector->is_passable(target_x, target_y)) {
        return best_path;
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
        PathStep last_step = PathStep(player_x, player_y, path_counter);
        while(last_step.counter > 0) {
            for(std::vector<PathStep>::iterator s = path.begin(); s != path.end(); ++s) {
                if(s->counter < last_step.counter) {
                    int shift_x = s->x - last_step.x;
                    int shift_y = s->y - last_step.y;
                    int distance = std::abs(shift_x) + std::abs(shift_y);
                    bool is_close_diagonal = std::abs(shift_x) == std::abs(shift_y) && std::abs(shift_x) == 1;
                    bool is_close = (distance == 1) || is_close_diagonal;
                    if(is_close) {
                        last_step = *s;
                        Control control;
                        if(shift_y == -1) {
                            if(shift_x == -1) {
                                control = Control::UP_LEFT;
                            } else if(shift_x == 0) {
                                control = Control::UP;
                            } else if(shift_x == 1) {
                                control = Control::UP_RIGHT;
                            } else {
                                control = Control::UNKNOWN;
                            }
                        } else if(shift_y == 0) {
                            if(shift_x == -1) {
                                control = Control::LEFT;
                            } else if(shift_x == 1) {
                                control = Control::RIGHT;
                            } else {
                                control = Control::UNKNOWN;
                            }
                        } else if(shift_y == 1) {
                            if(shift_x == -1) {
                                control = Control::DOWN_LEFT;
                            } else if(shift_x == 0) {
                                control = Control::DOWN;
                            } else if(shift_x == 1) {
                                control = Control::DOWN_RIGHT;
                            } else {
                                control = Control::UNKNOWN;
                            }
                        } else {
                            control = Control::UNKNOWN;
                        }
                        log("path: {0}, {1} -> {2}").arg(shift_x).arg(shift_y).arg(char(control.value));
                        best_path.push_back(control);
                        break;
                    }
                }

            }
        }
    }

    return best_path;
}
