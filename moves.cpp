#include <iostream>
using namespace std;
#include <cmath>
#include "moves.h"
#include "board.h"
#include "notation.h"
#include "check.h"



Move createMove(const string &move) {
	int fr,fc,tr,tc;
	parseMove(move,fr,fc,tr,tc);

	Move m;
	m.fr = fr;
	m.fc = fc;
	m.tr = tr;
	m.tc = tc;
	m.movedPiece = board[fr][fc];
	m.capturedPiece = board[tr][tc];
	//saved state for undo
	m.savedWK   = whiteKingsideCastle;
    m.savedWQ   = whiteQueensideCastle;
    m.savedBK   = blackKingsideCastle;
    m.savedBQ   = blackQueensideCastle;
    m.savedEPRow = enPassantRow;
    m.savedEPCol = enPassantCol;
    
    if ((m.movedPiece == 'P' || m.movedPiece == 'p') && tr == enPassantRow && tc == enPassantCol){
        m.isEnPassant = true;
    }
    if (m.movedPiece == 'K' && fr == 7 && fc == 4 && (tc == 6 || tc == 2))
        m.isCastle = true;
    if (m.movedPiece == 'k' && fr == 0 && fc == 4 && (tc == 6 || tc == 2))
        m.isCastle = true;
    

	return m;
}
void make_Move(Move m) {
    
    enPassantRow = -1;
    enPassantCol = -1;
    
    if (m.movedPiece == 'P' && m.fr == 6 && m.tr == 4) {
        enPassantRow = 5;
        enPassantCol = m.fc;
    }
    if (m.movedPiece == 'p' && m.fr == 1 && m.tr == 3) {
        enPassantRow = 2;
        enPassantCol = m.fc;
    }
    //capture enpassant
    if (m.isEnPassant) {
        int capturedRow = (m.movedPiece == 'P') ? m.tr + 1 : m.tr - 1;
        board[capturedRow][m.tc] = '.';
    }
    
    
	board[m.tr][m.tc] = m.movedPiece;
	board[m.fr][m.fc] = '.';
	
	//castling also move rook
	if (m.isCastle) {
        if (m.movedPiece == 'K') {
            if (m.tc == 6) { board[7][5]='R'; board[7][7]='.'; }  // kingside
            else           { board[7][3]='R'; board[7][0]='.'; }  // queenside
        } else {
            if (m.tc == 6) { board[0][5]='r'; board[0][7]='.'; }
            else           { board[0][3]='r'; board[0][0]='.'; }
        }
    }
    
    // Update castling rights
    if (m.movedPiece == 'K') { whiteKingsideCastle = whiteQueensideCastle = false; }
    if (m.movedPiece == 'k') { blackKingsideCastle = blackQueensideCastle = false; }
    if (m.movedPiece == 'R') {
        if (m.fr == 7 && m.fc == 7) whiteKingsideCastle  = false;
        if (m.fr == 7 && m.fc == 0) whiteQueensideCastle = false;
    }
    if (m.movedPiece == 'r') {
        if (m.fr == 0 && m.fc == 7) blackKingsideCastle  = false;
        if (m.fr == 0 && m.fc == 0) blackQueensideCastle = false;
    }
    // Rook captured → remove rights
    if (m.capturedPiece == 'R') {
        if (m.tr == 7 && m.tc == 7) whiteKingsideCastle  = false;
        if (m.tr == 7 && m.tc == 0) whiteQueensideCastle = false;
    }
    if (m.capturedPiece == 'r') {
        if (m.tr == 0 && m.tc == 7) blackKingsideCastle  = false;
        if (m.tr == 0 && m.tc == 0) blackQueensideCastle = false;
    }
}

void undoMove(Move m) {
	board[m.fr][m.fc] = m.movedPiece;
	board[m.tr][m.tc] = m.capturedPiece;
	
	if (m.isEnPassant) {
        int capturedRow = (m.movedPiece == 'P') ? m.tr + 1 : m.tr - 1;
        board[capturedRow][m.tc] = (m.movedPiece == 'P') ? 'p' : 'P';
    }

    if (m.isCastle) {
        if (m.movedPiece == 'K') {
            if (m.tc == 6) { board[7][7]='R'; board[7][5]='.'; }
            else           { board[7][0]='R'; board[7][3]='.'; }
        } else {
            if (m.tc == 6) { board[0][7]='r'; board[0][5]='.'; }
            else           { board[0][0]='r'; board[0][3]='.'; }
        }
    }

    // Restore state
    whiteKingsideCastle  = m.savedWK;
    whiteQueensideCastle = m.savedWQ;
    blackKingsideCastle  = m.savedBK;
    blackQueensideCastle = m.savedBQ;
    enPassantRow = m.savedEPRow;
    enPassantCol = m.savedEPCol;
}

bool islegalpawn(string move) {
    int fr, fc, tr, tc;
    parseMove(move,fr,fc,tr,tc);
    char piece = board[fr][fc];
    bool white = isupper(piece);
    int dir = white ? -1 : 1;
    int startRow = white ? 6 : 1;

    // Captures (including en passant)
    if (abs(fc-tc) == 1 && tr - fr == dir) {
        if (tr == enPassantRow && tc == enPassantCol) return true;
        char dest = board[tr][tc];
        if (dest != '.' && (white ? islower(dest) : isupper(dest))) return true;
        return false;
    }
    if (fc != tc) return false;
    if (board[tr][tc] != '.') return false;
    if (tr - fr == dir) return true;
    if (fr == startRow && tr - fr == 2*dir && board[fr+dir][fc] == '.') return true;
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
bool islegalcastle(string move) {
    int fr,fc,tr,tc;
    parseMove(move,fr,fc,tr,tc);
    // White
    if (board[fr][fc]=='K' && fr==7 && fc==4) {
        if (tc==6 && tr==7 && whiteKingsideCastle &&
            board[7][5]=='.'&&board[7][6]=='.' &&
            !isSquareAttacked(7,4,false)&&
            !isSquareAttacked(7,5,false)&&
            !isSquareAttacked(7,6,false)) return true;
        if (tc==2 && tr==7 && whiteQueensideCastle &&
            board[7][3]=='.'&&board[7][2]=='.'&&board[7][1]=='.' &&
            !isSquareAttacked(7,4,false)&&
            !isSquareAttacked(7,3,false)&&
            !isSquareAttacked(7,2,false)) return true;
    }
    // Black
    if (board[fr][fc]=='k' && fr==0 && fc==4) {
        if (tc==6 && tr==0 && blackKingsideCastle &&
            board[0][5]=='.'&&board[0][6]=='.' &&
            !isSquareAttacked(0,4,true)&&
            !isSquareAttacked(0,5,true)&&
            !isSquareAttacked(0,6,true)) return true;
        if (tc==2 && tr==0 && blackQueensideCastle &&
            board[0][3]=='.'&&board[0][2]=='.'&&board[0][1]=='.' &&
            !isSquareAttacked(0,4,true)&&
            !isSquareAttacked(0,3,true)&&
            !isSquareAttacked(0,2,true)) return true;
    }
    return false;
}

bool pseudoLegal(string move) {
    int fr, fc, tr, tc;
    parseMove(move, fr, fc, tr, tc);
    
    char piece = board[fr][fc];
    if (piece == '.') return false;
    char p = tolower(piece);
    if (p=='p') return islegalpawn(move);
    if (p=='n') return islegalknight(move);
    if (p=='b') return islegalbishop(move);
    if (p=='r') return islegalrook(move);
    if (p=='q') return islegalqueen(move);
    if (p=='k') {
        if (islegalcastle(move)) return true;
        return islegalking(move);
    }
    return false;
}

vector<Move> generateLegalMoves(bool white) {
    vector<Move> legal;
    for (int fr = 0; fr < 8; fr++) {
        for (int fc = 0; fc < 8; fc++) {
            char piece = board[fr][fc];
            if (piece == '.') continue;
            if (white  && islower(piece)) continue;
            if (!white && isupper(piece)) continue;

            for (int tr = 0; tr < 8; tr++) {
                for (int tc = 0; tc < 8; tc++) {
                    if (fr==tr && fc==tc) continue;

                    string moveStr = indexToSquare(fr,fc) + indexToSquare(tr,tc);

                    if (!pseudoLegal(moveStr)) continue;

                    Move m = createMove(moveStr);
                    make_Move(m);
                    bool inCheck = iskingincheck(white);
                    undoMove(m);
                    if (!inCheck) legal.push_back(m);
                }
            }
        }
    }
    return legal;
}

bool ispromotion(const Move &m)
{
    if(m.movedPiece == 'p' && m.tr == 7) return true;
    if(m.movedPiece == 'P' && m.tr == 0) return true;
    return false;
}

void promotePawn(const Move &m){
    board[m.tr][m.tc] = (m.movedPiece == 'P') ? 'Q' : 'q';
}


//make moves
void makemoves(const string& move) {
    int fr,fc,tr,tc;
    parseMove(move, fr, fc, tr, tc);

    char piece = board[fr][fc];
    if (piece == '.') { cout << "No piece on that square.\n"; return; }
    if ( whitetomove && islower(piece)) { cout << "It's White's turn.\n"; return; }
    if (!whitetomove && isupper(piece)) { cout << "It's Black's turn.\n"; return; }

    string moveStr = indexToSquare(fr,fc) + indexToSquare(tr,tc);
    if (!pseudoLegal(moveStr)) { cout << "Illegal move.\n"; return; }

    Move m = createMove(move);
    make_Move(m);

    if (iskingincheck(whitetomove)) {
        undoMove(m);
        cout << "That move leaves your king in check!\n";
        return;
    }

    if (ispromotion(m)) promotePawn(m);

    whitetomove = !whitetomove;
}