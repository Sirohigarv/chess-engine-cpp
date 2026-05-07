#include <iostream>
#include <cctype>
#include <cmath>
#include "board.h"
#include "moves.h"
#include "check.h"

using namespace std;

//main
int main(){
    initialiseboard();

    while(true){
        printboard();
        int status = gameStatus();
        if(status == 1)
        {
            cout << (whitetomove ? "Black" : "White") << " wins by checkmate!\n";
            break;
        }
        if(status == 2)
        {
            cout << "Stalemate! It's a draw.\n";
            break;
        }

        string move;
        cout << "Enter move: ";
        cin >> move;

        makemoves(move);
    }

    return 0;
}     