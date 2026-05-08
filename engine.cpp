#include "engine.h"
#include "board.h"
#include "moves.h"
#include <climits>

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

int minimax(int depth, int alpha, int beta, bool white) {
    if (depth == 0) return evaluate();
    vector<Move> moves = generateLegalMoves(white);
    if (moves.empty()) {
        if (iskingincheck(white)) return white ? -100000 : 100000;
        return 0;
    }
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
    Move bestMove = moves[0];
    int bestScore = white ? INT_MIN : INT_MAX;
    for (auto& m : moves) {
        make_Move(m);
        if (ispromotion(m)) promotePawn(m);
        int score = minimax(depth - 1, INT_MIN, INT_MAX, !white);
        undoMove(m);
        if (white && score > bestScore) { bestScore = score; bestMove = m; }
        if (!white && score < bestScore) { bestScore = score; bestMove = m; }
    }
    return bestMove;
}