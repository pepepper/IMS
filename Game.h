#pragma once
#include "Boards.h"
class Game{
	public:
	Game();
	Game(int x, int y);
	Game(int x, int y, std::vector<int> vmines);
	~Game();
	bool open(int x, int y);
	void openall();
	Boards *board;
	bool full = false;
	int howturn,  turn;
	long long joined;
	std::vector<int> vmines;
};
