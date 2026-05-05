#include "notation.h"

pair<int,int> squareToIndex(string square){
    int row = 8-(square[1] - '0');
    int column = square[0]-'a';
    
    return {row,column};
}

string indexToSquare(int row, int col){
    char file = 'a' + col;
    char rank = '0' + (8 - row);
    return string{file, rank};
}

void parseMove(string move, int &fr, int &fc, int &tr, int &tc){
    auto from = squareToIndex(move.substr(0,2));
    auto to   = squareToIndex(move.substr(2,2));

    fr = from.first;
    fc = from.second;
    tr = to.first;
    tc = to.second;
}