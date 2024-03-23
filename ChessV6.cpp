#include <iostream>
#include <string>
#include "GameLogic.h"
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
	/* something is definitely not right, hanging pieces randomly
		try adding more depth
		try printing out evaluation
	*/ 
	/* Things recently implemented / changed
	* added MovesContainer, TakeContainer
	* begin counting number of takes each move has
	* Need to implement:
	* Move ordering
	* A lot of collisions
	* infinite loop
	* step through
	*/
	GameLogic test;
	std::string userInput;
	std::cout << "ready" << std::endl;
	//std::getline(std::cin, userInput);
	test.startPlay();

	/*std::string userInput;
	VisualCapture test;
	std::cout << "ready" << std::endl;
	while (true) {
		std::getline(std::cin, userInput);
		auto pieces = test.getPieces();
		Board theBoard;
		theBoard.setComputerColor(false);
		theBoard.visualToBoard(pieces);
		StaticBoardEvaluation temp;
		std::cout << temp.boardScore(theBoard) << std::endl;
	}*/
	return(0);
}