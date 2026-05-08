#include <SFML/Graphics.hpp>
#include "board.h"
#include "moves.h"
#include "notation.h"
#include <map>
#include <string>
#include <vector>
#include "engine.h"
//written completely by claude newmain.cpp

const int TILE_SIZE = 80;
const int BOARD_SIZE = 8;

int main() {
    sf::RenderWindow window(
        sf::VideoMode({TILE_SIZE * BOARD_SIZE, TILE_SIZE * BOARD_SIZE}),
        "Chess Engine"
    );

    sf::Color light(237, 220, 255);
sf::Color dark(101, 55, 155);
    sf::Color highlight(255, 255, 0, 100);  // yellow highlight
    sf::Color legalDot(0, 0, 0, 80);        // legal move dot

    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    sf::RectangleShape highlightTile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    highlightTile.setFillColor(highlight);

    sf::CircleShape dot(12.f);
    dot.setFillColor(legalDot);
    dot.setOrigin({12.f, 12.f});

    // Load textures
    map<char, sf::Texture> textures;
    map<char, string> files = {
        {'K', "pieces/wK.png"}, {'Q', "pieces/wQ.png"},
        {'R', "pieces/wR.png"}, {'B', "pieces/wB.png"},
        {'N', "pieces/wN.png"}, {'P', "pieces/wP.png"},
        {'k', "pieces/bK.png"}, {'q', "pieces/bQ.png"},
        {'r', "pieces/bR.png"}, {'b', "pieces/bB.png"},
        {'n', "pieces/bN.png"}, {'p', "pieces/bP.png"}
    };

    for (auto& [ch, path] : files) {
        if (!textures[ch].loadFromFile(path)) return -1;
    }

    initialiseboard();

    // Selection state
    bool pieceSelected = false;
    int selectedRow = -1, selectedCol = -1;
    vector<Move> legalMoves;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    int col = mouseEvent->position.x / TILE_SIZE;
                    int row = mouseEvent->position.y / TILE_SIZE;

                    if (row < 0 || row >= 8 || col < 0 || col >= 8) continue;

                    char clicked = board[row][col];

                    if (!pieceSelected) {
                        // Select a piece
                        if (clicked != '.' &&
                            (whitetomove ? isupper(clicked) : islower(clicked))) {
                            pieceSelected = true;
                            selectedRow = row;
                            selectedCol = col;
                            // Get legal moves for this piece
                            legalMoves.clear();
                            vector<Move> all = generateLegalMoves(whitetomove);
                            for (auto& m : all)
                                if (m.fr == row && m.fc == col)
                                    legalMoves.push_back(m);
                        }
                    } else {
                        // Try to move
                        bool moved = false;
                        for (auto& m : legalMoves) {
                            if (m.tr == row && m.tc == col) {
                                make_Move(m);
                            if (ispromotion(m)) promotePawn(m);
                                whitetomove = !whitetomove;  // now black's turn

                         // AI plays black
                            if (!whitetomove) {
                                Move aiMove = getBestMove(false, 3);
                                make_Move(aiMove);
                            if (ispromotion(aiMove)) promotePawn(aiMove);
                                whitetomove = !whitetomove;  // back to white
                    }

        moved = true;
        break;
    }
}

                        // Deselect or select new piece
                        pieceSelected = false;
                        selectedRow = -1;
                        selectedCol = -1;
                        legalMoves.clear();

                        // If clicked own piece without moving, select it
                        if (!moved && clicked != '.' &&
                            (whitetomove ? isupper(clicked) : islower(clicked))) {
                            pieceSelected = true;
                            selectedRow = row;
                            selectedCol = col;
                            vector<Move> all = generateLegalMoves(whitetomove);
                            for (auto& m : all)
                                if (m.fr == row && m.fc == col)
                                    legalMoves.push_back(m);
                        }
                    }
                }
            }
        }

        window.clear();

        // Draw board
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                bool isLight = (row + col) % 2 == 0;
                tile.setFillColor(isLight ? light : dark);
                tile.setPosition({(float)(col * TILE_SIZE), (float)(row * TILE_SIZE)});
                window.draw(tile);
            }
        }

        // Draw selected highlight
        if (pieceSelected) {
            highlightTile.setPosition({(float)(selectedCol * TILE_SIZE),
                                       (float)(selectedRow * TILE_SIZE)});
            window.draw(highlightTile);

            // Draw legal move dots
            for (auto& m : legalMoves) {
                dot.setPosition({m.tc * TILE_SIZE + TILE_SIZE / 2.f,
                                 m.tr * TILE_SIZE + TILE_SIZE / 2.f});
                window.draw(dot);
            }
        }

        // Draw pieces
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                char piece = board[row][col];
                if (piece != '.') {
                    sf::Sprite sprite(textures[piece]);
                    sf::Vector2u texSize = textures[piece].getSize();
                    float scaleX = (float)TILE_SIZE / texSize.x;
                    float scaleY = (float)TILE_SIZE / texSize.y;
                    sprite.setScale({scaleX, scaleY});
                    sprite.setPosition({(float)(col * TILE_SIZE),
                                        (float)(row * TILE_SIZE)});
                    window.draw(sprite);
                }
            }
        }

        window.display();
    }

    return 0;
}