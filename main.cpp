#include <iostream>
#include <cctype>
#include <cmath>
using namespace std;

char board[8][8];
bool whitetomove = true;

//initialise board
void initialiseboard()
{
    for(int i=0;i<8;i++)
    {
        for(int j= 0;j<8;j++ )
        {
            board[i][j] =  '.';
        }
       
    }
    //pawns
    for(int i=0;i<8;i++)
    {
        board[1][i] = 'p';
        board[6][i] = 'P';
    }
    board[0][0] = 'r';
    board[0][7] = 'r';
    board[7][0] = 'R';
    board[7][7] = 'R';

    board[0][1] = 'n';
    board[0][6] = 'n';
    board[7][1] = 'N';
    board[7][6] = 'N';

    board[0][2] = 'b';
    board[0][5] = 'b';
    board[7][2] = 'B';
    board[7][5] = 'B';

    board[0][3] = 'q';
    board[0][4] = 'k';
    board[7][3] = 'Q';
    board[7][4] = 'K';
}

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
string findking(){
    char fileb ='?';
    char rankb ='?';
    char filew ='?';
    char rankw ='?';
    
    bool found = false;
    
    for(int i=0;i<8 && !found;i++)
    {
        for(int j=0;j<8;j++)
        {
            if (board[i][j] == 'k')
            {
                fileb = 'a' + j;
                rankb = '0' + 8 - i;
            }
            if (board[i][j] == 'K')
            {
                filew = 'a' + j;
                rankw = '0' + 8 - i;
            }
            if(filew != '?' && fileb != '?')
            {
                found = true;
                break;
            }
        }
    }
    return string{filew, rankw, fileb, rankb};
}
bool issquareattacked(int row, int col, bool bywhite)
{
    return false;
}

bool iskingincheck(bool white)
{
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
    whitetomove=!whitetomove;

}

//print
void printboard(){
    for(int i = 0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            cout<< board[i][j];
        }
        cout << endl;
    }
}
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