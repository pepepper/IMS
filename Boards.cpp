#include "Boards.h"

bool Boards::inArea(int x, int y) {
	if (x > -1 && x<boardx&&y>-1 && y < boardy)return true;
	else return false;
}

void Boards::addMineNum(int x, int y) {
	if (inArea(x, y) && board[x][y] != 9)board[x][y]++;
}

Boards::Boards(int x, int y, std::vector<int> mines) {
	boardx = x;
	boardy = y;
	board.resize(x);
	for (std::vector<int> &v : board) {
		v.resize(y, 0);
	}
	for (int i : mines) {
		int minex = i >> 16, miney = i & 0xffff;
		board[minex][miney] = 9;
		addMineNum(minex - 1, miney - 1);
		addMineNum(minex - 1, miney);
		addMineNum(minex - 1, miney + 1);
		addMineNum(minex, miney - 1);
		addMineNum(minex, miney + 1);
		addMineNum(minex + 1, miney - 1);
		addMineNum(minex + 1, miney);
		addMineNum(minex + 1, miney + 1);

	}
}

void Boards::Reverse(int x, int y) {
	int i = 0;
	delta.emplace_back(std::vector<int> {board[x][y], x, y });
	board[x][y] = -1;
}

bool Boards::isPutable(int x, int y) {
	if (inArea(x, y) && board[x][y] != -1)return true;
	return false;
}

bool Boards::Put(int x, int y) {
	bool ret = false;
	if (isPutable(x, y)) {
		int temp = board[x][y];
		Reverse(x, y);
		ret = true;
		if (temp== 0) {
			Put(x - 1, y - 1);
			Put(x - 1, y);
			Put(x - 1, y + 1);
			Put(x, y - 1);
			Put(x, y + 1);
			Put(x + 1, y - 1);
			Put(x + 1, y);
			Put(x + 1, y + 1);
		}
	}

	return ret;
}

bool Boards::isMine(int x, int y) {
	if (inArea(x,y)&&board[x][y] == 9)return true;
	return false;
}

bool Boards::isFull() {
	for (int x = 0; x < boardx; x++) {
		for (int y = 0; y < boardy; y++) {
			if (board[x][y] != -1)
				if (isPutable(x, y))return false;
		}
	}
	return true;
}

Boards::~Boards() {
	std::vector<std::vector<int>>().swap(board);
}
