#include "App.h"
#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::cin;
using std::endl;
int main() {

	//init board
	Board* board = new Board();
	board->setPositionFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	string command = "";
	cout << "Enter command, hh for you make everymove, hb for you play and then the bot replies, and bb for the bot to play itself" << endl;
	cin >> command;
	if (command == "hb"){
		cout << "Commands are depth, undo, and move. Depth is default to 4." << endl;
		cout << "When moving, type it without spaces, for example movee2e4 is move from e2 to e4" << endl;
		cout << "For other commands, do not include any other text other than 'depth' or 'undo'" << endl;
		int from = 0;
		int to = 0;
		int depth = 4;
		
		while (from != 100) {
			if (board->GenerateLegalMoves().size() == 0) {
				cout << "You have been checkmated!" << endl;
				while (true){}
			}
			cout << "Enter command, no spaces" << endl;
			
			cin >> command;
			string copy = command;
			if (command.substr(0, 4) == "move") {
				string sfrom = command.substr(4, 2);
				string sto = command.substr(6, 2);
				
				cout << "making move" << endl;
				if (!board->ManualMove(Board::convert(sfrom), Board::convert(sto))) {
					cout << "Illegal move, try again" << endl;
					cout << "You have " << board->GenerateLegalMoves().size() << " to choose from" << endl;
				}
				else {
					if (board->GenerateLegalMoves().size() == 0) {
						cout << "Checkmated, congrats" << endl;
						break;
					}
					else {
						board->MakeMove(board->findBestMove(depth));
					}
				}
				
				board->printBoard();
				cout << "This postion, materially, is valued at " << board->Evaluate() << endl;
			}
			else if (command.substr(0, 5) == "depth") {
				cout << "Please enter the depth" << endl;
				cin >> depth;
			}
			else if (command == "undo") {
				board->UndoMove();
				board->UndoMove();
			}
			
			board->printBoard();
			cout << "This postion, materially, is valued at " << board->Evaluate() << endl;
		}
	}
	else if (command == "bb") {
		int movesDone = 0;
		int movesToDo = 0;
		cout << "depth please" << endl;
		int depth = 0;
		cin >> depth;
		cout << "moves to do please" << endl;
		cin >> movesToDo;
		while (movesDone < movesToDo) {
			if (board->GenerateLegalMoves().size() == 0) {
				cout << "checkmate!" << endl;
				break;
			}
			else {
				board->MakeMove(board->findBestMove(depth));
				board->printBoard();
				movesDone++;
			}
		}
	}
	else if (command == "hh") {
		string command;
		while (command != "exit") {
			cout << "Enter move, in format fromto - example: e2e4 for e2 to e4" << endl;
			cin >> command;
			if (!board->ManualMove(Board::convert(command.substr(0,2)), Board::convert(command.substr(2,2)))) {
				cout << "Illegal move, try again" << endl;
				cout << "You have " << board->GenerateLegalMoves().size() << " to choose from" << endl;
			}


			board->printBoard();
			cout << "This postion, materially, is valued at " << board->Evaluate() << endl;
		}
	}
	else {
		cout << "bad command, you have to exit now :|" << endl;
	}


	while (true) {

	}
	board->~Board();

	delete board;
	return 0;
}
