#pragma once
#include "Imports.h"
struct BoardPiece{
	char colCoord = -1;
	char rowCoord = - 1;
	char pieceType = -1;
	bool isComputerPiece = false;
	bool hasMoved = false;
	char pieceWeight = -1;

	std::string toString();
	BoardPiece copy();
};