#pragma once

#include <SFML/Graphics.hpp>
#include "Piece.h"

// Dimensions du plateau (à adapter si déjà défini ailleurs)
const int ROWS = 20;
const int COLS = 10;
const int TILE = 28;

class Board {
private:
    sf::Color grille[ROWS][COLS];

    int score;
    int niveau;
    int lignesDetruites;

    bool estVide(int row, int col) const;

public:
    Board();

    void reset();

    bool isValid(const Piece& piece) const;
    void placerPiece(const Piece& piece);
    int supprimerLignes();
    bool isGameOver(const Piece& piece) const;

    void draw(sf::RenderWindow& window, int offsetX, int offsetY) const;

    int getScore() const;
    int getNiveau() const;
    int getLignes() const;
};

