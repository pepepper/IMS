#include "Game.h"
#include <random>
#include <utility>
#include  <vector>
Game::Game(){
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::uniform_int_distribution<int> dist(1, 20);
	int x= dist(engine),y= dist(engine);
	std::uniform_int_distribution<int> mines(0, x*y);
	int numMines=mines(engine);
	std::uniform_int_distribution<int> minex(0, x-1);
	std::uniform_int_distribution<int> miney(0, y-1);
	for(int i=0;i<numMines;i++)vmines.emplace_back((minex(engine)<<16)+miney(engine));
	board = new Boards(x, y,vmines);
	turn = 0;
	howturn = 0;
}

Game::Game(int x, int y){
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::uniform_int_distribution<int> mines(0, x*y);
	int numMines=mines(engine);
	std::uniform_int_distribution<int> minex(0, x-1);
	std::uniform_int_distribution<int> miney(0, y-1);
	std::vector<int> vmines;
	for(int i=0;i<numMines;i++)vmines.emplace_back((minex(engine)<<16)+miney(engine));
	board = new Boards(x, y,vmines);
	turn = 0;
	howturn = 0;
}

Game::Game(int x, int y,std::vector<int> vmines){
	board = new Boards(x, y,vmines);
	turn = 0;
	howturn = 0;
}


Game::~Game(){
	delete board;
}

bool Game::open(int x, int y){
	int temp1 = x, temp2 = y;
	if(board->Put(temp1, temp2 )){
		howturn += 1;
		if(joined>howturn)turn++;
		else turn=0;
		full = board->isFull();
		return true;
	}
	return false;
}

void Game::openall(){
	int temp = howturn;
	for (int i = 0; i < board->boardx; i++)
		for (int j = 0; j < board->boardy; j++)
			open(i, j);
	howturn = temp;
}
