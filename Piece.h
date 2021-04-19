#pragma once
class Piece
{
	
public:
	static const int King = 1;
	static const int Pawn = 2;
	static const int Knight = 3;
	static const int Bishop = 4;
	static const int Rook = 5;
	static  const int Queen = 6;

	static const int White = 8;
	static const int Black = 16;
	
	static const int HowPiecesCanMove[3][8];
	
	Piece(int initidentity);
	int identity;
	int value;
	bool isWhite;
	char pieceName;
	
	
	
	
private:
	bool setPieceName();
	bool setValue();

};

