#include "Board.h"
Board::Board() {
	pieceWeights['K'] = 20;
	pieceWeights['Q'] = 9;
	pieceWeights['R'] = 5;
	pieceWeights['B'] = 3;
	pieceWeights['N'] = 3;
	pieceWeights['P'] = 1;
	computerPlayingAsWhite = false;

	empty.pieceType = -1;
	empty.rowCoord = -1;
	empty.colCoord = -1;
	std::cout << "Board done" << std::endl;
}
bool Board::visualToBoard(std::vector<VisualPiece> visualPieces) {
	bool firstMoveMade = false;
	for (VisualPiece visualPiece : visualPieces) {
		backingBoard[visualPiece.rowCoord][visualPiece.colCoord].pieceType = visualPiece.pieceType;
		backingBoard[visualPiece.rowCoord][visualPiece.colCoord].isComputerPiece = (computerPlayingAsWhite == visualPiece.isWhite) ? true : false;
		backingBoard[visualPiece.rowCoord][visualPiece.colCoord].colCoord = visualPiece.colCoord;
		backingBoard[visualPiece.rowCoord][visualPiece.colCoord].rowCoord = visualPiece.rowCoord;
		backingBoard[visualPiece.rowCoord][visualPiece.colCoord].pieceWeight = pieceWeights[visualPiece.pieceType];
		//see if has moved yet in the case that the opponent is white and has first move
		if (visualPiece.rowCoord != 0 && visualPiece.rowCoord != 1 && visualPiece.rowCoord != 6 && visualPiece.rowCoord != 7) {
			backingBoard[visualPiece.rowCoord][visualPiece.colCoord].hasMoved = true;
			firstMoveMade = true;
		}

		//insert pawn hashing
		if (backingBoard[visualPiece.rowCoord][visualPiece.colCoord].pieceType == 'P') {
			hashPawn(visualPiece);
		}

	}
	boardToLinkedList();
	boardToDoublyLinkedList();
	return(firstMoveMade);
}
void Board::hashPawn(VisualPiece visualPiece) {
	if (backingBoard[visualPiece.rowCoord][visualPiece.colCoord].isComputerPiece) {
		computerPawns.insert(backingBoard[visualPiece.rowCoord][visualPiece.colCoord]);
	}
	else {
		playerPawns.insert(backingBoard[visualPiece.rowCoord][visualPiece.colCoord]);
	}
}
void Board::setComputerColor(bool computerColor) {
	computerPlayingAsWhite = computerColor;
	return;
}
BoardPiece Board::at(int row, int col) {
	return(backingBoard[row][col]);
}
bool Board::compareBoardPieces(const BoardPiece &first, const BoardPiece &second) {
	return(first.pieceWeight > second.pieceWeight );
}
void Board::boardToLinkedList() {
	BoardPiece currentPiece;
	for (int rows = 0; rows < 8; rows++) {
		for (int columns = 0; columns < 8; columns++) {
			currentPiece = backingBoard[rows][columns];
			if (currentPiece.pieceType == -1) {
				continue;
			}
			//std::cout << "here" << std::endl;
			if (currentPiece.isComputerPiece) {
				computerPieces.insert(currentPiece);
			}
			else {
				playerPieces.insert(currentPiece);
			}
		}
	}
}
int Board::rankHash(int row, int column) {
	return(row);
}
int Board::fileHash(int row, int column) {
	return(column);
}
int Board::diagonalL2RHash(int row, int column) {
	return(column - row + 7);
}
int Board::diagonalR2LHash(int row, int column) {
	return(14 - column - row);
}
void Board::boardToDoublyLinkedList() {
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			if (backingBoard[row][column].pieceType == -1) {
				continue;
			}
			ranks[rankHash(row, column)].insert(backingBoard[row][column]);
			files[fileHash(row, column)].insert(backingBoard[row][column]);
			diagonalsL2R[diagonalL2RHash(row, column)].insert(backingBoard[row][column]);
			diagonalsR2L[diagonalR2LHash(row, column)].insert(backingBoard[row][column]);
		}
	}
	return;
}
bool Board::inBounds(int row, int col) {
	return(0 <= row && row <= 7 && 0 <= col && col <= 7);
}

void Board::removePiece(int row, int col) {
	//completely remove takingPiece
	BoardPiece removePiece = backingBoard[row][col];
	if (removePiece.pieceType == -1) {
		//std::cout << "Attemptd to remove empty piece" << std::endl;
		return;
	}
	if (removePiece.isComputerPiece) {
		computerPieces.remove(removePiece);
		if (removePiece.pieceType == 'P') {
			computerPawns.remove(removePiece);
		}
	}
	else {
		playerPieces.remove(removePiece);
		if (removePiece.pieceType == 'P') {
			playerPawns.remove(removePiece);
		}
	}
	ranks[rankHash(removePiece.rowCoord, removePiece.colCoord)].remove(removePiece);
	files[fileHash(removePiece.rowCoord, removePiece.colCoord)].remove(removePiece);
	diagonalsL2R[diagonalL2RHash(removePiece.rowCoord, removePiece.colCoord)].remove(removePiece);
	diagonalsR2L[diagonalR2LHash(removePiece.rowCoord, removePiece.colCoord)].remove(removePiece);

	backingBoard[row][col] = empty;
	return;
}
void Board::insertPiece(BoardPiece piece) {
	//insert piece
	BoardPiece insertPiece = piece;
	if (insertPiece.pieceType == -1) {
		//std::cout << "Attempted to insert empty piece" << std::endl;
		return;
	}
	if (insertPiece.isComputerPiece) {
		computerPieces.insert(insertPiece);
		if (insertPiece.pieceType == 'P') {
			computerPawns.insert(insertPiece);
		}
	}
	else {
		playerPieces.insert(insertPiece);
		if (insertPiece.pieceType == 'P') {
			playerPawns.insert(insertPiece);
		}
	}
	ranks[rankHash(insertPiece.rowCoord, insertPiece.colCoord)].insert(insertPiece);
	files[fileHash(insertPiece.rowCoord, insertPiece.colCoord)].insert(insertPiece);
	diagonalsL2R[diagonalL2RHash(insertPiece.rowCoord, insertPiece.colCoord)].insert(insertPiece);
	diagonalsR2L[diagonalR2LHash(insertPiece.rowCoord, insertPiece.colCoord)].insert(insertPiece);
	
	backingBoard[piece.rowCoord][piece.colCoord] = insertPiece;
	return;
}
void Board::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
	BoardPiece newPiece = backingBoard[fromRow][fromCol].copy(); //PLACE FOR ERROR
	newPiece.colCoord = toCol; //if changed here
	newPiece.rowCoord = toRow;
	newPiece.hasMoved = true;
	//here
	removePiece(toRow, toCol);
	removePiece(fromRow, fromCol); //will it affect here?
	insertPiece(newPiece);
	return;
}
void Board::makePromotion(std::vector<VisualPiece> pieces, std::string move) {
	int fromRow = move[0] - '0';
	int fromCol = move[1] - '0';
	int destRow = move[2] - '0';
	int destCol = move[3] - '0';
	VisualPiece correctPiece = pieces[0];
	for (auto piece: pieces) {
		if (piece.rowCoord == destRow 
			&& piece.colCoord == destCol) {
			correctPiece = piece;
		}
	}
	removePiece(fromRow, fromCol);
	removePiece(destRow, destCol);
	BoardPiece newPiece;
	newPiece.rowCoord = destRow;
	newPiece.colCoord = destCol;
	newPiece.hasMoved = true;
	newPiece.isComputerPiece = false;
	newPiece.pieceType = correctPiece.pieceType;
	newPiece.pieceWeight = pieceWeights[newPiece.pieceType];
	insertPiece(newPiece);
	return;
}

void Board::doCastle(int row, int col, int destRow, int destCol) {
	//determine which way to castle
	bool castle2Left = false;
	if (col - destCol > 0) {
		castle2Left = true;
	}
	//move king piece
	movePiece(row, col, destRow, destCol);
	//move rook piece
	movePiece(row, (castle2Left) ? 0 : 7, destRow, destCol + ((castle2Left) ? 1 : -1));
	return;
} //DEBUG
void Board::undoCastle(std::pair<BoardPiece, BoardPiece> previousPieces) {
	bool castledLeft = (previousPieces.second.colCoord == 0) ? true : false;
	//set current king and rook to empty squares
	removePiece(previousPieces.first.rowCoord, previousPieces.first.colCoord + ((castledLeft) ? -2 : 2));
	removePiece(previousPieces.first.rowCoord, previousPieces.first.colCoord + ((castledLeft) ? -1 : 1));
	
	//put back king and rook
	insertPiece(previousPieces.first);
	insertPiece(previousPieces.second);
	return;
}
void Board::doEnPassant(int row, int col, int destRow, int destCol) {
	movePiece(row, col, destRow, destCol);
	removePiece(row, destCol);
	return;
}
void Board::undoEnPassant(int destRow, int destCol, std::pair<BoardPiece, BoardPiece> previousPieces) {
	removePiece(destRow, destCol);
	insertPiece(previousPieces.first);
	insertPiece(previousPieces.second);
}
void Board::doPromotion(int row, int col, int destRow, int destCol) {
	removePiece(destRow, destCol);
	BoardPiece temp = backingBoard[row][col].copy();
	removePiece(row, col);
	temp.colCoord = destCol;
	temp.rowCoord = destRow;
	temp.pieceType = 'Q';
	temp.pieceWeight = pieceWeights[temp.pieceType];
	insertPiece(temp);
	return;
}
void Board::undoPromotion(int destRow, int destCol, std::pair<BoardPiece, BoardPiece> previousPieces) {
	removePiece(destRow, destCol);
	insertPiece(previousPieces.first);
	insertPiece(previousPieces.second);
	return;
}
void Board::unMovePiece(int destRow, int destCol, std::pair<BoardPiece, BoardPiece> previousPieces) {
	removePiece(destRow, destCol);
	insertPiece(previousPieces.first);
	insertPiece(previousPieces.second);
	return;
}

std::pair<BoardPiece, BoardPiece> Board::makeTemporaryMove(int row, int col, int destRow, int destCol) {
	//return two BoardPieces, first piece will be the piece before it has moved
	//second piece will be any taken piece or castled piece
	//if nothing, will be empty
	//makes it easier to undo the move
	std::pair<BoardPiece, BoardPiece> returnPair;
	BoardPiece secondaryAffected;
	//if king is moved and it is a castle move
	if (backingBoard[row][col].pieceType == 'K' 
		&& (abs(col - destCol) > 1)) {
		secondaryAffected = backingBoard[row][((col - destCol) > 0) ? 0 : 7].copy();
		returnPair.first = backingBoard[row][col].copy();
		returnPair.second = secondaryAffected;
		//make castle move and adjust all backing structures
		doCastle(row, col, destRow, destCol);
		return(returnPair);
	}
	//en passant
	//if pawn, moving diagonal, and nothing in diagonal --> en passant
	if (backingBoard[row][col].pieceType == 'P' 
		&& col != destCol
		&& backingBoard[destRow][destCol].pieceType == -1) {
		secondaryAffected = backingBoard[row][destCol].copy();
		returnPair.first = backingBoard[row][col].copy();
		returnPair.second = secondaryAffected;
		doEnPassant(row, col, destRow, destCol);
		return(returnPair);
	}
	//promotion
	if (backingBoard[row][col].pieceType == 'P'
		&& destRow == (backingBoard[row][col].isComputerPiece ? 0 : 7)) {
		secondaryAffected = backingBoard[destRow][destCol].copy();
		returnPair.first = backingBoard[row][col].copy();
		returnPair.second = secondaryAffected;
		doPromotion(row, col, destRow, destCol);
		return(returnPair);
	}
	//cases regular move
	secondaryAffected = backingBoard[destRow][destCol].copy();
	returnPair.first = backingBoard[row][col].copy();
	returnPair.second = secondaryAffected;
	movePiece(row, col, destRow, destCol);
	return(returnPair);
}
void Board::undoTemporaryMove(int prevRow, int prevCol, int destRow, int destCol, std::pair<BoardPiece, BoardPiece> previousPieces) {
	//if move was a castle, undo move
	if (backingBoard[destRow][destCol].pieceType == 'K') {
		if (abs(destCol - prevCol) > 1) {
			//castled
			undoCastle(previousPieces);
			return;
		}
	}
	if (backingBoard[destRow][destCol].pieceType == 'P') {
		//if they were originally on the same row that means empessant
		if (previousPieces.first.rowCoord == previousPieces.second.rowCoord) {
			undoEnPassant( destRow, destCol,previousPieces);
			return;
		}
	}
	//promotion
	if (previousPieces.first.pieceType == 'P' 
		&& previousPieces.first.rowCoord == (previousPieces.first.isComputerPiece ? 1 : 6)) {
		undoPromotion(destRow, destCol, previousPieces);
		return;
	}
	unMovePiece(destRow, destCol, previousPieces);
	return;
}
bool Board::inCheck(bool computerKing) {
	BoardPiece targetKing;
	int testRow, testCol;
	if (computerKing) {
		targetKing = computerPieces.getHead();
	}
	else {
		targetKing = playerPieces.getHead();
	}
	//check for knights
	for (int tryMoves = 0; tryMoves < 8; tryMoves++) {
		//if there is a knight in an attack position and is not same color knight, return true;
		testRow = targetKing.rowCoord + knightMoveList[tryMoves][0];
		testCol = targetKing.colCoord + knightMoveList[tryMoves][1];
		if (inBounds(testRow, testCol)
			&& backingBoard[testRow][testCol].pieceType == 'N'
			&& backingBoard[testRow][testCol].isComputerPiece != computerKing) {
			return(true);
		}
	}
	
	//check for pawns
	testRow = targetKing.rowCoord + ((computerKing) ? -1 : 1);
	testCol = targetKing.colCoord - 1;
	//check both sides
	for (int side = 0; side < 2; side++) {
		if (inBounds(testRow, testCol)
			&& backingBoard[testRow][testCol].pieceType == 'P'
			&& backingBoard[testRow][testCol].isComputerPiece != computerKing) {
			return(true);
		}
		testCol = targetKing.colCoord + 1;
	}

	//check for king
	for (int tryMoves = 0; tryMoves < 8; tryMoves++) {
		testRow = targetKing.rowCoord + kingMoveList[tryMoves][0];
		testCol = targetKing.colCoord + kingMoveList[tryMoves][1];
		if (inBounds(testRow, testCol)
			&& backingBoard[testRow][testCol].pieceType == 'K'
			&& backingBoard[testRow][testCol].isComputerPiece != computerKing) {
			return(true);
		}
	}

	std::shared_ptr<Node> kingNode;
	//test for rooks, queens
	//files
	kingNode = files[fileHash(targetKing.rowCoord, targetKing.colCoord)].getKing(computerKing);
	if (kingNode->next != nullptr
		&& (kingNode->next->pieceVal.pieceType == 'R' || kingNode->next->pieceVal.pieceType == 'Q')
		&& kingNode->next->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}
	if (kingNode->prev != nullptr
		&& (kingNode->prev->pieceVal.pieceType == 'R' || kingNode->prev->pieceVal.pieceType == 'Q')
		&& kingNode->prev->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}
	//ranks
	kingNode = ranks[rankHash(targetKing.rowCoord, targetKing.colCoord)].getKing(computerKing);
	if (kingNode->next != nullptr
		&& (kingNode->next->pieceVal.pieceType == 'R' || kingNode->next->pieceVal.pieceType == 'Q')
		&& kingNode->next->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}
	if (kingNode->prev != nullptr
		&& (kingNode->prev->pieceVal.pieceType == 'R' || kingNode->prev->pieceVal.pieceType == 'Q')
		&& kingNode->prev->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}
	
	//test for bishops, queens
	//diagonals L2R
	kingNode = diagonalsL2R[diagonalL2RHash(targetKing.rowCoord, targetKing.colCoord)].getKing(computerKing);
	if (kingNode->next != nullptr
		&& (kingNode->next->pieceVal.pieceType == 'B' || kingNode->next->pieceVal.pieceType == 'Q')
		&& kingNode->next->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}
	if (kingNode->prev != nullptr
		&& (kingNode->prev->pieceVal.pieceType == 'B' || kingNode->prev->pieceVal.pieceType == 'Q')
		&& kingNode->prev->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}
	//diagonals R2L
	kingNode = diagonalsR2L[diagonalR2LHash(targetKing.rowCoord, targetKing.colCoord)].getKing(computerKing);
	if (kingNode->next != nullptr
		&& (kingNode->next->pieceVal.pieceType == 'B' || kingNode->next->pieceVal.pieceType == 'Q')
		&& kingNode->next->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}
	if (kingNode->prev != nullptr
		&& (kingNode->prev->pieceVal.pieceType == 'B' || kingNode->prev->pieceVal.pieceType == 'Q')
		&& kingNode->prev->pieceVal.isComputerPiece != computerKing) {
		return(true);
	}

	//if nothing matches, return false
	return(false);
}
//checks if a move is valid if it does not make the player check itself
bool Board::inFutureCheck(int row, int col, int destRow, int destCol) {
	//std::cout << counter2 << std::endl;
	//if (counter2 == 263296) {
		//std::cout << "here" << std::endl;
	//}
	//counter2++;	//make temporary move
	std::pair<BoardPiece, BoardPiece> previousPieces = makeTemporaryMove(row, col, destRow, destCol);
	//check if in check now
	bool nowInCheck = inCheck(backingBoard[destRow][destCol].isComputerPiece);
	//undo
	undoTemporaryMove(row, col, destRow, destCol, previousPieces);
	return(nowInCheck);
}

TakeContainer Board::getPawnMoves(BoardPiece piece) {
	std::vector<std::pair<int, int>> moves;
	int takeCounter = 0;
	TakeContainer takeContain;

	int direction = (piece.isComputerPiece) ? -1 : 1;
	//check if can take left/right
	// left
	int lookRow = 0, lookCol = 0;
	int currRow = piece.rowCoord, currCol = piece.colCoord;
	lookRow = piece.rowCoord + direction;
	lookCol = piece.colCoord - 1;
	if (inBounds(lookRow, lookCol) 
		&& backingBoard[lookRow][lookCol].pieceType != -1
		&& piece.isComputerPiece != backingBoard[lookRow][lookCol].isComputerPiece
		&& !inFutureCheck(currRow, currCol, lookRow, lookCol)) {
		takeCounter++;
		moves.emplace_back(lookRow, lookCol);
	}
	//right
	lookCol = piece.colCoord + 1;
	if (inBounds(lookRow, lookCol)
		&& backingBoard[lookRow][lookCol].pieceType != -1
		&& piece.isComputerPiece != backingBoard[lookRow][lookCol].isComputerPiece
		&& !inFutureCheck(currRow, currCol, lookRow, lookCol)) {
		takeCounter++;
		moves.emplace_back(lookRow, lookCol);
	}

	//check if it can move forward
	lookCol = piece.colCoord;
	if (inBounds(lookRow, lookCol)
		&& backingBoard[lookRow][lookCol].pieceType == -1
		&& !inFutureCheck(currRow, currCol, lookRow, lookCol)) {
		moves.emplace_back(lookRow, lookCol);
		//check if it can move forward twice
		if (inBounds(lookRow + direction, lookCol)
			&& currRow == (piece.isComputerPiece ? 6 : 1)
			&& backingBoard[lookRow + direction][lookCol].pieceType == -1
			&& !inFutureCheck(currRow, currCol, lookRow + direction, lookCol)) {
			moves.emplace_back(lookRow + direction, lookCol);
		}
	}

	//check en passant
	//left
	//expected value for previous move: 
	// fromRow: currRow - direction * 2]
	// fromCol: currCol - 1
	// toRow: currRow
	// toCol: currCol - 1
	// check if previous move was a pawn moving two forward
	//if there is a pawn next to us
	std::string looking = "";
	if (inBounds(currRow, currCol - 1)
		&& currRow == (piece.isComputerPiece ? 3 : 4)
		&& backingBoard[currRow][currCol - 1].pieceType == 'P'
		&& backingBoard[currRow][currCol - 1].isComputerPiece != piece.isComputerPiece) {
		looking = std::to_string(currRow + direction * 2) + std::to_string(currCol - 1) + std::to_string(currRow) + std::to_string(currCol - 1);
		if (looking == previousMove) {
			moves.emplace_back(currRow + direction, currCol - 1);
			takeCounter++;
		}
	}
	//right
	if (inBounds(currRow, currCol + 1)
		&& currRow == (piece.isComputerPiece ? 3 : 4)
		&& backingBoard[currRow][currCol + 1].pieceType == 'P'
		&& backingBoard[currRow][currCol + 1].isComputerPiece != piece.isComputerPiece) {
		looking = std::to_string(currRow + direction * 2) + std::to_string(currCol + 1) + std::to_string(currRow) + std::to_string(currCol + 1);
		if (looking == previousMove) {
			moves.emplace_back(currRow + direction, currCol + 1);
			takeCounter++;
		}
	}
	takeContain.moves = moves;
	takeContain.takes = takeCounter;
	return(takeContain);
}
TakeContainer Board::getKnightMoves(BoardPiece piece) {
	std::vector<std::pair<int, int>> moves;
	int takeCounter = 0;
	int lookRow, lookCol;
	for (int tryMoves = 0; tryMoves < 8; tryMoves++) {
		lookRow = piece.rowCoord + knightMoveList[tryMoves][0];
		lookCol = piece.colCoord + knightMoveList[tryMoves][1];
		if (inBounds(lookRow, lookCol)
			&& ((backingBoard[lookRow][lookCol].pieceType == -1)
				|| (backingBoard[lookRow][lookCol].pieceType != -1
					&& backingBoard[lookRow][lookCol].isComputerPiece != piece.isComputerPiece))
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, lookRow, lookCol)) {
			//if destination is a piece, must be taking
			if (backingBoard[lookRow][lookCol].pieceType != -1) {
				takeCounter++;
			}
			moves.emplace_back(lookRow, lookCol);
		}
	}
	TakeContainer temp;
	temp.moves = moves;
	temp.takes = takeCounter;
	return(temp);
}
TakeContainer Board::getBishopMoves(BoardPiece piece) {
	std::vector<std::pair<int, int>> moves;
	int takeCounter = 0;
	int rowDir , colDir;
	int lookRow, lookCol;
	for (int direction = 0; direction < 4; direction++) {
		// direction = 0, 1 1
		// direction = 1, -1 1
		// direction = 2, 1 -1 
		// direction = 3, -1 - 1
		rowDir = (direction % 2 == 0) ? 1 : -1;
		colDir = (direction > 1) ? -1 : 1;
		lookRow = piece.rowCoord + rowDir;
		lookCol = piece.colCoord + colDir;
		for (int tryMove = 0; tryMove < 7; tryMove++) {
			if(inBounds(lookRow, lookCol) 
				&& backingBoard[lookRow][lookCol].pieceType == -1
				&& !inFutureCheck(piece.rowCoord, piece.colCoord, lookRow, lookCol)) {
				moves.emplace_back(lookRow, lookCol);
			}
			//if can take, last move
			else if (inBounds(lookRow, lookCol)
				&& (backingBoard[lookRow][lookCol].pieceType != -1
					&& backingBoard[lookRow][lookCol].isComputerPiece != piece.isComputerPiece)
				&& !inFutureCheck(piece.rowCoord, piece.colCoord, lookRow, lookCol)) {
				takeCounter++;
				moves.emplace_back(lookRow, lookCol);
				break;
			}
			else {
				break;
			}
			lookRow = lookRow + rowDir;
			lookCol = lookCol + colDir;
		}
	}
	TakeContainer temp;
	temp.moves = moves;
	temp.takes = takeCounter;
	return(temp);
}
TakeContainer Board::getRookMoves(BoardPiece piece) {
	std::vector<std::pair<int, int>> moves;
	int takeCounter = 0;
	int rowDir, colDir;
	int lookRow, lookCol;
	for (int direction = 0; direction < 4; direction++) {
		//direction: 0, 1 , 0
		//direction: 1, -1, 0
		//direction: 2, 0, 1
		//direction: 3, 0, -1
		rowDir = ((direction % 2 == 0) ? 1 : -1) * ((direction < 2) ? 1 : 0);
		colDir = ((direction % 2 == 0) ? 1 : -1) * ((direction > 1) ? 1 : 0);
		lookRow = piece.rowCoord + rowDir;
		lookCol = piece.colCoord + colDir;
		for (int tryMove = 0; tryMove < 7; tryMove++) {
			if (inBounds(lookRow, lookCol)
				&& backingBoard[lookRow][lookCol].pieceType == -1
				&& !inFutureCheck(piece.rowCoord, piece.colCoord, lookRow, lookCol)) {
				moves.emplace_back(lookRow, lookCol);
			}
			//if can take, last move
			else if (inBounds(lookRow, lookCol)
				&& (backingBoard[lookRow][lookCol].pieceType != -1
					&& backingBoard[lookRow][lookCol].isComputerPiece != piece.isComputerPiece)
				&& !inFutureCheck(piece.rowCoord, piece.colCoord, lookRow, lookCol)) {
				moves.emplace_back(lookRow, lookCol);
				takeCounter++;
				break;
			}
			else {
				break;
			}
			lookRow = lookRow + rowDir;
			lookCol = lookCol + colDir;
		}
	}
	TakeContainer temp;
	temp.moves = moves;
	temp.takes = takeCounter;
	return(temp);
}
TakeContainer Board::getQueenMoves(BoardPiece piece) {
	TakeContainer straightMoves = getRookMoves(piece);
	TakeContainer diagonalMoves = getBishopMoves(piece);
	TakeContainer merged;
	merged.takes = straightMoves.takes + diagonalMoves.takes;
	diagonalMoves.moves.insert(diagonalMoves.moves.end(), straightMoves.moves.begin(), straightMoves.moves.end());
	merged.moves = diagonalMoves.moves;
	return(merged);
}
TakeContainer Board::getKingMoves(BoardPiece piece) {
	std::vector<std::pair<int, int>> moves;
	int takeCounter = 0;
	//check regular moves
	int lookRow, lookCol;
	for (int test = 0; test < 8; test++) {
		lookRow = piece.rowCoord + kingMoveList[test][0];
		lookCol = piece.colCoord + kingMoveList[test][1];
		if (inBounds(lookRow, lookCol)
			&& (backingBoard[lookRow][lookCol].pieceType == -1 
				|| (backingBoard[lookRow][lookCol].pieceType != -1 
					&& backingBoard[lookRow][lookCol].isComputerPiece != piece.isComputerPiece)) 
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, lookRow, lookCol)) {
			if (backingBoard[lookRow][lookCol].pieceType != -1) {
				takeCounter++;
			}
			moves.emplace_back(lookRow, lookCol);
		}
	}
	//check castle moves
	if (piece.isComputerPiece 
		&& !piece.hasMoved
		&& !inCheck(true)) {
		//check left
		if (backingBoard[7][0].pieceType == 'R'
			&& backingBoard[7][0].isComputerPiece
			&& !backingBoard[7][0].hasMoved
			&& checkRowEmpty(piece.rowCoord, 0, piece.colCoord - 1)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord - 1)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord - 2)) {
			moves.emplace_back(piece.rowCoord, piece.colCoord - 2);
		}
		//check right
		if (backingBoard[7][7].pieceType == 'R'
			&& backingBoard[7][7].isComputerPiece
			&& !backingBoard[7][7].hasMoved
			&& checkRowEmpty(piece.rowCoord, piece.colCoord + 1, 7)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord + 1)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord + 2)) {
			moves.emplace_back(piece.rowCoord, piece.colCoord + 2);
		}
	}
	if (!piece.isComputerPiece
		&& !piece.hasMoved
		&& !inCheck(false)) {
		//check left
		if (backingBoard[0][0].pieceType == 'R'
			&& !backingBoard[0][0].isComputerPiece
			&& !backingBoard[0][0].hasMoved
			&& checkRowEmpty(piece.rowCoord, 0, piece.colCoord - 1)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord - 1)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord - 2)) {
			moves.emplace_back(piece.rowCoord, piece.colCoord - 2);
		}
		//check right
		if (backingBoard[0][7].pieceType == 'R'
			&& !backingBoard[0][7].isComputerPiece
			&& !backingBoard[0][7].hasMoved
			&& checkRowEmpty(piece.rowCoord, piece.colCoord + 1, 7)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord + 1)
			&& !inFutureCheck(piece.rowCoord, piece.colCoord, piece.rowCoord, piece.colCoord + 2)) {
			moves.emplace_back(piece.rowCoord, piece.colCoord + 2);
		}
	}
	TakeContainer temp;
	temp.moves = moves;
	temp.takes = takeCounter;
	return(temp);
}
bool Board::checkRowEmpty(int row, int fromCol, int toCol) {
	for (int checkRow = fromCol; checkRow < toCol; checkRow++) {
		if (backingBoard[row][checkRow].pieceType != -1) {
			return(false);
		}
	}
	return(true);
}

TakeContainer Board::getMoves(BoardPiece piece) {
	if (piece.pieceType == 'P') {
		return(getPawnMoves(piece));
	}
	if (piece.pieceType == 'N') {
		return(getKnightMoves(piece));
	}
	if (piece.pieceType == 'B') {
		return(getBishopMoves(piece));
	}
	if (piece.pieceType == 'R') {
		return(getRookMoves(piece));
	}
	if (piece.pieceType == 'Q') {
		return(getQueenMoves(piece));
	}
	return(getKingMoves(piece));
}

void Board::makeMove(std::string move) {
	makeTemporaryMove(move[0] - '0', move[1] - '0', move[2] - '0', move[3] - '0');
	setPreviousMove(move);
	return;
}

void Board::setPreviousMove(std::string move) {
	previousMove = move;
	return;
}

std::string Board::coordToString(int fromRow, int fromCol, int toRow, int toCol) {
	std::string moveString = "";
	moveString = std::to_string(fromRow) + std::to_string(fromCol) + std::to_string(toRow) + std::to_string(toCol);
	return(moveString);
}