#include <iostream>
using namespace std;

#include "moves.h"
#include "board.h"
#include "notation.h"
#include "check.h"

bool islegalpawn(string move){
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    if(whitetomove)
    {
        if(fr-tr<=0){return false;} //forward direction
        
        if (abs(fc - tc) == 1) {
            if (fr - tr == 1 && islower(board[tr][tc]))
                return true;
            return false;
        }
        if(fc-tc!=0)//stay in one column
        {
            return false;
        }
        if(fr-tr==1 && board[tr][tc]=='.'){return true;}
        if(fr==6 && tr==4 && board[5][tc]=='.' && board[4][tc]=='.'){return true;}
        
        return false;
        
    }
    if(!whitetomove)
    {
        if(fr-tr>=0){return false;} //forward direction
        
        if (abs(fc - tc) == 1) {
            if (fr - tr == -1 && isupper(board[tr][tc]))
                return true;
            return false;
        }
        if(fc-tc!=0)//stay in one column
        {
            return false;
        }
        if(fr-tr==-1 && board[tr][tc]=='.'){return true;}
        if(fr==1 && tr==3 && board[2][tc]=='.' && board[3][tc]=='.'){return true;}
        
        return false;
    }
    return false;
}
bool islegalknight(string move){
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    
    if(!((abs(fr-tr)==1 &&abs(fc-tc)==2)||(abs(fr-tr)==2 && abs(fc-tc)==1)))
    {
        return false;
    }
    if((isupper(board[fr][fc]) && isupper(board[tr][tc])) || (islower(board[fr][fc]) && islower(board[tr][tc])))
    {
        return false;
    }
    return true;
}

bool islegalrook(string move){
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    char piece = board[fr][fc];
    char destination = board[tr][tc];
    
    if(fr!=tr && fc!= tc){return false;}
    
    if((islower(piece) && islower(destination)) || (isupper(piece) && isupper(destination)))
    {
        return false;
    }
    
    if(fc == tc){
        
        int i = (tr > fr) ? 1 : -1;
        
        for(int r = fr + i; r != tr; r += i)
        {
            if(board[r][fc] != '.') return false;
        }
    }
    
    if(fr == tr){
        int i = (tc > fc) ? 1 : -1;
        
        for(int c = fc + i; c != tc; c += i)
        {
            if(board[fr][c] != '.') return false;
        }
    }
    
    return true;
}
bool islegalbishop(string move){
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    
    char piece = board[fr][fc];
    char destination = board[tr][tc];
    
    if(abs(fr-tr)!=abs(fc-tc)) {return false;}
    if((islower(piece) && islower(destination)) || (isupper(piece) && isupper(destination)))
    {
        return false;
    }
    
    int rStep = (tr > fr) ? 1 : -1;
    int cStep = (tc > fc) ? 1 : -1;

    int r = fr + rStep;
    int c = fc + cStep;

    while(r != tr && c != tc){
        if(board[r][c] != '.') return false;
        r += rStep;
        c += cStep;
    }
    
    return true;
}
bool islegalqueen(string move){
    return islegalrook(move) || islegalbishop(move);
}
bool islegalking(string move){
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    
    char piece = board[fr][fc];
    char destination = board[tr][tc];
    
    if((islower(piece) && islower(destination)) || (isupper(piece) && isupper(destination)))
    {
        return false;
    }
    
    if(abs(fr-tr)<=1 && abs(fc-tc)<=1)
    {
        return true;
    }
    
    return false;
}


bool legalmove(string move){
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    
    char piece = board[fr][fc];
    if(piece == 'P' || piece == 'p'){return islegalpawn(move);}
    if(piece == 'N' || piece == 'n'){return islegalknight(move);}
    if(piece == 'B' || piece == 'b'){return islegalbishop(move);}
    if(piece == 'R' || piece == 'r'){return islegalrook(move);}
    if(piece == 'Q' || piece == 'q'){return islegalqueen(move);}
    if(piece == 'K' || piece == 'k'){return islegalking(move);}
    
    return false;
}

//make moves
void makemoves(string move){ 
    
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    char piece = board[fr][fc];
    char destination = board[tr][tc];
    
    if(piece == '.'){
        cout<< "no piece on that square"<<endl;
        return;
    }  
    if(whitetomove && (piece>='a' && piece<='z'))
    {
        cout<<"white's turn"<<endl;
        return;
    }
    if(!whitetomove && (piece>='A' && piece<='Z'))
    {
        cout<<"Black's turn"<<endl;
        return;
    }
    
    if(!legalmove(move))
    {
        cout<<"illegal"<<endl;
        return;
    }
    
    board[tr][tc] = piece;
    board[fr][fc] = '.';
    
    if(iskingincheck(whitetomove))
    {
        //undo the move
        board[fr][fc] = piece;
        board[tr][tc] = destination;
        cout << "leaves king in check, ILLEGAL"<<endl;
        return;
    }

    
    whitetomove=!whitetomove;

}