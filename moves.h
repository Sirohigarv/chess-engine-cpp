#ifndef MOVES_H
#define MOVES_H
#include "board.h"

#include <string>
#include <vector>
using namespace std;

struct Move {
    int fr, fc, tr, tc;
    char movedPiece;
    char capturedPiece;

   
    bool isCastle       = false;
    bool isEnPassant    = false;
    bool isPromotion    = false;

    
    bool savedWK, savedWQ, savedBK, savedBQ;
    int  savedEPRow, savedEPCol;
};

Move createMove(const string &move);

void make_Move(Move m);
void undoMove(Move m);

bool pseudoLegal(string move);

bool islegalpawn(string move);
bool islegalknight(string move);
bool islegalrook(string move);
bool islegalbishop(string move);
bool islegalqueen(string move);
bool islegalking(string move);
bool islegalcastle(string move);
bool ispromotion(const Move &m);
void promotePawn(const Move &m);
vector<Move> generateLegalMoves(bool white);

void makemoves(const string &move);

#endif