#pragma once
class Move
{
public:
	Move(int ifrom, int ito);
	~Move();
	int to;
	int from;
};

