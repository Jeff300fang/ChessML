#include "GameLogic.h"
GameLogic::GameLogic() {

}
int GameLogic::gameLoop() {
	//see if the computer is playing white
	bool isComputerWhite = visualInstance.computerPlayingWhite();
	theBoard.setComputerColor(isComputerWhite);
	bool computerMove = isComputerWhite;
	//get pieces and convert it to a board
	std::vector<VisualPiece> pieces = visualInstance.getPieces();
	bool firstMoveMade = theBoard.visualToBoard(pieces);
	std::string move;
	if (firstMoveMade && !isComputerWhite) {
		move = visualInstance.playerMadeMove(isComputerWhite);
		theBoard.setPreviousMove(move);
		computerMove = !computerMove;
	}
	bool gameEnded = false;
	std::string userInput;
	displayInstance.showBoard(theBoard);
	while (!gameEnded) {
		if (computerMove) {
			/*for (auto piece : theBoard.computerPieces) {
				std::cout << piece.toString() << std::endl;
				std::vector<std::pair<int, int>> moves = theBoard.getMoves(piece);
				for (auto move : moves) {
					std::cout << move.first - piece.rowCoord << " " << move.second - piece.colCoord << " ";
				}
				std::cout << std::endl;
			}
			//find move
			std::getline(std::cin, userInput);
			move = visualInstance.playerMadeMove(!isComputerWhite);
			while (move == " ") {
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				move = visualInstance.playerMadeMove(!isComputerWhite);
			}*/
			move = getBestMove();
			visualInstance.moveOnScreen(move);
			std::cout << move << std::endl;
		}
		else {
			/*for (auto piece : theBoard.playerPieces) {
				std::cout << piece.toString() << std::endl;
				std::vector<std::pair<int, int>> moves = theBoard.getMoves(piece);
				for (auto move : moves) {
					std::cout << move.first - piece.rowCoord << " " << move.second - piece.colCoord << " ";
				}
				std::cout << std::endl;
			}*/
			move = visualInstance.playerMadeMove(isComputerWhite);
			while (move == " ") {
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				move = visualInstance.playerMadeMove(isComputerWhite);
			}
		}
		//promotion case
		//if moving into the last rank, and the pieceType is a pawn, promotion
		if (move[2] == '7' 
			&& theBoard.at(move[0] - '0', move[1] - '0').pieceType == 'P'
			&& !computerMove) {
			pieces = visualInstance.getPieces();
			theBoard.makePromotion(pieces, move);
		}
		else {
			theBoard.makeMove(move);
		}
		computerMove = !computerMove;
		displayInstance.showBoard(theBoard);
	}
	return(0);
}
int GameLogic::startPlay() {
	return(gameLoop());
}

/*std::string GameLogic::getBestMove() {
	std::string move = "";
	int best_eval = INT_MIN;
	int alpha = INT_MIN;
	int beta = INT_MAX;

	std::vector < std::pair < BoardPiece, std::vector<std::pair<int, int>>>> computerMoves; 
	for (auto piece : theBoard.computerPieces) {
		computerMoves.emplace_back(piece, theBoard.getMoves(piece));
	}
	for (auto& movePair : computerMoves) {
		std::cout << movePair.first.toString() << std::endl;
		for (const auto& possibleMove : movePair.second) {
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(movePair.first.rowCoord, movePair.first.colCoord, possibleMove.first, possibleMove.second);
			int eval = alphabeta(1, alpha, beta, false);
			theBoard.undoTemporaryMove(movePair.first.rowCoord, movePair.first.colCoord, possibleMove.first, possibleMove.second, affected);

			if (eval >= best_eval) {
				best_eval = eval;
				move = theBoard.coordToString(movePair.first.rowCoord, movePair.first.colCoord, possibleMove.first, possibleMove.second);
			}
			alpha = (alpha > eval) ? alpha : eval;
		}
	}
	//std::cout << eval.boardScore(theBoard) << std::endl;
	return(move);
}*/
int GameLogic::hash() {
	int hashValue = 0;
	for (auto piece : theBoard.computerPieces) {
		hashValue = hashValue ^ (piece.pieceWeight  * (piece.rowCoord * 8 + piece.colCoord));
	}
	for (auto piece : theBoard.playerPieces) {
		hashValue = hashValue ^ (piece.pieceWeight * -1 * (piece.rowCoord * 8 + piece.colCoord));
	}
	return(hashValue);
}

std::pair<bool, int> GameLogic::getHash(int depth) {
	std::pair<bool, int> ret;
	//see if have been in this position before and the position at depth >= previousDepth
	int hashVal = hash();
	//see if the map contains the hash
	if (hashTable.count(hashVal)) {
		if (hashTable.count(hashVal) > 1) {
			std::cout << "Collision" << std::endl;
		}
		for (HashContainer hashInst : hashTable[hashVal]) {
			if (hashInst.depth <= depth
				&& hashInst.compPieces.isEqual(theBoard.computerPieces)
				&& hashInst.playPieces.isEqual(theBoard.playerPieces)) {
				//if the hash is contained, check if the previous depth is less than or equal to,new depth, indicating a deeper level of analysis and make sure that the actual pieces are the same
				ret.first = true;
				ret.second = hashInst.eval;
				return(ret);
			}
		}
	}
	ret.first = false;
	ret.second = -1;
	return(ret);
}
void GameLogic::insertHash(int currDepth, int eval) {
	int hashVal = hash();
	//if hash exists, test if the board position exisits, if it does update the depth and eval
	if (hashTable.count(hashVal)) {
		for (HashContainer hashInst : hashTable[hashVal]){
			if (hashInst.compPieces.isEqual(theBoard.computerPieces)
				&& hashInst.playPieces.isEqual(theBoard.playerPieces)) {
				hashInst.depth = currDepth;
				hashInst.eval = eval;
				return;
			}
		}
	}
	//else insert 
	HashContainer temp;
	temp.compPieces = theBoard.computerPieces;
	temp.playPieces = theBoard.playerPieces;
	temp.depth = currDepth;
	temp.eval = eval;
	hashTable[hashVal].push_back(temp);
	return;
}
/*int GameLogic::quiesce(int alpha, int beta, bool computerMove, int depth) {
	std::pair<bool, int> hashedEval = getHash(depth);
	if (hashedEval.first) {
	//	std::cout << "GET HASHED" << std::endl;
	//	std::cout << "GET HASHED" << std::endl;
	//	std::cout << "GET HASHED" << std::endl;
	//	std::cout << "GET HASHED" << std::endl;
		return(hashedEval.second);
	}
	//std::cout << counter << std::endl;
	//counter++;
	std::vector<std::pair<BoardPiece, std::vector<std::pair<int, int>>>> moves;
	if (computerMove) {
		for (auto piece : theBoard.computerPieces) {
			moves.emplace_back(piece, theBoard.getMoves(piece));
		}
	}
	else {
		for (auto piece : theBoard.playerPieces) {
			moves.emplace_back(piece, theBoard.getMoves(piece));
		}
	}
	if (inCheckMate(moves, computerMove)) {
		insertHash(depth, (computerMove ? INT_MIN : INT_MAX));
		return(computerMove ? INT_MIN : INT_MAX);
	}
	if (inDraw(moves, computerMove)) {
		insertHash(depth, 0);
		return(0);
	}
	if (depth >= MAX_DEPTH + MAX_QDEPTH) {
		int score = eval.boardScore(theBoard);
		insertHash(depth, score);
		return(score);
	}
	int currEval;
	if (computerMove) {
		int max_eval = eval.boardScore(theBoard);
		for (auto thePair : moves) {
			for (auto move : thePair.second) {
				if (theBoard.at(move.first, move.second).pieceType == -1 || theBoard.at(move.first, move.second).isComputerPiece == computerMove) {
					continue;
				}
				std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second);
				currEval = quiesce(alpha, beta, !computerMove, depth + 1);
				theBoard.undoTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second, affected);
				max_eval = (max_eval > currEval) ? max_eval : currEval;
				alpha = (alpha > currEval) ? alpha : currEval;
				if (beta <= alpha) {
					insertHash(depth, max_eval);
					return(max_eval);
				}
			}
		}
		insertHash(depth, max_eval);
		return(max_eval);
	}
	int min_eval = eval.boardScore(theBoard);;
	for (auto thePair : moves) {
		for (auto move : thePair.second) {
			if (theBoard.at(move.first, move.second).pieceType == -1 || theBoard.at(move.first, move.second).isComputerPiece == computerMove) {
				continue;
			}
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second);
			//quiescence search
			currEval = quiesce(alpha, beta, !computerMove, depth + 1);
			theBoard.undoTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second, affected);

			//adjust values
			min_eval = (min_eval < currEval) ? min_eval : currEval;
			beta = (beta < currEval) ? beta : currEval;
			if (beta <= alpha) {
				insertHash(depth, min_eval);
				return(min_eval);
			}
		}
	}
	insertHash(depth, min_eval);
	return(min_eval);

}
int GameLogic::alphabeta(int depth, int alpha, int beta, bool computerMove) {
	//see if have been in this position before and the position at depth >= previousDepth
	std::pair<bool, int> hashedEval = getHash(depth);
	if (hashedEval.first) {
		//std::cout << "GET HASHED" << std::endl;
		//std::cout << "GET HASHED" << std::endl;
		//std::cout << "GET HASHED" << std::endl;
		//std::cout << "GET HASHED" << std::endl;
		return(hashedEval.second);
	}
	//std::cout << counter << std::endl;
	//if (counter == 20996) {
	//	std::cout << "here" << std::endl;
	//}
	counter++;
	std::vector<std::pair<BoardPiece, std::vector<std::pair<int, int>>>> moves;
	if (computerMove) {
		for (auto piece : theBoard.computerPieces) {
			moves.emplace_back(piece, theBoard.getMoves(piece));
		}
	}
	else {
		for (auto piece : theBoard.playerPieces) {
			moves.emplace_back(piece, theBoard.getMoves(piece));
		}
	}
	if (inCheckMate(moves, computerMove)) {
		insertHash(depth, computerMove ? INT_MIN : INT_MAX);
		return(computerMove ? INT_MIN : INT_MAX);
	}
	if (inDraw(moves, computerMove)) {
		insertHash(depth, 0);
		return(0);
	}
	if (depth >= MAX_DEPTH) {
		int score = eval.boardScore(theBoard);
		insertHash(depth, score);
		return(score);
	}
	int currEval;
	if (computerMove) {
		int max_eval = INT_MIN;
		for (auto thePair : moves) {
			for (auto move : thePair.second) {
				std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second);
				//quiescence search
				if ((depth == MAX_DEPTH - 1) && affected.second.pieceType != -1 && affected.second.isComputerPiece != computerMove) {
					currEval = quiesce(alpha, beta, false, depth + 1);
					//eval = alphabeta(depth, alpha, beta, false);
				}
				else {
					currEval = alphabeta(depth + 1, alpha, beta, false);
				}
				theBoard.undoTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second, affected);
				
				//adjust values
				max_eval = (max_eval > currEval) ? max_eval : currEval;
				alpha = (alpha > currEval) ? alpha : currEval;
				if (beta <= alpha) {
					insertHash(depth, max_eval);
					return(max_eval);
				}
			}
		}
		insertHash(depth, max_eval);
		return(max_eval);
	}
	int min_eval = INT_MAX;
	for (auto thePair: moves) {
		for (auto move : thePair.second) {
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second);
			//quiescence search
			if ((depth == MAX_DEPTH - 1) && affected.second.pieceType != -1 && affected.second.isComputerPiece != computerMove) {
				currEval = quiesce(alpha, beta, true, depth + 1);
				//eval = alphabeta(depth, alpha, beta, true);
			}
			else {
				currEval = alphabeta(depth + 1, alpha, beta, true);
			}
			theBoard.undoTemporaryMove(thePair.first.rowCoord, thePair.first.colCoord, move.first, move.second, affected);

			//adjust values
			min_eval = (min_eval < currEval) ? min_eval : currEval;
			beta = (beta < currEval) ? beta : currEval;
			if (beta <= alpha) {
				insertHash(depth, min_eval);
				return(min_eval);
			}
		}
	}
	insertHash(depth, min_eval);
	return(min_eval);
}*/

//better version?
/*std::string GameLogic::getBestMove() {
	//default values
	int bestEval = INT_MIN + 1;
	std::string bestMove = "";
	int alpha = INT_MIN + 1;
	int beta = INT_MAX - 1;
	int currEval = INT_MIN + 1;
	//go through all the moves
	TakeContainer moves;
	std::vector<MovesContainer> moveContainer;
	for (auto piece : theBoard.computerPieces) {
		MovesContainer temp;
		temp.piece = piece;
		temp.moves = theBoard.getMoves(piece);
		moveContainer.push_back(temp);
	}
	//sort whatever, move ordering
	std::sort(moveContainer.begin(), moveContainer.end(), compareMovesContainer);
	for(auto moveCont : moveContainer){
		BoardPiece piece = moveCont.piece;
		std::cout << piece.toString() << std::endl;
		for(auto move : moveCont.moves.moves){
			//make temporary move
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(piece.rowCoord, piece.colCoord, move.first, move.second);
			displayInstance.showBoard(theBoard);
			//alpha beta
			//currEval = alphabeta(1, alpha, beta, false);
			currEval = -alphabeta(1, -beta, -alpha, false);
			theBoard.undoTemporaryMove(piece.rowCoord, piece.colCoord, move.first, move.second, affected);
			//get best move
			if (currEval >= bestEval) {
				bestEval = currEval;
				bestMove = theBoard.coordToString(piece.rowCoord, piece.colCoord, move.first, move.second);
			}
			//alpha cut off
			alpha = std::max(alpha, bestEval);
			if (alpha >= beta) {
				//break;
				std::cout << "Pruned nodes" << std::endl;
				return(bestMove); //maybe?

			}
		}
	}
	return(bestMove);
}
int GameLogic::alphabeta(int depth, int alpha, int beta, bool computerMove) {
	//std::cout << "asdsa" << std::endl;
	//see if already contained through hash
	//std::cout << counter << std::endl;
	//counter++;
	std::pair<bool, int> currHash = getHash(depth);
	//if hash found
	if (currHash.first) {
		return(currHash.second);
	}

	//terminal cases
	SinglyLinkedList relativePieces = (((computerMove) ? theBoard.computerPieces : theBoard.playerPieces));
	std::vector<MovesContainer> possibleMoves;
	for (auto thePiece : relativePieces) {
		MovesContainer temp;
		temp.piece = thePiece;
		temp.moves = theBoard.getMoves(thePiece);
		possibleMoves.push_back(temp);
	}
	int theEval = 0;
	//checkmate
	if (inCheckMate(possibleMoves, computerMove)) {
		theEval = (computerMove ? (INT_MIN + 1) : (INT_MAX - 1));
		insertHash(depth, theEval);
		return(theEval);
	}
	//draw
	if (inDraw(possibleMoves, computerMove)) {
		insertHash(depth, 0);
		return(0);
	}
	//depth
	if (depth >= MAX_DEPTH) {
		//insert into hash
		theEval = eval.boardScore(theBoard);
		insertHash(depth, theEval);
		return(theEval);
	}
	//move ordering
	std::sort(possibleMoves.begin(), possibleMoves.end(), compareMovesContainer);

	//evalutate
	int bestEval = INT_MIN + 1;
	for (auto moveContainer : possibleMoves) {
		for (auto move : moveContainer.moves.moves) {
			//make temporary move
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second);
			displayInstance.showBoard(theBoard);
			//quiese? if a piece was just taken
			if ((affected.second.pieceType != -1
				&& affected.second.isComputerPiece != affected.first.isComputerPiece)) {
				theEval = -quiesce(-beta, -alpha, !computerMove, depth + 1);
			}
			//else regular eval
			else {
				theEval = -alphabeta(depth + 1, -beta, -alpha, !computerMove);
			}
			theBoard.undoTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second, affected);

			if (theEval >= beta) {
				return(beta);
			}

			bestEval = std::max(bestEval, theEval);
			alpha = std::max(alpha, bestEval);
			if (alpha >= beta) {
				insertHash(depth, bestEval);
				return(bestEval);
			}

		}
	}
	insertHash(depth, bestEval);
	return(bestEval);
}
int GameLogic::quiesce(int alpha, int beta, bool computerMove, int depth) {
	//std::cout << counter << std::endl;
	//counter++;
	//see if already contained through hash
	std::pair<bool, int> currHash = getHash(depth);
	//if hash found
	if (currHash.first) {
		return(currHash.second);
	}

	//terminal cases
	SinglyLinkedList relativePieces = (((computerMove) ? theBoard.computerPieces : theBoard.playerPieces));
	std::vector<MovesContainer> possibleMoves;
	for (auto thePiece : relativePieces) {
		MovesContainer temp;
		temp.piece = thePiece;
		temp.moves = theBoard.getMoves(thePiece);
		possibleMoves.push_back(temp);
	}
	int theEval;
	//checkmate
	if (inCheckMate(possibleMoves, computerMove)) {
		theEval = (computerMove ? (INT_MIN + 1): (INT_MAX - 1));
		insertHash(depth, theEval);
		return(theEval);
	}
	//draw
	if (inDraw(possibleMoves, computerMove)) {
		insertHash(depth, 0);
		return(0);
	}
	//depth
	int standPat = eval.boardScore(theBoard);
	if (depth >= (MAX_DEPTH + MAX_QDEPTH)) {
		//insert into 
		theEval = standPat;
		insertHash(depth, theEval);
		return(theEval);
	}
	//if (standPat >= beta) {
	//	return(beta);
	//}
	if (alpha < standPat) {
		alpha = standPat;
	}

	//move ordering
	int bestEval;
	std::sort(possibleMoves.begin(), possibleMoves.end(), compareMovesContainer);
	int currEval;
	for (auto moveContainer : possibleMoves) {
		for (auto move : moveContainer.moves.moves) {
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second);
			//see if the move is a taking move
			if (affected.second.pieceType == -1 || affected.second.isComputerPiece == computerMove) {
				theBoard.undoTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second, affected);
				continue;
			}
			displayInstance.showBoard(theBoard);
			currEval = -quiesce(-beta, -alpha, !computerMove, depth + 1);
			theBoard.undoTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second, affected);
			if (currEval >= beta) {
				return(beta);
			}
			bestEval = std::max(bestEval, theEval);
			alpha = std::max(alpha, bestEval);
			if (alpha >= beta) {
				insertHash(depth, bestEval);
				return(bestEval);
			}
		}
	}
	insertHash(depth, bestEval);
	return(bestEval);
}*/

//better original
TerminalNode GameLogic::getTerminalNode(std::vector<MovesContainer> themMoves) {
	TerminalNode temp;
	temp.is = false;
	bool computerMove = themMoves.at(0).piece.isComputerPiece;
	if (inCheckMate(themMoves, computerMove)) {
		temp.is = true;
		temp.eval = (computerMove ? (INT_MIN + 1) : (INT_MAX - 1));
	}
	else if (inDraw(themMoves, computerMove)) {
		temp.is = true;
		temp.eval = 0;
	}
	return(temp);
}

std::string GameLogic::getBestMove() {
	std::string bestMove = "";
	//terminal node
	std::vector<MovesContainer> moveContainer;
	for (auto piece : theBoard.computerPieces) {
		MovesContainer temp;
		temp.piece = piece;
		temp.moves = theBoard.getMoves(piece);
		moveContainer.push_back(temp);
	}
	TerminalNode temNode = getTerminalNode(moveContainer);
	if (temNode.is) {
		return("end");
	}
	std::sort(moveContainer.begin(), moveContainer.end(), compareMovesContainer);
	int best_eval = INT_MIN + 1;
	int alpha = INT_MIN + 1;
	int beta = INT_MAX - 1;
	for (auto theMoves : moveContainer) {
		BoardPiece piece = theMoves.piece;
		std::cout << piece.toString() << std::endl;
		for (auto move : theMoves.moves.moves) {
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(piece.rowCoord, piece.colCoord, move.first, move.second);
			displayInstance.showBoard(theBoard);
			int eval = alphabeta(alpha, beta, false, 1);
			theBoard.undoTemporaryMove(piece.rowCoord, piece.colCoord, move.first, move.second, affected);

			if (eval >= best_eval) {
				best_eval = eval;
				bestMove = theBoard.coordToString(piece.rowCoord, piece.colCoord, move.first, move.second);
			}
			alpha = std::max(alpha, best_eval);
			if (alpha >= beta) {
				return(bestMove);
			}
		}
	}
	return(bestMove);
}
int GameLogic::alphabeta(int alpha, int beta, bool computerMove, int depth) {
	std::cout << counter << std::endl;
	counter++;
	std::pair<bool, int> currHash = getHash(depth);
	//if hash found
	if (currHash.first) {
		return(currHash.second);
	}
	std::vector<MovesContainer> possibleMoves;
	auto pieces = (computerMove ? theBoard.computerPieces : theBoard.playerPieces);
	for (auto piece : pieces) {
		MovesContainer temp;
		temp.piece = piece;
		temp.moves = theBoard.getMoves(piece);
		possibleMoves.push_back(temp);
	}
	TerminalNode temNode = getTerminalNode(possibleMoves);
	if (temNode.is) {
		return(temNode.eval);
	}
	int theEval = eval.boardScore(theBoard, possibleMoves);
	if (depth >= MAX_DEPTH) {
		insertHash(depth, theEval);
		return(theEval);
	}
	std::sort(possibleMoves.begin(), possibleMoves.end(), compareMovesContainer);
	int currEval;
	int bestEval = (computerMove ? (INT_MIN + 1) : (INT_MAX - 1));
	for (auto moveContainer : possibleMoves) {
		for (auto move : moveContainer.moves.moves) {
			if (counter == 2) {
				std::cout << "asdas" << std::endl;
			}
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second);
			displayInstance.showBoard(theBoard);

			if (affected.second.pieceType != -1
				&& affected.second.isComputerPiece != affected.first.isComputerPiece
				&& depth == MAX_DEPTH - 1) {
				currEval = quiesce(alpha, beta, !computerMove, depth + 1);
			}
			else {
				currEval = alphabeta(alpha, beta, !computerMove, depth + 1);
			}
			theBoard.undoTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second, affected);
			if (computerMove) {
				bestEval = std::max(bestEval, theEval);
				alpha = std::max(alpha, bestEval);
			}
			else {
				bestEval = std::min(bestEval, currEval);
				beta = std::min(beta, bestEval);
			}
			if (alpha > beta) {
				insertHash(depth, bestEval);
				return(bestEval);
			}
		}
	}
	insertHash(depth, bestEval);
	return(bestEval);
}
int GameLogic::quiesce(int alpha, int beta, bool computerMove, int depth) {
	std::pair<bool, int> currHash = getHash(depth);
	//if hash found
	if (currHash.first) {
		return(currHash.second);
	}
	std::vector<MovesContainer> possibleMoves;
	auto pieces = (computerMove ? theBoard.computerPieces : theBoard.playerPieces);
	for (auto piece : pieces) {
		MovesContainer temp;
		temp.piece = piece;
		temp.moves = theBoard.getMoves(piece);
		possibleMoves.push_back(temp);
	}
	TerminalNode temNode = getTerminalNode(possibleMoves);
	if (temNode.is) {
		return(temNode.eval);
	}
	int theEval = eval.boardScore(theBoard, possibleMoves);
	if (depth >= (MAX_DEPTH + MAX_QDEPTH)) {
		insertHash(depth, theEval);
		return(theEval);
	}
	std::sort(possibleMoves.begin(), possibleMoves.end(), compareMovesContainer);
	int currEval;
	int bestEval = (computerMove ? (INT_MIN + 1) : (INT_MAX - 1));
	for (auto moveContainer : possibleMoves) {
		for (auto move : moveContainer.moves.moves) {
			//evaluate onyl taking moves
			if (theBoard.at(move.first, move.second).isComputerPiece == moveContainer.piece.isComputerPiece) {
				continue;
			}
			std::pair<BoardPiece, BoardPiece> affected = theBoard.makeTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second);
			displayInstance.showBoard(theBoard);
			currEval = quiesce(alpha, beta, !computerMove, depth + 1);
			theBoard.undoTemporaryMove(moveContainer.piece.rowCoord, moveContainer.piece.colCoord, move.first, move.second, affected);
			if (computerMove) {
				bestEval = std::max(bestEval, theEval);
				alpha = std::max(alpha, bestEval);
			}
			else {
				bestEval = std::min(bestEval, currEval);
				beta = std::min(beta, bestEval);
			}
			if (alpha > beta) {
				insertHash(depth, bestEval);
				return(bestEval);
			}
		}
	}
	insertHash(depth, bestEval);
	return(bestEval);
}

bool GameLogic::compareMovesContainer(const MovesContainer& a, const MovesContainer& b) {
	return(a.moves.takes > b.moves.takes);
}

bool GameLogic::inCheckMate(std::vector<MovesContainer> possibleMoves, bool computer) {
	//iterate through each piece, if a piece can move that means not in checkmate
	if (!theBoard.inCheck(computer)) {
		return(false);
	}
	for (auto moveContainer : possibleMoves) {
		if (moveContainer.moves.moves.size() > 0) {
			return(false);
		}
	}
	return(true);
}
bool GameLogic::inDraw(std::vector<MovesContainer> possibleMoves, bool computer) {
	//iterate through each piece, if a piece can move that means not in checkmate
	if (theBoard.inCheck(computer)) {
		return(false);
	}
	for (auto moveContainer : possibleMoves) {
		if (moveContainer.moves.moves.size() > 0) {
			return(false);
		}
	}
	return(true);
}