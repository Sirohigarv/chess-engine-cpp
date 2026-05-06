#ifndef MOVES_H
#define MOVES_H

#include <string>

struct Move {
    int fr, fc, tr, tc;
    char movedPiece;
    char capturedPiece;
};

extern char board[8][8];
extern bool whitetomove;

Move createMove(string move);

void make_Move(Move m);
void undoMove(Move m);

bool legalmove(string move);

bool islegalpawn(string move);
bool islegalknight(string move);
bool islegalrook(string move);
bool islegalbishop(string move);
bool islegalqueen(string move);
bool islegalking(string move);

bool ispromotion(const Move &m);
void promotePawn(const Move &m);

void makemoves(string move);

#endif