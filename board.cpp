#include "board.h"

#include <iostream>

bool iskingincheck(bool white);

using namespace std;


char board[8][8];
bool whitetomove = true;

bool whiteKingsideCastle  = true;
bool whiteQueensideCastle = true;
bool blackKingsideCastle  = true;
bool blackQueensideCastle = true;

int enPassantRow = -1;
int enPassantCol = -1;

void initialiseboard()
{
    for(int i=0;i<8;i++)
    {
        for(int j= 0;j<8;j++ )
        {
            board[i][j] =  '.';
        }
       
    }
    //pawns
    for(int i=0;i<8;i++)
    {
        board[1][i] = 'p';
        board[6][i] = 'P';
    }
    board[0][0] = 'r';
    board[0][7] = 'r';
    board[7][0] = 'R';
    board[7][7] = 'R';

    board[0][1] = 'n';
    board[0][6] = 'n';
    board[7][1] = 'N';
    board[7][6] = 'N';

    board[0][2] = 'b';
    board[0][5] = 'b';
    board[7][2] = 'B';
    board[7][5] = 'B';

    board[0][3] = 'q';
    board[0][4] = 'k';
    board[7][3] = 'Q';
    board[7][4] = 'K';
}
//print
void printboard() {
    cout<< "\n   a b c d e f g h \n\n";
    
	for(int i = 0; i<8; i++)
	{
		cout << 8-i << "  ";
		for(int j=0; j<8; j++)
		{
			cout<< board[i][j] << " ";
		}
		cout << " " << 8-i << "\n";
	}
	cout<< "\n   a b c d e f g h \n";
	cout << (whitetomove ? "White" : "Black") << " to move";
	if(iskingincheck(whitetomove)) cout << " [CHECK]";
	cout << "\n\n";
	
}