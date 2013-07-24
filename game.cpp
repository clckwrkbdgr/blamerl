#include "game.h"
#include "log.h"
#include <cmath>
#include <cstdlib>

Control::Control(int v, bool running)
    : value(v), run(running)
{
}

// --------------------------------------------------------------------------

Game::Game(int w, int h)
	: map(w, h), examining(false),
	player_x(map.width / 2), player_y(map.height / 2)
{
	map.generate();
}

bool Game::move_by(int shift_x, int shift_y)
{
    if(!map.valid(player_x + shift_x, player_y + shift_y)) {
        return false;
    }
    if(!map.passable(player_x + shift_x, player_y + shift_y)) {
        return false;
    }
    player_x += shift_x;
    player_y += shift_y;
    //map.cell(player_x, player_y).sprite = '*';
    return true;
}

struct PathStep {
    int x, y, counter;
    bool fixed;
    PathStep(int _x, int _y, int _counter) : x(_x), y(_y), counter(_counter), fixed(false) {}
};

bool Game::move_to(int target_x, int target_y)
{
    if(!map.valid(target_x, target_y)) {
        return false;
    }
    if(!map.passable(target_x, target_y)) {
        return false;
    }

    std::vector<PathStep> path;
    path.push_back(PathStep(target_x, target_y, 0));
    int path_counter = 0;
    for(int i = 1; i < 2000; ++i) {
        log("Iteration {0}").arg(i);
        std::vector<PathStep> next_steps;
        for(std::vector<PathStep>::iterator step = path.begin(); step != path.end(); ++step) {
            if(step->fixed) {
                continue;
            }
            log("Processing cell {x}, {y}").arg("x", step->x).arg("y", step->y);
            if(step->x == player_x && step->y == player_y) {
                log("\tFound step is player: {0}, {1}").arg(step->x).arg(step->y);
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
                if(!map.valid(next_step->x, next_step->y)) {
                    continue;
                }
                if(!map.passable(next_step->x, next_step->y)) {
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
                log("Found step: {x}, {y}").arg("x", next_step->x).arg("y", next_step->y);
                next_steps.push_back(*next_step);
            }

            log("End processing cell {x}, {y}").arg("x", step->x).arg("y", step->y);
            step->fixed = true;
        }
        path.insert(path.end(), next_steps.begin(), next_steps.end());
        if(path_counter > 0) {
            break;
        }
    }
    log("Path counter: {0}").arg(path_counter);
    if(path_counter > 0) {
        for(int i = 0; i < path.size(); ++i) {
            log("Found path {0}: ({1}, {2}: {3})").arg(i).arg(path[i].x).arg(path[i].y).arg(path[i].counter);
        }

        std::vector<PathStep> best_path;
        PathStep last_step = PathStep(player_x, player_y, path_counter);
        best_path.push_back(last_step);
        while(last_step.counter > 0) {
            log("Last step: {0}, {1}: {2}").arg(last_step.x).arg(last_step.y).arg(last_step.counter);
            log("Current counter: {0}").arg(last_step.counter);
            for(std::vector<PathStep>::iterator s = path.begin(); s != path.end(); ++s) {
                if(s->counter < last_step.counter) {
                    int distance = std::abs(s->x - last_step.x) + std::abs(s->y - last_step.y);
                    bool is_close_diagonal = std::abs(s->x - last_step.x) == std::abs(s->y - last_step.y) && std::abs(s->x - last_step.x) == 1;
                    bool is_close = (distance == 1) || is_close_diagonal;
                    log("\tStep: {0}, {1}: {2} || {3}=={4}").arg(s->x).arg(s->y).arg(distance)
                        .arg(std::abs(s->x - last_step.x))
                        .arg(std::abs(s->y - last_step.y));
                    if(is_close) {
                        last_step = *s;
                        log("\tFound best step: {x}, {y}").arg("x", s->x).arg("y", s->y);
                        best_path.push_back(last_step);
                        break;
                    }
                }

            }
        }
        for(int i = 0; i < best_path.size(); ++i) {
            log("Found best path {0}: ({1}, {2}, {3})").arg(i).arg(best_path[i].x).arg(best_path[i].y).arg(best_path[i].counter);
        }
        PathStep last(player_x, player_y, -1);
        for(std::vector<PathStep>::iterator step = best_path.begin(); step != best_path.end(); ++step) {
            if(last.counter < 0) {
                last = *step;
                continue;
            }
            move_by(step->x - last.x, step->y - last.y);
            last = *step;
        }
    }

    return true;
}

bool Game::move_cursor_by(int shift_x, int shift_y)
{
    if(!map.valid(cursor_x + shift_x, cursor_y + shift_y)) {
        return false;
    }
    cursor_x += shift_x;
    cursor_y += shift_y;
    return true;
}

bool Game::process_control(const Control & control)
{
    int shift_x = 0;
    int shift_y = 0;
	switch(control.value) {
        case Control::EXAMINE:
            examining = !examining;
            if(examining) {
                cursor_x = player_x;
                cursor_y = player_y;
            }
            break;
        case Control::LEFT: shift_x = -1; shift_y = 0; break;
        case Control::DOWN: shift_x = 0; shift_y = 1; break;
        case Control::UP: shift_x = 0; shift_y = -1; break;
        case Control::RIGHT: shift_x = 1; shift_y = 0; break;
        case Control::DOWN_LEFT: shift_x = -1; shift_y = 1; break;
        case Control::DOWN_RIGHT: shift_x = 1; shift_y = 1; break;
        case Control::UP_LEFT: shift_x = -1; shift_y = -1; break;
        case Control::UP_RIGHT: shift_x = 1; shift_y = -1; break;
        case Control::TARGET:
            if(examining) {
                bool ok = move_to(cursor_x, cursor_y);
                if(ok) {
                    examining = false;
                }
            }
            break;
        case Control::QUIT: return false;
		default: break;
	}

    if(shift_x != 0 || shift_y != 0) {
        if(examining) {
            move_cursor_by(shift_x, shift_y);
        } else if(control.run) {
            bool running = true;
            while(running) {
                running = move_by(shift_x, shift_y);
            }
        } else {
            move_by(shift_x, shift_y);
        }
    }
	return true;
}

int Game::width() const
{
	return map.width;
}

int Game::height() const
{
	return map.height;
}

const Sprite & Game::sprite(int x, int y) const
{
	return map.sprite(x, y);
}


