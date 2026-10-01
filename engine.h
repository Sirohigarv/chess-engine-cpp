#ifndef ENGINE_H
#define ENGINE_H
#include "moves.h"

int pieceValue(char piece);
int evaluate();
int minimax(int depth, int alpha, int beta, bool white);
Move getBestMove(bool white, int depth);

void initOpeningBook();
string moveToString(const Move& m);
void recordMove(const string& move);
bool getBookMove(bool white, Move& out);

#endif