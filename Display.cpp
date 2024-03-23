#include "Display.h"
Display::Display() {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window* window = SDL_CreateWindow("Inner Chess Board", 2300, 70, MAX_DIM, MAX_DIM, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}
void Display::showBoard(Board inputBoard) {
	SDL_SetRenderDrawColor(renderer, 184, 140, 100, 255);
	SDL_RenderClear(renderer);
	SDL_Surface* imagePiece;
	SDL_Texture* imageTexture;
	SDL_Rect placement;
	for (int rows = 0; rows < 8; rows++) {
		for (int columns = 0; columns < 8; columns++) {
			if ((rows + columns) % 2 == 0) {
				//color in rectangle every other square for checkerboard pattern
				SDL_Rect otherColorRect = { columns * MAX_DIM/8, rows * MAX_DIM / 8, MAX_DIM / 8, MAX_DIM / 8};
				SDL_SetRenderDrawColor(renderer, 240, 220, 180, 255);
				SDL_RenderFillRect(renderer, &otherColorRect);
			}
			BoardPiece boardPiece = inputBoard.at(rows, columns);
			//if no piece is there
			if (boardPiece.pieceType == -1) {
				continue;
			}
			std::string filename = "images/";
			if (boardPiece.isComputerPiece) {
				filename = filename + "White/";
			}
			else {
				filename = filename + "Black/";
			}
			filename = filename + boardPiece.pieceType;
			filename = filename + ".png";
			//std::cout << filename << std::endl;
			imagePiece = IMG_Load(filename.c_str());
			imageTexture = SDL_CreateTextureFromSurface(renderer, imagePiece);
			placement = { columns * MAX_DIM / 8, rows * MAX_DIM / 8, imagePiece->w, imagePiece->h };
			SDL_RenderCopy(renderer, imageTexture, NULL, &placement);
		}
	}
	SDL_RenderPresent(renderer);
	return;
}
void Display::displayBoard() {
	SDL_RenderPresent(renderer);
}