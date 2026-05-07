    #ifndef NOTATION_H
    #define NOTATION_H

    #include <string>
    #include <utility>
    using namespace std;

    pair<int,int> squareToIndex(string square);
    string indexToSquare(int row, int col);
    void parseMove(string move, int &fr, int &fc, int &tr, int &tc);

    #endif