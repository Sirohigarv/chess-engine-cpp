#include <iostream>
#include <cctype>
#include <cmath>
#include "board.h"
#include "moves.h"

using namespace std;

//main
int main(){
    initialiseboard();

    while(true){
        printboard();

        string move;
        cin >> move;

        makemoves(move);
    }

    return 0;
}