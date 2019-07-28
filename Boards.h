#pragma once
#include <vector>
#include <array>
#include <utility>
class Boards {
public:
	Boards(int x, int y, std::vector<int> mines);
	~Boards();
	bool isPutable(int x, int y);
	bool Put(int x, int y);
	bool isFull();
	bool isMine(int x, int y);
	std::vector<std::vector<int>> delta;
	std::vector<std::vector<int>> board;
	int boardx, boardy;
private:
	bool inArea(int x, int y);
	void addMineNum(int x, int y);
	void Reverse(int x, int y);
};

