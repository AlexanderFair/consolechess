#pragma once
#include "Piece.h"
#include "Move.h"
#include "MoveInHistory.h"
#include <string>
#include <vector>
class Board
{
public:
	Board();
	~Board();
	Piece** board;
	bool whiteTurn;
	static int convert(std::string str);
	std::vector<MoveInHistory> moveHistory;

	void printBoard();
	bool setPositionFEN(std::string FENString);
	bool ManualMove(int from, int to);
	void MakeMove(Move move);
	void UndoMove();

	void generateSlidingMoves(std::vector<Move>& legalMoves, int fpos);
	void generateKingMoves(std::vector<Move>& legalMoves, int fpos);
	void generateKnightMoves(std::vector<Move>& legalMoves, int fpos);
	void generateWhitePawnMoves(std::vector<Move>& legalMoves, int fpos);
	void generateBlackPawnMoves(std::vector<Move>& legalMoves, int fpos);
	void filterMoves(std::vector<Move>& moves);
	bool kingInCheck(bool white);

	std::vector<Move> GenerateLegalMoves();
	std::vector<Move> generateAllMoves();

	int Evaluate(bool captures = true);
	int moveUntilNoCaptures();

	Move findBestMove(int depth);
	int findBestMoveEval(int depth, int alpha, int beta);

private:

	long postionsEvaluated = 0;

	int directionOffsets[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };
	int knightOffsets[8] = { -10, -17, -15, -6, 10, 17, 15, 6 };
	int knightNumToTheEdgeDirections[8][2] = { {3, 1}, {1, 3}, {1, 4}, {4, 1}, {4, 6}, {6, 4}, {6, 3}, {3, 6} };
	int** numToTheEdge = PreComputeMoveData();

	int** PreComputeMoveData() {
		int** data = new int* [64];
		int xychanges[8][2] = { {-1, -1},
			{0, -1},
			{1, -1},
			{-1, 0},
			{1, 0},
			{-1, 1},
			{0, 1},
			{1, 1} };
		for (int square = 0; square < 64; square++) {
			data[square] = new int[8];
			int file = square % 8;
			int rank = (square - (square % 8)) / 8;
			for (int direction = 0; direction < 8; direction++) {
				//find how far from the edge it is
				//the change of x for this direction
				int x = xychanges[direction][0];
				int y = xychanges[direction][1];
				int filecopy = file;
				int rankcopy = rank;
				int distToEdge = -1;
				while (filecopy >= 0 && filecopy < 8 && rankcopy >= 0 && rankcopy < 8) {
					filecopy += x;
					rankcopy += y;
					distToEdge++;
				}
				data[square][direction] = distToEdge;
			}
		}
		return data;
	}


	//tables for placement bonuses of pieces
	int whitePostionBonuses[6][64] = {
		//king
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 4, 3, 2, 1, 1, 2, 3, 4,
		},

		//pawn
		{0, 0, 0, 0, 0, 0, 0, 0,
		 5, 5, 5, 6, 6, 5, 5, 5,
		 4, 4, 4, 5, 5, 4, 4, 4,
		 3, 3, 3, 4, 4, 3, 3, 3,
		 2, 2, 2, 3, 3, 2, 2, 2,
		 1, 1, 1, 2, 2, 1, 1, 1,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//knight
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 1, 3, 3, 3, 3, 1, 0,
		 0, 1, 3, 5, 5, 3, 1, 0,
		 0, 1, 2, 5, 5, 2, 1, 0,
		 0, 1, 2, 2, 2, 2, 1, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//bishop
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 2, 1, 1, 1, 1, 2, 0,
		 0, 2, 4, 3, 3, 4, 2, 0,
		 0, 2, 3, 5, 5, 3, 2, 0,
		 0, 2, 2, 5, 5, 2, 2, 0,
		 0, 1, 3, 2, 2, 3, 2, 0,
		 0, 2, 1, 1, 1, 1, 2, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//rook
		{0, 0, 0, 0, 0, 0, 0, 0,
		 2, 2, 2, 2, 2, 2, 2, 2,
		 1, 1, 1, 1, 1, 1, 1, 1,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 1, 2, 2, 1, 0, 0,
		},
		//queen
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 1, 3, 3, 3, 3, 1, 0,
		 0, 1, 3, 5, 5, 3, 1, 0,
		 0, 1, 2, 5, 5, 2, 1, 0,
		 0, 1, 2, 2, 2, 2, 1, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		}
	};

	int blackPositionBonuses[6][64] = {
		//king
		{4, 3, 2, 1, 1, 2, 3, 4,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//pawn
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 1, 1, 1, 2, 2, 1, 1, 1,
		 2, 2, 2, 3, 3, 2, 2, 2,
		 3, 3, 3, 4, 4, 3, 3, 3,
		 4, 4, 4, 5, 5, 4, 4, 4,
		 5, 5, 5, 6, 6, 5, 5, 5,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//knight
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 1, 2, 2, 2, 2, 1, 0,
		 0, 1, 2, 5, 5, 2, 1, 0,
		 0, 1, 3, 5, 5, 3, 1, 0,
		 0, 1, 3, 3, 3, 3, 1, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//bishop
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 2, 1, 1, 1, 1, 2, 0,
		 0, 1, 3, 3, 3, 2, 2, 0,
		 0, 2, 2, 5, 5, 2, 2, 0,
		 0, 2, 3, 5, 5, 3, 2, 0,
		 0, 2, 4, 2, 2, 4, 2, 0,
		 0, 2, 1, 1, 1, 1, 2, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//rook
		{0, 0, 1, 2, 2, 1, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 1, 1, 1, 1, 1, 1, 1, 1,
		 2, 2, 2, 2, 2, 2, 2, 2,
		 0, 0, 0, 0, 0, 0, 0, 0,
		},
		//queen
		{0, 0, 0, 0, 0, 0, 0, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 1, 2, 2, 2, 2, 1, 0,
		 0, 1, 2, 5, 5, 2, 1, 0,
		 0, 1, 3, 5, 5, 3, 1, 0,
		 0, 1, 3, 3, 3, 3, 1, 0,
		 0, 1, 1, 1, 1, 1, 1, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		}
	};
};

