#include "engine.h"
#include <algorithm>
#include "board.h"
#include "moves.h"
#include <climits>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

int pieceValue(char piece) {
    switch (tolower(piece)) {
        case 'p': return 100;
        case 'n': return 320;
        case 'b': return 330;
        case 'r': return 500;
        case 'q': return 900;
        case 'k': return 20000;
        default:  return 0;
    }
}
//----------------------------------------Evaluation Function--------------------------------------------------------------//
int evaluate() {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            char piece = board[r][c];
            if (piece == '.') continue;
            int val = pieceValue(piece);
            if (isupper(piece)) score += val;
            else                score -= val;
        }
    }
    return score;
}
//-----------------------------Quiscience search----------------------------------------------------------------//

bool isCapture(const Move& m) {
    return m.isEnPassant || m.capturedPiece != '.';
}

int moveScore(const Move& m) {
    if (m.isEnPassant)
        return 10 * pieceValue('P') - pieceValue(m.movedPiece);
    if (m.capturedPiece != '.')
        return 10 * pieceValue(m.capturedPiece) - pieceValue(m.movedPiece);
    return 0;
}

void orderMoves(vector<Move>& moves) {
    sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return moveScore(a) > moveScore(b);
    });
}

int quiescence(int alpha, int beta, bool white) {
    bool inCheck = iskingincheck(white);
    if (!inCheck) {
        int standPat = evaluate();
        if (white) {
            if (standPat >= beta) return beta;
            if (alpha < standPat) alpha = standPat;
        } else {
            if (standPat <= alpha) return alpha;
            if (beta > standPat) beta = standPat;
        }
    }

    vector<Move> moves = generateLegalMoves(white);
    vector<Move> searchMoves;
    if (inCheck) {
        searchMoves = moves; // forced to respond, no stand pat allowed
    } else {
        for (auto& m : moves) if (isCapture(m)) searchMoves.push_back(m);
    }
    orderMoves(searchMoves);

    if (inCheck && searchMoves.empty()) {
        // checkmate reached inside quiescence
        return white ? -100000 : 100000;
    }

    for (auto& m : searchMoves) {
        make_Move(m);
        if (ispromotion(m)) promotePawn(m);
        int score = quiescence(alpha, beta, !white);
        undoMove(m);

        if (white) {
            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        } else {
            if (score <= alpha) return alpha;
            if (score < beta) beta = score;
        }
    }
    return white ? alpha : beta;
}



//-----------------------------Minimax & Alpha Beta Pruning----------------------------------------------------------------//

int minimax(int depth, int alpha, int beta, bool white) {
    if (depth <= 0) return quiescence(alpha, beta, white);
    vector<Move> moves = generateLegalMoves(white);
    if (moves.empty()) {
        if (iskingincheck(white)) return white ? -100000 - depth : 100000 + depth;
        return 0;
    }
    orderMoves(moves);
    if (white) {
        int maxScore = INT_MIN;
        for (auto& m : moves) {
            make_Move(m);
            if (ispromotion(m)) promotePawn(m);
            int score = minimax(depth - 1, alpha, beta, false);
            undoMove(m);
            maxScore = max(maxScore, score);
            alpha = max(alpha, score);
            if (beta <= alpha) break;
        }
        return maxScore;
    } else {
        int minScore = INT_MAX;
        for (auto& m : moves) {
            make_Move(m);
            if (ispromotion(m)) promotePawn(m);
            int score = minimax(depth - 1, alpha, beta, true);
            undoMove(m);
            minScore = min(minScore, score);
            beta = min(beta, score);
            if (beta <= alpha) break;
        }
        return minScore;
    }
}

Move getBestMove(bool white, int depth) {
    vector<Move> moves = generateLegalMoves(white);
    orderMoves(moves);
    Move bestMove = moves[0];
    int bestScore = white ? INT_MIN : INT_MAX;
    for (auto& m : moves) {
        make_Move(m);
        if (ispromotion(m)) promotePawn(m);
        int score = minimax(depth - 1, INT_MIN, INT_MAX, !white);
        undoMove(m);
        if (white && score >= bestScore) { bestScore = score; bestMove = m; }
        if (!white && score <= bestScore) { bestScore = score; bestMove = m; }
    }
    return bestMove;
}
//------------------------------------opening book------------------------------------------------------------------------// 
unordered_map<string, vector<string>> openingBook;
vector<string> moveHistory;
