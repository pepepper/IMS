#include <memory>
#include <iostream>
#include <typeinfo>
#include <tuple>
#include <iomanip>
#include <sstream>
#include "ims.h"

ims::ims() {
	SDL_Init(SDL_INIT_EVERYTHING);
	eventid = SDL_RegisterEvents(1);
	graphic.reset(new Graphic());
}

ims::~ims() {
	game.release();
	graphic.release();
	SDL_Quit();
	if (mode == 1) {
		net->closing();
		netthread.join();
		net.release();
	}
}

void ims::run() {
	end = 1;
	graphic->StartGame(game->board->boardx, game->board->boardy);
	graphic->update();
	while (end ) {
		while (SDL_WaitEvent(&e)) {
			if (e.type == eventid) {
				graphic->Put(game->board->delta);
				graphic->netchangeturn(myturn, game->turn);
				graphic->update();
			}
			if (graphic->dialogmode)switch (e.type) {
				case SDL_KEYUP:
					if (graphic->dialogmode == 1) {
						graphic->dialogmode = 0;
						graphic->update();
					} else if (graphic->dialogmode == 2) {
						if (e.key.keysym.sym == SDLK_y) {
							game.release();
							graphic.release();
							SDL_Quit();
							if (mode == 1) {
								net->closing();
								netthread.join();
								net.release();
							}
							return;
						} else if (e.key.keysym.sym == SDLK_n) {
							graphic->dialogmode = 0;
							graphic->update();
						}
					}
					break;
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
						graphic->updateDialog();
					}
					break;
				case SDL_QUIT:
					break;
			} else switch (e.type) {
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
						graphic->update();
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (e.button.x < 128 && e.button.y < 32) {
						graphic->resetgame();
						game.release();
						if (mode == 1) {
							net->closing();
							netthread.join();
							net.release();
						}
						mode = -1;
						netmode = -1;
						myturn = 0;
						setting();
					} else {
						x = (int)((e.button.x - graphic->rect.x) / 16);
						y = (int)((e.button.y - graphic->rect.y - 32) / 16);
						if (mode == 0 || (myturn == game->turn&&net->ready)) {
							if (game->board->isMine(x, y)) {
								graphic->mine();
								game->openall();
								graphic->Put(game->board->delta);
								graphic->EndGameDialogBox(game->turn, game->howturn);
								graphic->end();
							}
							if (game->open(x, y)) {
								if (mode == 1)net->put(x, y);
								graphic->Put(game->board->delta);
								if (mode == 1) graphic->netchangeturn(myturn, game->turn);
								graphic->update();
								if (game->full) {
									graphic->ClearGameDialogBox(game->howturn);
									graphic->end();
									game->full = false;
								}
							}
						}
					}
					break;
				case SDL_QUIT:
					graphic->QuitinGameDialogbox();
					break;
			}
			if (mode == 1 && net->closed == 1 && net->ready == 1) {
				graphic->ConnectionclosedDialogBox();
				graphic->end();
				net->ready = 0;
			}
		}
	}
}

void ims::netstart() {
	netthread = std::thread([this] {
		SDL_Event graph;
		while (net->closed == 0) {
			std::tuple<std::string, int, int> action = net->get();
			if (std::get<0>(action).find("nodata") != std::string::npos) {
				net->closed = 1;
			} else if (std::get<0>(action).find("OPEN") != std::string::npos && netmode != game->turn) {
				game->open(std::get<1>(action), std::get<2>(action));
				SDL_zero(graph);
				graph.type = eventid;
				SDL_PushEvent(&graph);
			} else if (std::get<0>(action).find("JOIN") != std::string::npos) {
				graphic->netchangeturn(netmode == game->turn, game->turn);
				net->started = 1;
				net->ready = 1;
				game->joined = ((long long)std::get<1>(action) << 32) + (long long)std::get<2>(action);
				if (!myturn)myturn = game->joined;
			} else if (std::get<0>(action).find("QUIT") != std::string::npos) {
				game->joined--;
				if (myturn > ((long long)(std::get<1>(action) << 32) + (long long)std::get<2>(action)))myturn--;
			}
		}
		});
}

void ims::setting() {
	graphic->settingdialog = 1;
	while (graphic->settingdialog) {
		while (SDL_WaitEvent(&e)) {
			graphic->fillscreen();
			if (graphic->settingdialog == 1) {
				graphic->modeselect();
				if (e.type == SDL_KEYUP) {
					if (e.key.keysym.sym == SDLK_0 || e.key.keysym.sym == SDLK_KP_0) {
						mode = 0;
						graphic->boardselect();
						break;
					} else if (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_KP_1) {
						mode = 1;
						graphic->serverselect(ip);
						break;
					}
				}
			} else if (graphic->settingdialog == 2) {
				graphic->netmodeselect();
				if (e.type == SDL_KEYUP) {
					if (e.key.keysym.sym == SDLK_0 || e.key.keysym.sym == SDLK_KP_0) {
						netmode = 0;
						graphic->boardselect();
						break;
					} else if (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_KP_1) {
						netmode = 1;
						graphic->automatchselect();
						break;
					}
				}
			} else if (graphic->settingdialog == 3) {
				graphic->boardselect();
				if (e.type == SDL_KEYUP) {
					if (e.key.keysym.sym == SDLK_0 || e.key.keysym.sym == SDLK_KP_0) {
						if (mode == 0) {
							game.reset(new Game());
							graphic->settingdialog = 0;
							break;
						}
						if (mode == 1)graphic->autoroomselect();
						break;
					} else if (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_KP_1) {
						graphic->boardsize(strsize);
						break;
					}
				}
			} else if (graphic->settingdialog == 4) {
				if (e.type == SDL_TEXTINPUT) {
					strsize += e.text.text;
					graphic->boardsize(strsize);
				} else if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
					std::stringstream stream(strsize);
					std::string temp;
					std::vector<std::string> replys;
					while (std::getline(stream, temp, ' ')) {
						if (!temp.empty()) {
							replys.push_back(temp);
						}
					}
					x = std::stoi(replys[1]);
					y = std::stoi(replys[2]);
					if (x < 21 && y < 21) {
						graphic->mineselect();
					}
				}
			} else if (graphic->settingdialog == 5) {
				graphic->mineselect();
				if (e.type == SDL_KEYUP) {
					if (e.key.keysym.sym == SDLK_0 || e.key.keysym.sym == SDLK_KP_0) {
						if (mode == 1)graphic->passwordselect();
						else {
							game.reset(new Game(x, y));
							graphic->settingdialog = 0;
							break;
						}
						break;
					} else if (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_KP_1) {
						graphic->minesize(x*y, strminesize);
						break;
					}
				}
			} else if (graphic->settingdialog == 6) {
				if (e.type == SDL_TEXTINPUT) {
					strminesize += e.text.text;
					graphic->minesize(x*y, strminesize);
				} else if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
					std::stringstream stream(strminesize);
					std::string temp;
					std::vector<std::string> replys;
					while (std::getline(stream, temp, ' ')) {
						if (!temp.empty()) {
							replys.push_back(temp);
						}
					}
					if (std::stoi(replys[1]) <= x * y) {
						if (mode == 1)graphic->passwordselect();
						else {
							game.reset(new Game(x, y, std::stoi(replys[1])));
							graphic->settingdialog = 0;
							break;
						}
					}
				}
			} else if (graphic->settingdialog == 7) {
				if (e.type == SDL_TEXTINPUT) {
					ip += e.text.text;
					graphic->serverselect(ip);
				} else if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
					netret = net->makeconnect(ip);
					if (netret) {
						graphic->ConnectionclosedDialogBox();
						graphic->settingdialog = 1;
					} else {
						netstart();
						graphic->netmodeselect();
					}
				}
			} else if (graphic->settingdialog == 8) {
				if (e.type == SDL_TEXTINPUT) {
					strroom += e.text.text;
					graphic->roomselect(strroom);
				} else if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
					std::stringstream stream(strroom);
					stream.setf(std::ios::hex, std::ios::basefield);
					stream >> room;
					std::cin.setf(std::ios::dec, std::ios::basefield);
					graphic->passwordselect();
				}
			} else if (graphic->settingdialog == 9) {
				graphic->passwordselect();
				if (e.type == SDL_KEYUP) {
					if (e.key.keysym.sym == SDLK_0 || e.key.keysym.sym == SDLK_KP_0) {
						if (netmode == 0)room = net->makeroom(game->board->boardx, game->board->boardy, game->vmines);
						else {
							std::tuple<int, int, std::vector<int>> size = net->login(room, pass);
							game.reset(new Game(std::get<0>(size), std::get<1>(size), std::get<2>(size)));
							graphic->settingdialog = 0;
							break;
						}
						break;
					} else if (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_KP_1) {
						graphic->password(pass);
						break;
					}
				}
			} else if (graphic->settingdialog == 10) {
				if (e.type == SDL_TEXTINPUT) {
					pass += e.text.text;
					graphic->password(pass);
				} else if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
					if (netmode = 0)room = net->makeroom(game->board->boardx, game->board->boardy, game->vmines, pass);
					else {
						std::tuple<int, int, std::vector<int>> size = net->login(room, pass);
						game.reset(new Game(std::get<0>(size), std::get<1>(size), std::get<2>(size)));
						graphic->settingdialog = 0;
						break;
					}
				}
			} else if (graphic->settingdialog == 11) {
				graphic->autoroomselect();
				if (e.type == SDL_KEYUP) {
					if (e.key.keysym.sym == SDLK_0 || e.key.keysym.sym == SDLK_KP_0) {
						graphic->passwordselect();
						break;
					} else if (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_KP_1) {
						room = net->freeroom(game->board->boardx, game->board->boardy, game->vmines);
						break;
					}
				} 
			}else if (graphic->settingdialog == 12) {
				graphic->automatchselect();
				if (e.type == SDL_KEYUP) {
					if (e.key.keysym.sym == SDLK_0 || e.key.keysym.sym == SDLK_KP_0) {
						graphic->roomselect(strroom);
						break;
					} else if (e.key.keysym.sym == SDLK_1 || e.key.keysym.sym == SDLK_KP_1) {
						room = net->automatch();
						std::tuple<int, int, std::vector<int>> size = net->login(room);
						game.reset(new Game(std::get<0>(size), std::get<1>(size), std::get<2>(size)));
						graphic->settingdialog = 0;
						break;
					}
				}
			}
		}
	}
}
