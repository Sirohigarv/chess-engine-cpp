#ifndef MOVES_H
#define MOVES_H

#include <string>
using namespace std;

bool islegalpawn(string move);
bool islegalknight(string move);
bool islegalbishop(string move);
bool islegalrook(string move);
bool islegalqueen(string move);
bool islegalking(string move);

bool legalmove(string move);
void makemoves(string move);

#endif