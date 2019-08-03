#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
class Graphic {
public:
	Graphic();
	void StartGame(int x, int y);
	void resetgame();
	void Put(std::vector<std::vector<int>> &delta);
	void mine();
	void update();
	void netchangeturn(long long, int);
	void end();
	void fillscreen();
	int dialogmode = 0;//1 yesonly 2 yesno
	int settingdialog = 	1;//1 modeset 2 netmodeselect 3 boardselect 4 boardsize 5 mineselect 6 minesizeset 7 serverselect 8 roomselect 9 
	void FreeDialog();
	void updateDialog();
	void QuitinGameDialogbox();
	void EndGameDialogBox(int turn, int howturn);
	void ClearGameDialogBox(int howturn);
	void ConnectionclosedDialogBox();
	void modeselect();
	void netmodeselect();
	void boardselect();
	void boardsize(std::string input);
	void mineselect();
	void minesize(int max,std::string input);
	void serverselect(std::string input);
	void roomselect(std::string input);
	void passwordselect();
	void password(std::string input);
	void autoroomselect();
	void automatchselect();
	SDL_Rect rect = { 0, 0, 0, 0 };
	~Graphic();
private:
	void Render_UTF8_Text(std::vector<std::string> text);
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *board, *tex,*dialog ,*bomb,*reset;
	SDL_Rect  komarect = { 0, 0, 16, 16 }, resetrect = {0,0,128,32};
	TTF_Font *font;
};
//192 +48
