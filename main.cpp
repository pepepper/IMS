#define SDL_MAIN_HANDLED
#include "ims.h"


int main(int argc, char *argv[]) {
	ims game;
	game.setting();
	game.run();
	game.~ims();
	return 0;
}
