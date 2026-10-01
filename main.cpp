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
    initOpeningBook();

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
            if (!(cin >> move)) break;   // input closed (EOF) - stop instead of looping forever

            bool before = whitetomove;
            makemoves(move);
            if (whitetomove != before) recordMove(move);   // only record moves that were accepted
        }
        else{
            cout << "Ai is thinking...\n";
            Move aiMove;
            if (getBookMove(false, aiMove)) cout << "Book move\n";
            else aiMove = getBestMove(false, 3);
            recordMove(moveToString(aiMove));
            make_Move(aiMove);
            if(ispromotion(aiMove)) promotePawn(aiMove);
            whitetomove = !whitetomove;
        }
    }

    return 0;
}