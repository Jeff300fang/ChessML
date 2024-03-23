#include "BoardPiece.h"

std::string BoardPiece::toString() {
	if (pieceType == -1) {
		return("");
	}
	std::string piece(1,pieceType);
	std::string row(1,char(rowCoord + '0'));
	std::string col(1,char(colCoord + '0'));
	return(piece + " " + row + " " + col);
}
BoardPiece BoardPiece::copy() {
	BoardPiece newCopy;
	newCopy.colCoord = colCoord;
	newCopy.hasMoved = hasMoved;
	newCopy.isComputerPiece = isComputerPiece;
	newCopy.pieceType = pieceType;
	newCopy.pieceWeight = pieceWeight;
	newCopy.rowCoord = rowCoord;
	return(newCopy);
}