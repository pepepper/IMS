#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
class Graphic {
public:
	Graphic();
	void StartGame(int x, int y);
	void Put(std::vector<std::vector<int>> &delta);
	void mine();
	void update();
	void netchangeturn(long long, int);
	void end();
	int dialogmode = 0;//1 yesonly 2 yesno
	void FreeDialog();
	void updateDialog();
	void QuitinGameDialogbox();
	void EndGameDialogBox(int turn, int howturn);
	void ClearGameDialogBox(int howturn);
	void ConnectionclosedDialogBox();
	SDL_Rect rect = { 0, 0, 0, 0 };
	~Graphic();
private:
	void Render_UTF8_Text(std::vector<std::string> text);
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *board, *tex,*dialog ,*bomb;
	SDL_Rect  komarect = { 0, 0, 16, 16 };
	TTF_Font *font;
};
//192 +48
