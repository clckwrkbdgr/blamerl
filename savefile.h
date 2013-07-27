#pragma once
#include <string>
class Game;

bool save(const Game & game, const std::string & filename);
bool load(Game & game, const std::string & filename);
