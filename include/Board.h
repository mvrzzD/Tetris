#pragma once

#include <SFML/Graphics.hpp>
#include "Piece.h"

// Dimensions du plateau (à adapter si déjà défini ailleurs)
const int ROWS = 20;
const int COLS = 10;
const int TILE = 44;

class Board {
private:
    sf::Color grille[ROWS][COLS];

    int score;
    int highScore;
    int niveau;
    int lignesDetruites;

    std::vector<int> lignesAAnimer;
    float timerAnimation;

    bool estVide(int row, int col) const;
    void chargerHighScore();
    void sauvegarderHighScore();

public:
    Board();

    void reset();

    bool isValid(const Piece& piece) const;
    void placerPiece(const Piece& piece);
    int  supprimerLignes();
    void updateAnimation(float deltaMs);
    bool isAnimating() const { return !lignesAAnimer.empty(); }

    bool isGameOver(const Piece& piece) const;

    void draw(sf::RenderWindow& window, int offsetX, int offsetY) const;

    int getScore()     const;
    int getHighScore() const;
    int getNiveau()    const;
    int getLignes()    const;
};

