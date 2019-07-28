#include "Graphic.h"
#include <tuple>
#include <string>

Graphic::Graphic() {
	SDL_Surface *temp;
	SDL_CreateWindowAndRenderer(480, 480, NULL, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	temp = SDL_LoadBMP("./icon.bmp");
	SDL_SetWindowIcon(window, temp);
	SDL_FreeSurface(temp);
	SDL_SetWindowTitle(window, u8"Internet Mine Sweeper");

	temp = SDL_LoadBMP("./masu.bmp");
	board = SDL_CreateTextureFromSurface(renderer, temp);
	SDL_FreeSurface(temp);

	temp = SDL_LoadBMP("./bakuhatsu.bmp");
	bomb = SDL_CreateTextureFromSurface(renderer, temp);
	SDL_FreeSurface(temp);

	TTF_Init();
	font = TTF_OpenFont("Koruri-Regular.ttf", 16);
}


Graphic::~Graphic() {
	if (font) {
		TTF_CloseFont(font);
		font = 0;
	}
	TTF_Quit();
	if (board)SDL_DestroyTexture(board);
	if (board)SDL_DestroyTexture(tex);
	if (dialog)SDL_DestroyTexture(dialog);
	if (renderer)SDL_DestroyRenderer(renderer);
	if (window)SDL_DestroyWindow(window);
}

void Graphic::StartGame(int x, int y) {
	tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, x * 16, y * 16);
	SDL_SetRenderTarget(renderer, tex);
	SDL_RenderClear(renderer);

	SDL_Rect mine = { 0, 0, 16, 16 };
	SDL_Rect pos = { 0, 0, 16, 16 };
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			pos.x = i * 16;
			pos.y = j * 16;
			SDL_RenderCopy(renderer, board, &mine, &pos);
		}
	}
	SDL_SetRenderTarget(renderer, NULL);
	rect.x = (480 - (x * 16)) / 2;
	rect.y = (480 - (y * 16)) / 2;
	rect.w = x * 16;
	rect.h = y * 16;
	SDL_RenderCopy(renderer, tex, NULL, &rect);
}

void Graphic::Put(std::vector<std::vector<int>> &delta) {//[n][0] white/black [n][1] x [n][2] y
	SDL_SetRenderTarget(renderer, tex);
	SDL_Rect mine = { 0, 0, 16, 16 };
	for (std::vector<int> &masu : delta) {
		komarect.x = 16 * masu[1];
		komarect.y = 16 * masu[2];
		mine.x = 16 * masu[0] + 16;
		SDL_RenderCopy(renderer, board, &mine, &komarect);
		masu[0] = masu[1] = masu[2] = -1;
	}
	std::vector<std::vector<int>>().swap(delta);
	SDL_SetRenderTarget(renderer, NULL);
}

void Graphic::mine() {
	for (int i = 0; i < 5; i++) {
		SDL_RenderCopy(renderer, bomb, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_Delay(200);
		SDL_RenderFillRect(renderer, NULL);
		SDL_RenderCopy(renderer, tex, NULL, &rect);
		SDL_RenderPresent(renderer);
		SDL_Delay(200);
	}
}

void Graphic::update() {
	SDL_RenderFillRect(renderer, NULL);
	SDL_RenderCopy(renderer, tex, NULL, &rect);
	SDL_RenderPresent(renderer);
}

void Graphic::netchangeturn(long long you, int turn) {
	std::string title = u8"Internet Mine Sweeper:";
	if (you == turn) {
		title += u8"あなたの番です";
	} else title += turn + u8"番です";
	SDL_SetWindowTitle(window, title.c_str());
}

void Graphic::end() {
	SDL_SetWindowTitle(window, u8"Internet Mine Sweeper:ゲーム終了");
}

void Graphic::Render_UTF8_Text(std::vector<std::string> text) {
	std::vector<std::tuple<SDL_Surface *, int, int>> texts;
	SDL_Texture *texture;
	SDL_Rect dst = { 16, 16, 0, 0 };
	SDL_Color color = { 255, 255, 255, 0 };
	int dialogheight = 0, dialogwidth = 0;
	for (std::string tex : text) {
		int texx, texy;
		TTF_SizeUTF8(font, tex.c_str(), &texx, &texy);
		texts.emplace_back(std::make_tuple(TTF_RenderUTF8_Blended(font, tex.c_str(), color), texx, texy));
		if (dialogwidth < texx)dialogwidth = texx;
		dialogheight += texy;
	}
	dialogheight += 32;
	dialogwidth += 32;
	dialog = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dialogwidth, dialogheight);
	SDL_SetRenderTarget(renderer, dialog);
	for (std::tuple<SDL_Surface *, int, int> textdata : texts) {
		SDL_SetRenderTarget(renderer, NULL);
		texture = SDL_CreateTextureFromSurface(renderer, std::get<0>(textdata));
		SDL_FreeSurface(std::get<0>(textdata));
		dst.w = std::get<1>(textdata);
		dst.h = std::get<2>(textdata);
		SDL_SetRenderTarget(renderer, dialog);
		SDL_RenderCopy(renderer, texture, NULL, &dst);
		dst.y += std::get<2>(textdata);
	}
	SDL_SetRenderTarget(renderer, NULL);
	dst.x = (480 - dialogwidth) / 2;
	dst.y = (480 - dialogheight) / 2;
	dst.w = dialogwidth;
	dst.h = dialogheight;
	SDL_RenderCopy(renderer, dialog, NULL, &dst);
	SDL_RenderPresent(renderer);
}

void Graphic::QuitinGameDialogbox() {
	Render_UTF8_Text({ u8"確認", u8"本当にこのゲームを終了しますか?", u8"Yキー:終了する Nキー:終了しない" });
	dialogmode = 2;
	return;
}

void Graphic::EndGameDialogBox(int turn, int howturn) {
	std::string str = std::to_string(turn) + u8"番が総ターン数" + std::to_string(howturn) + u8"で地雷の犠牲者です";
	Render_UTF8_Text({ u8"ゲーム終了", str, u8"何かキーを押してください" });
	dialogmode = 1;
	return;
}

void Graphic::ClearGameDialogBox(int howturn) {
	std::string str = u8"総ターン数" + std::to_string(howturn) + u8"でクリア";
	Render_UTF8_Text({ u8"ゲーム終了", str, u8"何かキーを押してください" });
	dialogmode = 1;
	return;
}

void Graphic::ConnectionclosedDialogBox() {
	Render_UTF8_Text({ u8"通知", u8"接続が切断されました", u8"何かキーを押してください" });
	dialogmode = 1;
	return;
}

void Graphic::FreeDialog() {
	if (dialog)SDL_DestroyTexture(dialog);
	dialog = 0;
}
void Graphic::updateDialog() {
	SDL_RenderCopy(renderer, tex, NULL, &rect);
	SDL_RenderCopy(renderer, dialog, NULL, NULL);
	SDL_RenderPresent(renderer);
}