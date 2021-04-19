#include "Board.h"
#include <iostream>
#include <string>
#include <array>
#include <chrono>
using std::string;
using std::cout;
using std::endl;
using std::vector;

Board::Board() : whiteTurn(true) {
	board = new Piece * [64];

	for (int i = 0; i < 64; i++) {
		board[i] = NULL;
	}
	PreComputeMoveData();
}

Board::~Board() {
	for (int i = 0; i < 64; i++) {
		delete board[i];
	}
	delete board;

	for (int i = 0; i < 64; i++) {
		delete numToTheEdge[i];
	}
	delete numToTheEdge;
}


void Board::printBoard() {
	cout << endl;
	int pos = 0;
	while (pos < 64) {
		for (int i = 0; i < 8; i++) {
			//cout << pos << endl;
			if (board[pos] == NULL) {
				//empty square, print a _
				cout << " _";
			}
			else {
				cout << " " << board[pos]->pieceName;
			}
			pos++;
		}
		//end the line
		cout << endl;
	}
	
	cout << endl;
}

bool Board::setPositionFEN(std::string FENSTring) {
	unsigned int posOnBoard = 0;
	Piece** newBoard = new Piece* [64];
	for (int i = 0; i < 64; i++) {
		delete board[i];
	}
	delete board;

	for (unsigned int i = 0; i < FENSTring.length(); i++) {
		unsigned char current = FENSTring.at(i);
		if (current == '/') {
			//skip to next rank
			while (posOnBoard % 8 != 0) {
				newBoard[posOnBoard] = NULL;
				posOnBoard++;
			}  
		}
		else if (isdigit(current)) {
			int amountToSkip = current - 48;
			for (int i = 0; i < amountToSkip; i++) {
				newBoard[posOnBoard] = NULL;
				posOnBoard++;
			}
		}
		else {
			int colour = Piece::Black;
			if (isupper(current)) {
				current = tolower(current);
				colour = Piece::White;
			}
			int pieceType = 0;
			//find what piece it is
			switch (current) {
				case 'k':
					//king
					pieceType = Piece::King;
					break;
				case 'p':
					//pawn
					pieceType = Piece::Pawn;
					break;
				case 'n':
					//knight
					pieceType = Piece::Knight;
					break;
				case 'b':
					//bishop
					pieceType = Piece::Bishop;
					break;
				case 'r':
					//rook
					pieceType = Piece::Rook;
					break;
				case 'q':
					//queen
					pieceType = Piece::Queen;
					break;
			}
			newBoard[posOnBoard] = new Piece(pieceType | colour);
			//cout << newBoard[posOnBoard]->getPieceName() << posOnBoard << "Hello?" << endl;
			posOnBoard++;
		}
	}

	board = newBoard;
	return true;
}

bool Board::ManualMove(int from, int to) {
	//returns false if it is an illegal move
	vector<Move> moves;
	moves = GenerateLegalMoves();
	for (Move cmove : moves) {
		if (cmove.from == from && cmove.to == to) {
			MakeMove(cmove);
			return true;
		}
	}
	//was not in the legal moves, therfore illegal
	return false;
}

void Board::MakeMove(Move move) {
	moveHistory.push_back(MoveInHistory(move.from, move.to, board[move.to]));
	board[move.to] = board[move.from];
	board[move.from] = NULL; 
	whiteTurn = !whiteTurn;
}

void Board::UndoMove() {
	MoveInHistory move = moveHistory.back();
	board[move.from] = board[move.to];
	board[move.to] = move.pOnTo;
	moveHistory.pop_back();
	whiteTurn = !whiteTurn;
}

vector<Move> Board::GenerateLegalMoves() {
	vector<Move> allMoves;
	allMoves = generateAllMoves();
	filterMoves(allMoves);
	/*
	for (Move cmove : legalMoves) {
		cout << cmove.from << " to " << cmove.to << endl;
	}
	*/
	//cout << "number of legal moves: " << allMoves.size() << endl;

	return allMoves;
}

vector<Move> Board::generateAllMoves() {
	//cout << "Starting generation" << endl;
	vector<Move> moves;
	for (int fpos = 0; fpos < 64; fpos++) {
		//if you are on a piece that is also the colour that it is to move
		if (board[fpos] != NULL && (board[fpos]->isWhite == whiteTurn)) {
			//if it is a sliding piece, the array that it will read off of will not b emtpy
			int pieceNum = board[fpos]->identity % 8;
			if (pieceNum >= 4) {
				//sliding piece
				generateSlidingMoves(moves, fpos);
			}
			else if (pieceNum == Piece::King) {
				generateKingMoves(moves, fpos);
			}
			else if (pieceNum == Piece::Knight) {
				generateKnightMoves(moves, fpos);
			}
			else if (pieceNum == Piece::Pawn) {
				if (board[fpos]->isWhite) {
					generateWhitePawnMoves(moves, fpos);
				}
				else {
					generateBlackPawnMoves(moves, fpos);
				}
			}
			
			
		}
	}
	//cout << "finished genration" << endl;
	
	return moves;
}



void Board::generateSlidingMoves(vector<Move>& legalMoves, int fpos) {
	int pieceDirectionsi = board[fpos]->identity % 8 - 4;
	bool colour = board[fpos]->isWhite;
	for (int i = 0; i < sizeof(Piece::HowPiecesCanMove[pieceDirectionsi]) / sizeof(Piece::HowPiecesCanMove[pieceDirectionsi][0]); i++) {
		int direction = Piece::HowPiecesCanMove[pieceDirectionsi][i];
		if (direction != 10) {
			//go in the direction until the edge
			int pos = fpos;
			for (int distance = numToTheEdge[fpos][direction]; distance > 0; distance--) {
				//slide until you hit self or enemy
				pos += directionOffsets[direction];

				//if you are on a friendly piece
				if (board[pos] != NULL && (board[pos]->isWhite == colour)) {
					//on friendly, impossible move
					break;
				}

				//legal move, add to moves
				legalMoves.push_back(Move(fpos, pos));

				//see if you are on a opponents pice, if so this move is legal but you cant go any farther
				if (board[pos] != NULL && (board[pos]->isWhite != colour)) {
					//on opponent, dont go farther
					break;
				}
			}
		}
	}
}

void Board::generateKingMoves(vector<Move>& legalMoves, int fpos) {
	for (int direction = 0; direction < 8; direction++) {
		//if there is spae in that direction on the board
		if (numToTheEdge[fpos][direction] >= 1) {
			//if its empty or its an opponent
			int npos = fpos + directionOffsets[direction];
			if (board[npos] == NULL || board[fpos]->isWhite != board[npos]->isWhite) {
				//legal move
				legalMoves.push_back(Move(fpos, npos));
			}
		}
	}
}

void Board::generateKnightMoves(std::vector<Move>& legalMoves, int fpos) {
	for (int direction = 0; direction < 8; direction++) {
		//if that square is possible
		if (numToTheEdge[fpos][knightNumToTheEdgeDirections[direction][0]] >= 2 && numToTheEdge[fpos][knightNumToTheEdgeDirections[direction][1]] >= 1) {
			//possible square, see if it is empty or an opponent
			int npos = fpos + knightOffsets[direction];
			if (board[npos] == NULL || board[fpos]->isWhite != board[npos]->isWhite) {
				legalMoves.push_back(Move(fpos, npos));
			}
		}
	}
}

void Board::generateWhitePawnMoves(std::vector<Move>& legalMoves, int fpos) {
	//single
	int npos = fpos + directionOffsets[1];
	if (numToTheEdge[fpos][1] >= 1 && board[npos] == NULL) {
		legalMoves.push_back(Move(fpos, npos));
		//see if you can go twice
		npos += directionOffsets[1];
		bool onStartingRank = numToTheEdge[fpos][6] == 1;
		if (onStartingRank && board[npos] == NULL) {
			legalMoves.push_back(Move(fpos, npos));
		}
	}
	npos = fpos + directionOffsets[0];
	//diagonal captue moves
	if (numToTheEdge[fpos][0] >= 1 && board[npos] != NULL && !board[npos]->isWhite) {
		legalMoves.push_back(Move(fpos, npos));
	}
	npos = fpos + directionOffsets[2];
	if (numToTheEdge[fpos][2] >= 1 && board[npos] != NULL && !board[npos]->isWhite) {
		legalMoves.push_back(Move(fpos, npos));
	}
}

void Board::generateBlackPawnMoves(std::vector<Move>& legalMoves, int fpos) {
	//single
	int npos = fpos + directionOffsets[6];
	if (numToTheEdge[fpos][6] >= 1 && board[npos] == NULL) {
		legalMoves.push_back(Move(fpos, npos));
		//see if you can go twice
		npos += directionOffsets[6];
		bool onStartingRank = numToTheEdge[fpos][1] == 1;
		if (onStartingRank && board[npos] == NULL) {
			legalMoves.push_back(Move(fpos, npos));
		}
	}
	npos = fpos + directionOffsets[5];
	//diagonal captue moves
	if (numToTheEdge[fpos][5] >= 1 && board[npos] != NULL && board[npos]->isWhite) {
		legalMoves.push_back(Move(fpos, npos));
	}
	npos = fpos + directionOffsets[7];
	if (numToTheEdge[fpos][7] >= 1 && board[npos] != NULL && board[npos]->isWhite) {
		legalMoves.push_back(Move(fpos, npos));
	}
}

void Board::filterMoves(std::vector<Move>& moves) {
	//make each move, generate the legal moves, then filter illegal moves
	//make move
	auto i = moves.begin();
	while (i != moves.end()) {
		MakeMove(*i);
		
		//see if it would be an illegal move
		bool valid = !kingInCheck(!whiteTurn);
		
		//unmake move
		UndoMove();

		if (valid) {
			++i;
		}
		else {
			i = moves.erase(i);
		}
		
	}
}

bool Board::kingInCheck(bool white) {
	//find king
	int kingpos = 0;
	while (kingpos < 64) {
		if (board[kingpos] != NULL && board[kingpos]->identity % 8 == Piece::King && board[kingpos]->isWhite == white) {
			break;
		}
		kingpos++;
	}


	//if in check by a knight
	for (int direction = 0; direction < 8; direction++) {
		//if that square is possible
		if (numToTheEdge[kingpos][knightNumToTheEdgeDirections[direction][0]] >= 2 && numToTheEdge[kingpos][knightNumToTheEdgeDirections[direction][1]] >= 1) {
			//possible square, see if it is empty or an opponent
			int npos = kingpos + knightOffsets[direction];
			if (board[npos] != NULL && white != board[npos]->isWhite && board[npos]->identity % 8 == Piece::Knight) {
				//in check by knight
				return true;
			}
		}
	}
	int npos;
	//pawn check
	if (white) {
		//up left
		npos = kingpos + directionOffsets[0];
		if (board[npos] != NULL && board[npos]->identity == Piece::Pawn + Piece::Black) {
			return true;
		}
		//up right
		npos = kingpos + directionOffsets[2];
		if (board[npos] != NULL && board[npos]->identity == Piece::Pawn + Piece::Black) {
			return true;
		}
	}
	else {
		//down left
		npos = kingpos + directionOffsets[5];
		if (board[npos] != NULL && board[npos]->identity == Piece::Pawn + Piece::White) {
			return true;
		}
		//down right
		npos = kingpos + directionOffsets[7];
		if (board[npos] != NULL && board[npos]->identity == Piece::Pawn + Piece::White) {
			return true;
		}
	}

	//check for sliding pieces
	for (int direction = 0; direction < 8; direction++) {
		//go in the direction until the edge
		int pos = kingpos;
		for (int distance = numToTheEdge[kingpos][direction]; distance > 0; distance--) {
			//slide until you hit self or enemy
			pos += directionOffsets[direction];

			//if you are on a friendly piece
			if (board[pos] != NULL && (board[pos]->isWhite == white)) {
				//on friendly, not in check from here
				break;
			}

			//see if you are on a opponents pice, if so this move is legal but you cant go any farther
			if (board[pos] != NULL && (board[pos]->isWhite != white)) {
				//on opponent, see if it sees you
				//if it is a queen 
				if (board[pos]->identity % 8 == Piece::Queen) {
					return true;
				}
				//check other types
				if (direction == 0 || direction == 2 || direction == 5 || direction == 7) {
					//on a diagonal move, if bishop - in check
					if (board[pos]->identity % 8 == Piece::Bishop) {
						return true;
					}
				}
				else {
					//on a straight move, if rook - in check
					if (board[pos]->identity % 8 == Piece::Rook) {
						return true;
					}
				}

				break;
			}
		}
	}


	return false;
}

int Board::Evaluate(bool captures) {
	//go until there are no captures remaining
	int movesMade = 0;
	if (captures) {
		movesMade = moveUntilNoCaptures();
	}
	int white = 0;
	int black = 0;
	for (int pos = 0; pos < 64; pos++) {
		if (board[pos] != NULL) {
			//add into eval
			if (board[pos]->isWhite) {
				white += board[pos]->value * 100 + whitePostionBonuses[board[pos]->identity % 8 - 1][pos];
			}
			else {
				black += board[pos]->value * 100 + blackPositionBonuses[board[pos]->identity % 8 - 1][pos];
			}
		}
	}
	//eval has been made, return back to the position that was given
	for (int i = 0; i < movesMade; i++) {
		UndoMove();
	}
	//printBoard();

	if (!captures) {
		return white - black;
	}

	//this is the first call
	//higher is better ofr white, so give the one that is better for the player whos turn it is
	int nocaptures = Evaluate(false);
	int thisEval = white - black;
	if (whiteTurn) {
		return thisEval > nocaptures ? thisEval : nocaptures;
	}
	else {
		return thisEval > nocaptures ? nocaptures : thisEval;
	}

	//cout << "this pos is at " << white - black << endl;
	//cout << "white is " << white << endl;
	//cout << "black is " << black << endl;
	//cout << "had made " << movesMade << " moves to do all captures before evaluating materially" << endl;
}

int Board::moveUntilNoCaptures() {
	int movesMade = 0;
	bool capturing = true;
	while (capturing) {
		capturing = false;
		vector<Move> moves = GenerateLegalMoves();
		for (Move cmove : moves) {
			if (board[cmove.to] != NULL) {
				MakeMove(cmove);
				movesMade++;
				capturing = true;
				break;
			}
		}
		//no captures possible in this postion on this turn
	}
	return movesMade;
}

Move Board::findBestMove(int depth) {
	auto start = std::chrono::high_resolution_clock::now();
	postionsEvaluated = 0;
	vector<Move> moves = GenerateLegalMoves();
	std::size_t bestMovei = 0;
	int bestMoveEval = -10000000;
	//search for the best eval for this move
	int movesLookedAt = 1;
	for (std::size_t i = 0; i < moves.size(); ++i) {
		//for each move, find the best move
		MakeMove(moves[i]);
		int thisEval = findBestMoveEval(depth - 1, -100000, 100000) * -1;
		
		cout << "done " << movesLookedAt << " moves" << endl;
		if (thisEval > bestMoveEval) {
			bestMoveEval = thisEval;
			bestMovei = i;
		}

		
		UndoMove();
		movesLookedAt++;
	}
	cout << "searched " << postionsEvaluated << " postions" << endl;
	
	auto end = std::chrono::high_resolution_clock::now();
	auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	cout << "what" << endl;
	cout << "took " << timeTaken.count() << " milliseconds to find the best move" << endl;
	return moves[bestMovei];
}

int Board::findBestMoveEval(int depth, int alpha, int beta) {
	if (depth == 0) {
		//base case
		postionsEvaluated++;
		int eval = Evaluate();
		if (whiteTurn){
			return eval;
		}
		return eval * -1;
	}


	vector<Move> moves = GenerateLegalMoves();
	//check to se if you are mated
	if (moves.empty()) {
		if (kingInCheck(whiteTurn)) {
			return -100000000;
		}
		return 0;
	}

	int bestMoveEval = -1000000000;
	//search for the best eval for this move
	for (std::size_t i = 0; i < moves.size(); ++i) {
		//for each move, find the best move
		MakeMove(moves[i]);
		int thisEval = findBestMoveEval(depth - 1, -beta, -alpha) * -1;

		if (thisEval > bestMoveEval) {
			bestMoveEval = thisEval;
			alpha = thisEval;
		}


		UndoMove();
		if (thisEval >= beta) {
			return beta;//move was too good opponent will avoid it
		}
	}

	return bestMoveEval;
}


int Board::convert(string str) {
	int file = str.at(0) - 'a';
	int rank = str.at(1) - '0';
	return file + (8 - rank) * 8;
}
