#include "engine.h"
#include <algorithm>
#include "board.h"
#include "moves.h"
#include "notation.h"
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

// Each line is a full opening, 5 moves deep for both sides (10 half-moves), in from-square/to-square notation.
// Castling is written as the king's move (e1g1, e8g8).
static const vector<vector<string>> openingLines = {
    // Ruy Lopez (Morphy Defence)
    {"e2e4","e7e5","g1f3","b8c6","f1b5","a7a6","b5a4","g8f6","e1g1","f8e7"},
    // Italian Game (Giuoco Piano)
    {"e2e4","e7e5","g1f3","b8c6","f1c4","f8c5","c2c3","g8f6","d2d4","e5d4"},
    // Scotch Game
    {"e2e4","e7e5","g1f3","b8c6","d2d4","e5d4","f3d4","g8f6","d4c6","b7c6"},
    // Petrov Defence
    {"e2e4","e7e5","g1f3","g8f6","f3e5","d7d6","e5f3","f6e4","d2d4","d6d5"},
    // Sicilian Najdorf
    {"e2e4","c7c5","g1f3","d7d6","d2d4","c5d4","f3d4","g8f6","b1c3","a7a6"},
    // French Defence (Classical)
    {"e2e4","e7e6","d2d4","d7d5","b1c3","g8f6","c1g5","f8e7","e4e5","f6d7"},
    // Caro-Kann (Classical)
    {"e2e4","c7c6","d2d4","d7d5","b1c3","d5e4","c3e4","c8f5","e4g3","f5g6"},
    // Queen's Gambit Declined
    {"d2d4","d7d5","c2c4","e7e6","b1c3","g8f6","c1g5","f8e7","e2e3","e8g8"},
    // Queen's Gambit Accepted
    {"d2d4","d7d5","c2c4","d5c4","g1f3","g8f6","e2e3","e7e6","f1c4","c7c5"},
    // Slav Defence
    {"d2d4","d7d5","c2c4","c7c6","g1f3","g8f6","b1c3","d5c4","a2a4","c8f5"},
    // London System
    {"d2d4","d7d5","c1f4","g8f6","e2e3","e7e6","g1f3","c7c5","c2c3","b8c6"},
    // King's Indian Defence
    {"d2d4","g8f6","c2c4","g7g6","b1c3","f8g7","e2e4","d7d6","g1f3","e8g8"},
    // Nimzo-Indian Defence
    {"d2d4","g8f6","c2c4","e7e6","b1c3","f8b4","e2e3","e8g8","f1d3","d7d5"},
    // English Opening (Reversed Sicilian)
    {"c2c4","e7e5","b1c3","g8f6","g1f3","b8c6","g2g3","d7d5","c4d5","f6d5"},
};

// key = moves played so far joined by spaces ("" for the start position), value = possible next moves
static string historyKey(const vector<string>& moves, size_t count) {
    string key;
    for (size_t i = 0; i < count; i++) {
        if (i > 0) key += ' ';
        key += moves[i];
    }
    return key;
}

void initOpeningBook() {
    srand(static_cast<unsigned>(time(nullptr)));
    openingBook.clear();
    moveHistory.clear();
    for (const auto& line : openingLines) {
        for (size_t i = 0; i < line.size(); i++) {
            vector<string>& replies = openingBook[historyKey(line, i)];
            if (find(replies.begin(), replies.end(), line[i]) == replies.end())
                replies.push_back(line[i]);
        }
    }
}

string moveToString(const Move& m) {
    return indexToSquare(m.fr, m.fc) + indexToSquare(m.tr, m.tc);
}

// call after every move actually played on the board (human or engine)
void recordMove(const string& move) {
    moveHistory.push_back(move.substr(0, 4));
}

// returns true and fills 'out' if the current position is in the book
bool getBookMove(bool white, Move& out) {
    auto it = openingBook.find(historyKey(moveHistory, moveHistory.size()));
    if (it == openingBook.end() || it->second.empty()) return false;

    const string& choice = it->second[rand() % it->second.size()];

    // only play the book move if it is legal in the current position
    for (const Move& m : generateLegalMoves(white)) {
        if (moveToString(m) == choice) {
            out = m;
            return true;
        }
    }
    return false;
}
