#ifndef BOARD_H
#define BOARD_H
#include "check.h"
using namespace std;
extern char board[8][8];
extern bool whitetomove;

extern bool whiteKingsideCastle;
extern bool whiteQueensideCastle;
extern bool blackKingsideCastle;
extern bool blackQueensideCastle;

extern int enPassantRow;
extern int enPassantCol;


void initialiseboard();
void printboard();

#endif