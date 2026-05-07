#ifndef CHECK_H
#define CHECK_H

#include <string>
#include <vector>

using namespace std;

string findking();
bool isSquareAttacked(int row, int col, bool byWhite);
bool iskingincheck(bool white);
int gameStatus();

#endif 