#include <SFML/Graphics.hpp>

#include "board.h"
#include "moves.h"
#include "notation.h"
#include "engine.h"

#include <cctype>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace {

constexpr int   TILE_SIZE    = 80;
constexpr int   BOARD_SIZE   = 8;
constexpr float TILE_F       = static_cast<float>(TILE_SIZE);
constexpr int   SEARCH_DEPTH = 3;


constexpr bool HUMAN_IS_WHITE = true;

sf::Vector2f squareTopLeft(int row, int col) {
    return { col * TILE_F, row * TILE_F };
}

sf::Vector2f squareCentre(int row, int col) {
    return { col * TILE_F + TILE_F / 2.f, row * TILE_F + TILE_F / 2.f };
}

bool ownsPiece(char piece, bool white) {
    if (piece == '.') return false;
    const auto c = static_cast<unsigned char>(piece);
    return white ? std::isupper(c) != 0 : std::islower(c) != 0;
}

std::vector<Move> movesFromSquare(bool white, int row, int col) {
    std::vector<Move> out;
    for (const Move& m : generateLegalMoves(white))
        if (m.fr == row && m.fc == col)
            out.push_back(m);
    return out;
}

} 

int main() {
    sf::RenderWindow window(
        sf::VideoMode({ static_cast<unsigned int>(TILE_SIZE * BOARD_SIZE),
                        static_cast<unsigned int>(TILE_SIZE * BOARD_SIZE) }),
        "Chess Engine");
    window.setFramerateLimit(60);

    const sf::Color light(237, 220, 255);
    const sf::Color dark(101, 55, 155);
    const sf::Color highlight(255, 255, 0, 100);
    const sf::Color legalDot(0, 0, 0, 80);

    sf::RectangleShape tile({ TILE_F, TILE_F });

    sf::RectangleShape highlightTile({ TILE_F, TILE_F });
    highlightTile.setFillColor(highlight);

    sf::CircleShape dot(12.f);
    dot.setFillColor(legalDot);
    dot.setOrigin({ 12.f, 12.f });

    
    const std::map<char, std::string> files = {
        {'K', "pieces/wK.png"}, {'Q', "pieces/wQ.png"},
        {'R', "pieces/wR.png"}, {'B', "pieces/wB.png"},
        {'N', "pieces/wN.png"}, {'P', "pieces/wP.png"},
        {'k', "pieces/bK.png"}, {'q', "pieces/bQ.png"},
        {'r', "pieces/bR.png"}, {'b', "pieces/bB.png"},
        {'n', "pieces/bN.png"}, {'p', "pieces/bP.png"}
    };

    std::map<char, sf::Texture> textures;
    for (const auto& [ch, path] : files) {
        sf::Texture tex;
        if (!tex.loadFromFile(path)) {
            std::cerr << "Failed to load texture: " << path
                      << "\nRun the exe from the folder that contains pieces/.\n";
            return -1;
        }
        tex.setSmooth(true);           
        textures.emplace(ch, std::move(tex));
    }

    initialiseboard();

    
    bool pieceSelected = false;
    int  selectedRow = -1, selectedCol = -1;
    std::vector<Move> legalMoves;

    bool engineOwesMove = false;   
    bool gameOver       = false;

    while (window.isOpen()) {

        
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }

            if (gameOver || engineOwesMove) continue;
            if (whitetomove != HUMAN_IS_WHITE) continue;

            const auto* click = event->getIf<sf::Event::MouseButtonPressed>();
            if (!click || click->button != sf::Mouse::Button::Left) continue;

            // integer division rounds toward zero, so reject negatives before dividing
            if (click->position.x < 0 || click->position.y < 0) continue;
            const int col = click->position.x / TILE_SIZE;
            const int row = click->position.y / TILE_SIZE;
            if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) continue;

            const char clicked = board[row][col];

            if (!pieceSelected) {
                if (ownsPiece(clicked, whitetomove)) {
                    pieceSelected = true;
                    selectedRow   = row;
                    selectedCol   = col;
                    legalMoves    = movesFromSquare(whitetomove, row, col);
                }
                continue;
            }

            
            bool moved = false;
            for (const Move& m : legalMoves) {
                if (m.tr == row && m.tc == col) {
                    make_Move(m);
                    if (ispromotion(m)) promotePawn(m);
                    whitetomove    = !whitetomove;
                    moved          = true;
                    engineOwesMove = true;
                    break;
                }
            }

            pieceSelected = false;
            selectedRow   = -1;
            selectedCol   = -1;
            legalMoves.clear();

            
            if (!moved && ownsPiece(clicked, whitetomove)) {
                pieceSelected = true;
                selectedRow   = row;
                selectedCol   = col;
                legalMoves    = movesFromSquare(whitetomove, row, col);
            }
        }

        if (!window.isOpen()) break;

        
        window.clear(sf::Color::Black);

        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                tile.setFillColor(((row + col) % 2 == 0) ? light : dark);
                tile.setPosition(squareTopLeft(row, col));
                window.draw(tile);
            }
        }

        if (pieceSelected) {
            highlightTile.setPosition(squareTopLeft(selectedRow, selectedCol));
            window.draw(highlightTile);

            for (const Move& m : legalMoves) {
                dot.setPosition(squareCentre(m.tr, m.tc));
                window.draw(dot);
            }
        }

        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                const char piece = board[row][col];
                if (piece == '.') continue;

                const auto it = textures.find(piece);
                if (it == textures.end()) continue;   

                sf::Sprite sprite(it->second);        
                const sf::Vector2u texSize = it->second.getSize();
                sprite.setScale({ TILE_F / texSize.x, TILE_F / texSize.y });
                sprite.setPosition(squareTopLeft(row, col));
                window.draw(sprite);
            }
        }

        window.display();

        if (engineOwesMove && !gameOver) {
            engineOwesMove = false;

            if (generateLegalMoves(whitetomove).empty()) {
                gameOver = true;
                window.setTitle("Chess Engine - game over");
            } else {
                const Move best = getBestMove(whitetomove, SEARCH_DEPTH);
                make_Move(best);
                if (ispromotion(best)) promotePawn(best);
                whitetomove = !whitetomove;

                if (generateLegalMoves(whitetomove).empty()) {
                    gameOver = true;
                    window.setTitle("Chess Engine - game over");
                }
            }
        }
    }

    return 0;
}