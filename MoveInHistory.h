#pragma once
#include "Piece.h"
class MoveInHistory
{
public:
	MoveInHistory(int ifrom, int ito, Piece* pOnTo);
	~MoveInHistory();
	int to;
	int from;
	Piece* pOnTo;
};

