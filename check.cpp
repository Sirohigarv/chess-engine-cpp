#include "check.h"
#include "board.h"
#include "notation.h"

string findking(){
    char fileb ='?';
    char rankb ='?';
    char filew ='?';
    char rankw ='?';
    
    
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if (board[i][j] == 'k')
            {
                fileb = 'a' + j;
                rankb = '0' + (8 - i);
            }
            if (board[i][j] == 'K')
            {
                filew = 'a' + j;
                rankw = '0' + (8 - i);
            }
            
        }
    }
    
    return string{filew, rankw, fileb, rankb};
}

bool isSquareAttacked(int row, int col, bool byWhite)
{
    
    if (byWhite)
    {
        //pawns
        if (row <= 5)
        {
            if (col > 0 && board[row + 1][col - 1] == 'P')
                return true;
            if (col < 7 && board[row + 1][col + 1] == 'P')
                return true;
        }
        //knights
        int knightmoves[8][2] = {{-2,-1},{-2,1},{2,-1},{2,1},{1,-2},{1,2},{-1,-2},{-1,2}};
        for(auto& m : knightmoves)
        {
            int r = row + m[0];
            int c = col + m[1];
            if(r>= 0 && r<8 && c>=0 && c<8 && board[r][c]=='N')
            {
                return true;
            }
        }
        //rook/partial queen
        int rookdirs[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
        for(auto& d: rookdirs)
        {
            int r = row + d[0];
            int c = col + d[1];
            while(r>=0 && r<8 && c>=0 && c<8 )
            {
                if(board[r][c]!='.')
                {
                    if(board[r][c]=='R' || board[r][c]=='Q')
                    {
                        return true;
                    }
                    break;
                }
                r+= d[0];
                c+= d[1];
            }
        }
        //bishop/partial queen
        int bishopdirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
        for(auto& d: bishopdirs)
        {
            int r = row + d[0];
            int c = col + d[1];
            while(r>=0 && r<8 && c>=0 && c<8 )
            {
                if(board[r][c]!='.')
                {
                    if(board[r][c]=='B' || board[r][c]=='Q')
                    {
                        return true;
                    }
                    break;
                }
                r+= d[0];
                c+= d[1];
            }
        }
        // king
        int kingMoves[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
        for (auto& m : kingMoves)
        {
            int r = row + m[0];
            int c = col + m[1];
            if (r >= 0 && r < 8 && c >= 0 && c < 8 && board[r][c] == 'K')
                return true;
        }
        
    }
    else
    {
        //pawns
        if (row >=2)
        {
            if (col > 0 && board[row - 1][col - 1] == 'p')
                return true;
            if (col < 7 && board[row - 1][col + 1] == 'p')
                return true;
        }
        //knights
        int knightmoves[8][2] = {{-2,-1},{-2,1},{2,-1},{2,1},{1,-2},{1,2},{-1,-2},{-1,2}};
        for(auto& m : knightmoves)
        {
            int r = row + m[0];
            int c = col + m[1];
            if(r>= 0 && r<8 && c>=0 && c<8 && board[r][c]=='n')
            {
                return true;
            }
        }
        //rook/partial queen
        int rookdirs[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
        for(auto& d: rookdirs)
        {
            int r = row + d[0];
            int c = col + d[1];
            while(r>=0 && r<8 && c>=0 && c<8 )
            {
                if(board[r][c]!='.')
                {
                    if(board[r][c]=='r' || board[r][c]=='q')
                    {
                        return true;
                    }
                    break;
                }
                r+= d[0];
                c+= d[1];
            }
        }
        //bishop/partial queen
        int bishopdirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
        for(auto& d: bishopdirs)
        {
            int r = row + d[0];
            int c = col + d[1];
            while(r>=0 && r<8 && c>=0 && c<8 )
            {
                if(board[r][c]!='.')
                {
                    if(board[r][c]=='b' || board[r][c]=='q')
                    {
                        return true;
                    }
                    break;
                }
                r+= d[0];
                c+= d[1];
            }
        }
        // king
        int kingMoves[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
        for (auto& m : kingMoves)
        {
            int r = row + m[0];
            int c = col + m[1];
            if (r >= 0 && r < 8 && c >= 0 && c < 8 && board[r][c] == 'k')
                return true;
        }
    }

    return false;
}

bool iskingincheck(bool white)
{
    string kings = findking();
    string kingsquare;
    if(white)
    {
        kingsquare = kings.substr(0,2);
    }
    else
    {
        kingsquare = kings.substr(2,2);
    }
    auto [row,col] = squareToIndex(kingsquare);

    return isSquareAttacked(row,col,!white); //by enemy piece, !white
}