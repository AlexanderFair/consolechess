#include "Piece.h"
#include <string>
#include <iostream>
//10 means ignore
const int Piece::HowPiecesCanMove[3][8] = { {0, 2, 5, 7,10,10,10,10}, {1, 3, 4, 6, 10, 10, 10, 10}, {0,1,2,3,4,5,6,7} };

Piece::Piece(int initidentity) : identity(initidentity) {
	setPieceName();
	isWhite = isupper(pieceName);
	setValue();
}

bool Piece::setPieceName() {
	int copy = identity;
	bool isWhite = true;
	char name = 'k';
	//find the colour
	if (copy > 16) {
		copy -= 16;
		//piece is black
		isWhite = false;
	}
	else {
		copy -= 8;
	}

	//find what piece it is
	switch (copy) {
	case 1:
		//king
		name = 'k';
		break;
	case 2:
		//pawn
		name = 'p';
		break;
	case 3:
		//knight
		name = 'n';
		break;
	case 4:
		//bishop
		name = 'b';
		break;
	case 5:
		//rook
		name = 'r';
		break;
	case 6:
		//queen
		name = 'q';
		break;
	}

	//apply colour
	if (isWhite) {
		name = name - 32;
	}

	pieceName = name;

	return true;
}

bool Piece::setValue() {
	switch (identity % 8) {
	case Piece::King:
		value = 0;
		break;
	case Piece::Pawn:
		value = 1;
		break;
	case Piece::Knight:
		value = 3;
		break;
	case Piece::Bishop:
		value = 3;
		break;
	case Piece::Rook:
		value = 5;
		break;
	case Piece::Queen:
		value = 9;
		break;
	default:
		value = 0;
	}

	return true;
}