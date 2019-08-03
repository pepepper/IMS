#pragma once
#include <string>
#include "Graphic.h"
#include "net.h"
#include "Game.h"
#include <thread>
class ims{
	public:
	ims();
	~ims();
	void run();
	void setting();
	private:
	std::string ip, pass, strminesize, strsize, strroom;
	unsigned long long room;
	SDL_Event e;
	int x, y, mode = -1, netmode = -1, netret,end;
	long long myturn = 0;
	Uint32 eventid;
	std::unique_ptr<Game> game;
	std::unique_ptr<Net> net;
	std::unique_ptr<Graphic> graphic;
	std::thread netthread;
	void netstart();
};
