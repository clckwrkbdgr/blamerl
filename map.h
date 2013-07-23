#pragma once
#include <vector>

class Map {
public:
	int width, height;
	std::vector<int> map;

	Map(int w, int h);

	bool valid(int x, int y) const;
	int & get(int x, int y);
	const int & get(int x, int y) const;

};

