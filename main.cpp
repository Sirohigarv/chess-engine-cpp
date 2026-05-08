#include <iostream>
#include <cctype>
#include <cmath>
#include "board.h"
#include "moves.h"
#include "check.h"
#include "engine.h"

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
            cout << "Stalemate! Draw...\n";
            break;
        }

        if(whitetomove)
        {
            string move;
            cout << "Enter move: ";
            cin >> move;
 
            makemoves(move);
        }
        else{
            cout << "Ai is thinking...\n";
            Move aiMove = getBestMove(false, 3);
            make_Move(aiMove);
            if(ispromotion(aiMove)) promotePawn(aiMove);
            whitetomove = !whitetomove;
        }
    }

    return 0;
}     