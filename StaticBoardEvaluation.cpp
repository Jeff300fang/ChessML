#include "StaticBoardEvaluation.h"
StaticBoardEvaluation::StaticBoardEvaluation() {
    pieceIndex['P'] = 0;
    pieceIndex['N'] = 1;
    pieceIndex['B'] = 2;
    pieceIndex['R'] = 3;
    pieceIndex['Q'] = 4;
    pieceIndex['K'] = 5;
    
    value.push_back(mg_value);
    value.push_back(eg_value);

    mg_table.push_back(mg_pawn_table);
    mg_table.push_back(mg_knight_table);
    mg_table.push_back(mg_bishop_table);
    mg_table.push_back(mg_rook_table);
    mg_table.push_back(mg_queen_table);
    mg_table.push_back(mg_king_table);

    eg_table.push_back(eg_pawn_table);
    eg_table.push_back(eg_knight_table);
    eg_table.push_back(eg_bishop_table);
    eg_table.push_back(eg_rook_table);
    eg_table.push_back(eg_queen_table);
    eg_table.push_back(eg_king_table);

    table.push_back(mg_table);
    table.push_back(eg_table);

    std::cout << "Static board evaluation done" << std::endl;
}

void StaticBoardEvaluation::setInEndGame() {
    int numQueens = 0;
    int numMinorPieces = 0;
    int pawnAdvancement = 0;
    int totalPawns = 0;
    for (auto piece : board.computerPieces) {
        if (piece.pieceType == 'Q') {
            numQueens++;
        }
        if (piece.pieceType == 'R' || piece.pieceType == 'B' || piece.pieceType == 'N') {
            numMinorPieces++;
        }
        if (piece.pieceType == 'P') {
            pawnAdvancement = pawnAdvancement + 6 - piece.rowCoord;
            totalPawns++;
        }
    }
    for (auto piece : board.playerPieces) {
        if (piece.pieceType == 'Q') {
            numQueens++;
        }
        if (piece.pieceType == 'R' || piece.pieceType == 'B' || piece.pieceType == 'N') {
            numMinorPieces++;
        }
        if (piece.pieceType == 'P') {
            pawnAdvancement = pawnAdvancement + piece.rowCoord - 1;
            totalPawns++;
        }
    }
    if ((numQueens + numMinorPieces) < 6 && pawnAdvancement/(float)totalPawns > 1.5) {
        endGame = 1;
    }
    else {
        endGame = 0;
    }
    return;
}

int StaticBoardEvaluation::boardScore(Board theBoard, std::vector<MovesContainer> possibleMoves) {
	//positive score favors computer
	//negatiuve score facotrs player
	int totalScore = 0;
    board = theBoard;
    setInEndGame();
	//piece material
	totalScore = totalScore + pieceMaterial();
	totalScore = totalScore + pieceSquare();
    totalScore = totalScore + pieceMoves(possibleMoves);
	return(totalScore);
}
int StaticBoardEvaluation::pieceMaterial() {
	int materialTotal = 0;
	for (auto piece : board.computerPieces) {
        materialTotal = materialTotal + value.at(endGame).at( pieceIndex[piece.pieceType]);
	}
	for (auto piece : board.playerPieces) {
		materialTotal = materialTotal - value.at(endGame).at(pieceIndex[piece.pieceType]);
	}
	return(materialTotal);
}
int StaticBoardEvaluation::pieceSquare() {
	int squareTotal = 0;
    int correctRow = 0;
    int correctCol = 0;
    for (auto piece : board.computerPieces) {
        correctRow = piece.rowCoord;
        correctCol = piece.colCoord;
        if (!board.computerPlayingAsWhite) {
            correctCol = 7 - piece.colCoord;
        }
        squareTotal = squareTotal + table.at(endGame).at(pieceIndex[piece.pieceType]).at(correctRow * 8 + correctCol);
    }
    for (auto piece : board.playerPieces) {
        correctRow = 7 - piece.rowCoord;
        correctCol = 7 - piece.colCoord;
        if (board.computerPlayingAsWhite) {
            correctCol = piece.colCoord;
        }
        squareTotal = squareTotal - table.at(endGame).at(pieceIndex[piece.pieceType]).at(correctRow * 8 + correctCol);
    }
	return(squareTotal);
}

int StaticBoardEvaluation::pieceMoves(std::vector<MovesContainer> possibleMoves) {
    int total = 0;
    bool currComputerPieces;
    currComputerPieces = possibleMoves.at(0).piece.isComputerPiece;
    for (auto possibleMove : possibleMoves) {
        if (possibleMove.piece.isComputerPiece) {
            total = total + possibleMove.moves.moves.size();
        }
        else {
            total = total - possibleMove.moves.moves.size();
        }
    }
    SinglyLinkedList other;
    if (currComputerPieces) {
        other = board.playerPieces;
    }
    else {
        other = board.computerPieces;
    }
    for (BoardPiece piece : other) {
        TakeContainer moves = board.getMoves(piece);
        if (currComputerPieces) {
            total = total + moves.moves.size();
        }
        else {
            total = total - moves.moves.size();
        }
    }
    return(10 * total);
}