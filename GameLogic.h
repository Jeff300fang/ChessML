#pragma once
#define NOMINMAX
#include "VisualCapture.h"
#include "Board.h"
#include <thread>
#include <chrono>
#include <string>
#include "Display.h"
#include "StaticBoardEvaluation.h"
#include <algorithm>
#include <map>
#include "MovesContainer.h"

#define MAX_DEPTH 3
#define MAX_QDEPTH 0

struct HashContainer {
	int depth;
	int eval;
	SinglyLinkedList compPieces;
	SinglyLinkedList playPieces;
};

struct TerminalNode {
	bool is;
	int eval;
};

class GameLogic
{
public:
	GameLogic();
	int startPlay();
private:
	Display displayInstance;
	VisualCapture visualInstance;
	Board theBoard;
	int gameLoop();
	std::string getBestMove();
	StaticBoardEvaluation eval;
	int alphabeta(int alpha, int beta, bool computerMove, int depth);
	bool inCheckMate(std::vector<MovesContainer> movePair, bool computer);
	bool inDraw(std::vector<MovesContainer> possibleMoves, bool computer);
	int quiesce(int alpha, int beta, bool computerMove, int depth);
	int counter = 0;
	std::pair<bool, int> getHash(int depth);
	int hash();
	std::map<int, std::vector<HashContainer>> hashTable;
	void insertHash(int currDepth, int eval);
	static bool compareMovesContainer(const MovesContainer& a, const MovesContainer& b);

	TerminalNode getTerminalNode(std::vector<MovesContainer>);
};

	