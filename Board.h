#pragma once
#include "VisualPiece.h"
#include "BoardPiece.h"
#include "Imports.h"
#include "DoublyLinkedList.h"
#include "SinglyLinkedList.h"

struct TakeContainer {
	int takes;
	std::vector<std::pair<int, int>> moves;
};

class Board
{
public:
	Board();
	SinglyLinkedList computerPawns;
	SinglyLinkedList playerPawns;
	SinglyLinkedList computerPieces;
	SinglyLinkedList playerPieces;
	bool visualToBoard(std::vector<VisualPiece>);
	void setComputerColor(bool isComputerWhite);
	BoardPiece at(int row, int col);
	void makeMove(std::string move);
	void makePromotion(std::vector<VisualPiece> pieces, std::string move);
	void setPreviousMove(std::string move);
	bool computerPlayingAsWhite;
	TakeContainer getMoves(BoardPiece piece);
	std::pair<BoardPiece, BoardPiece> makeTemporaryMove(int row, int col, int destRow, int destCol);
	void undoTemporaryMove(int prevRow, int prevCol, int nowRow, int nowCol, std::pair<BoardPiece, BoardPiece>);
	std::string coordToString(int fromRow, int fromCol, int toRow, int toCol);
	bool inCheck(bool computerPieces);
	int counter2 = 0;
private:
	BoardPiece empty;
	BoardPiece backingBoard[8][8];
	std::map<char, int> pieceWeights;

	DoublyLinkedList ranks[8];
	DoublyLinkedList files[8];
	DoublyLinkedList diagonalsL2R[15];
	DoublyLinkedList diagonalsR2L[15];

	std::string previousMove;

	void hashPawn(VisualPiece visualPiece);
	void boardToLinkedList();
	bool compareBoardPieces(const BoardPiece& first, const BoardPiece& second);
	void boardToDoublyLinkedList();
	int rankHash(int row, int col);
	int fileHash(int row, int col);
	int diagonalL2RHash(int row, int col);
	int diagonalR2LHash(int row, int col);

	void movePiece(int fromRow, int fromCol, int toRow, int toCol);
	bool inBounds(int row, int col);
	bool inFutureCheck(int row, int col, int destRow, int destCol);
	TakeContainer getPawnMoves(BoardPiece piece);
	TakeContainer getKnightMoves(BoardPiece piece);
	TakeContainer getBishopMoves(BoardPiece piece);
	TakeContainer getRookMoves(BoardPiece piece);
	TakeContainer getQueenMoves(BoardPiece piece);
	TakeContainer getKingMoves(BoardPiece piece);
	bool checkRowEmpty(int row, int fromCol, int toCol);

	void doCastle(int row, int col, int destRow, int destCol);
	void undoCastle(std::pair<BoardPiece, BoardPiece> previousPieces);
	void doEnPassant(int row, int col, int destRow, int destCol);
	void undoEnPassant(int destRow, int destCol, std::pair<BoardPiece, BoardPiece> previousPieces);
	void doPromotion(int row, int col, int destRow, int destCol);
	void undoPromotion(int destRow, int destCol, std::pair<BoardPiece, BoardPiece> previousPieces);
	void unMovePiece(int destRow, int destCol, std::pair<BoardPiece, BoardPiece> previousPieces);
	void removePiece(int row, int col);
	void insertPiece(BoardPiece piece);

	//std::pair<BoardPiece, BoardPiece> makeTemporaryMove(int row, int col, int destRow, int destCol);
	//void undoTemporaryMove(int prevRow, int prevCol, int nowRow, int nowCol, std::pair<BoardPiece, BoardPiece>);

	int knightMoveList[8][2] = { {1,2},{-1,2},{1,-2},{-1,-2},{2,1},{-2,1},{2,-1},{-2,-1} };
	int kingMoveList[8][2] = { {1,1},{-1,1},{1,-1},{-1,-1},{1,0},{-1,0},{0,1},{0,-1} };
};
