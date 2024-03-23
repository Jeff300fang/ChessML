#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Board.h"
#include "Imports.h"

class Display
{
public:
	Display();
	void showBoard(Board inputBoard);
	void displayBoard();
private:
	SDL_Renderer* renderer;
	const int MAX_DIM = 400;
};

