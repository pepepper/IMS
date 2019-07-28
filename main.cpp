#define SDL_MAIN_HANDLED
#include <memory>
#include "Graphic.h"
#include "Game.h"
#include "net.h"
#include <string>
#include <iostream>
#include <typeinfo>
#include <tuple>
#include <thread>
#include <iomanip>

int intGetOption(const char *message) {
	int temp;
	std::cout << message;
	std::cin >> temp;
	return temp;
}

long long llGetOption(const char *message) {
	long long temp;
	std::cout << message;
	std::cin >> temp;
	return temp;
}

std::string strGetOption(const char *message) {
	std::string temp;
	std::cout << message;
	std::cin >> temp;
	return temp;
}
//todo:yourturn
int main(int argc, char *argv[]) {
	std::string ip, pass, arg;
	unsigned long long room;
	SDL_Event e;
	int x, y, mode = -1, netmode = -1, netret;
	long long myturn = 0;
	Uint32 eventid;
	std::unique_ptr<Game> game;
	std::unique_ptr<Net> net;
	std::thread netthread;
	//部屋番号指定
	try {
		mode = intGetOption("モードを選択してください\n0:オフラインでプレイする 1:オンラインでプレイする場合 :");
		if (mode != 0 && mode != 1)throw std::invalid_argument("");
		else if (mode == 0) {//offline
			if (intGetOption("盤面サイズを指定しますか?\n0:指定しない 1:指定する :") == 0) game.reset(new Game());
			else {
				std::string sx, sy;
				int x, y;
				x = intGetOption("100以下で縦のサイズを入力してください:");
				y = intGetOption("100以下で横のサイズを入力してください:");
				if (x > 100 || y > 100)throw std::invalid_argument("");
				game.reset(new Game(x, y));
			}
		} else if (mode == 1) {//online

			net.reset(new Net());
			ip = strGetOption("サーバーのIPアドレスまたはドメインを入力してください:");
			netret = net->makeconnect(ip);
			if (netret) {
				std::cout << "通信エラー:終了します" << std::endl;
				return 1;
			}
			netmode = intGetOption("モードを選択してください\n0:ホストとして部屋を立てる 1:ゲストとして部屋に入る 2:オートマッチング :");
			if (netmode != 0 && netmode != 1 && netmode != 2)throw std::invalid_argument("");
			else if (netmode == 0) {//host
				netmode = -1;
				if (netret == -1) {
					std::cout << "通信エラー:終了します" << std::endl;
					return 1;
				}
				if (intGetOption("盤面サイズを指定しますか?\n0:指定しない 1:指定する :") == 0) {
					game.reset(new Game());
				} else {
					std::string sx, sy;
					int x, y;
					x = intGetOption("100以下で縦のサイズを入力してください:");
					y = intGetOption("100以下で横のサイズを入力してください:");
					if (x > 100 || y > 100)throw std::invalid_argument("");
					game.reset(new Game(x, y));
				}
				std::cout << "パスワードを設定しますか?" << std::endl << "0:設定しない 1:設定する :";
				std::cin >> arg;
				if (!arg.compare("1")) {
					pass = strGetOption("パスワードを入力してください:");

					room = net->makeroom(game->board->boardx, game->board->boardy, game->vmines, pass);
				} else 	room = net->makeroom(game->board->boardx, game->board->boardy, game->vmines);
				std::cout << "部屋番号:" << std::hex << room << std::dec << std::endl;
			} else if (netmode == 1) {//guest
				netmode = 1;
				std::tuple<int, int, std::vector<int>> size;
				std::cin.setf(std::ios::hex, std::ios::basefield);
				room = llGetOption("部屋番号を入力してください:");
				std::cin.setf(std::ios::dec, std::ios::basefield);
				std::cout << "パスワードが設定されていますか?" << std::endl << "0:設定されていない 1:設定されている :";
				std::cin >> arg;
				if (arg.compare("0") && arg.compare("1"))throw std::invalid_argument("");
				else if (!arg.compare("1")) {
					pass = strGetOption("パスワードを入力してください:");
					size = net->login(room, pass);
				} else {
					size = net->login(room);
				}
				if (std::get<0>(size) == -1) {
					std::cout << "通信エラー:終了します" << std::endl;
					return 1;
				}
				game.reset(new Game(std::get<0>(size), std::get<1>(size), std::get<2>(size)));
			} else if (netmode == 2) {//automatch
				long long proom = net->automatch();
				if (proom == 0) {
					netmode = -1;
					game.reset(new Game());
					net->makeroom(game->board->boardx, game->board->boardy, game->vmines);

				} else if (proom != -1) {
					netmode = 1;
					std::tuple<int, int, std::vector<int>> size;
					size = net->login(proom);
					if (std::get<0>(size) == -1) {
						std::cout << "通信エラー:終了します" << std::endl;
						return 1;
					}
					game.reset(new Game(std::get<0>(size), std::get<1>(size), std::get<2>(size)));
				}
			}
		}
	} catch (const std::invalid_argument& e) {
		std::cout << "入力が不正です:終了します";
		return 1;
	}
	SDL_Init(SDL_INIT_EVERYTHING);
	eventid = SDL_RegisterEvents(1);
	Graphic graphic;

	graphic.StartGame(game->board->boardx, game->board->boardy);
	graphic.update();
	if (mode == 1)netthread = std::thread([&game, &net, &graphic, &netmode, &eventid, &myturn] {
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
				graphic.netchangeturn(netmode == game->turn, game->turn);
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
	while (true) {
		while (SDL_WaitEvent(&e)) {
			if (e.type == eventid) {
				graphic.Put(game->board->delta);
				graphic.netchangeturn(myturn, game->turn);
				graphic.update();
			}
			if (graphic.dialogmode)switch (e.type) {
				case SDL_KEYUP:
					if (graphic.dialogmode == 1) {
						graphic.dialogmode = 0;
						graphic.update();
					} else if (graphic.dialogmode == 2) {
						if (e.key.keysym.sym == SDLK_y) {
							game.release();
							graphic.~Graphic();
							SDL_Quit();
							if (mode == 1) {
								net->closing();
								netthread.join();
								net.release();
							}
							return 0;
						} else if (e.key.keysym.sym == SDLK_n) {
							graphic.dialogmode = 0;
							graphic.update();
						}
					}
					break;
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
						graphic.updateDialog();
					}

					break;
				case SDL_QUIT:
					break;
			} else switch (e.type) {
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
						graphic.update();
					}

					break;
				case SDL_MOUSEBUTTONUP:
					x = (int)((e.button.x - graphic.rect.x) / 16);
					y = (int)((e.button.y - graphic.rect.y) / 16);
					if (mode == 0 || (myturn == game->turn&&net->ready)) {
						if (game->board->isMine(x,y)) {
							graphic.mine();
							game->openall();
							graphic.Put(game->board->delta);
							graphic.EndGameDialogBox(game->turn, game->howturn);
							graphic.end();
						}
						if (game->open(x, y)) {
							if (mode == 1)net->put(x, y);
							graphic.Put(game->board->delta);
							if (mode == 1) graphic.netchangeturn(myturn, game->turn);
							graphic.update();
							if (game->full) {
								graphic.ClearGameDialogBox( game->howturn);
								graphic.end();
								game->full = false;
							}
						}
					}
					break;
				case SDL_QUIT:
					graphic.QuitinGameDialogbox();
					break;
			}
			if (mode == 1 && net->closed == 1 && net->ready == 1) {
				graphic.ConnectionclosedDialogBox();
				graphic.end();
				net->ready = 0;
			}
		}
	}
	netthread.join();
	return 0;
}
