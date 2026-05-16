#include "Board.h"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <vector>

static void drawFlatBlock(sf::RenderWindow& window,
                         float x, float y,
                         sf::Color couleur,
                         float taille = static_cast<float>(TILE))
{
    sf::RectangleShape face(sf::Vector2f(taille - 1.f, taille - 1.f));
    face.setPosition(sf::Vector2f(x, y));
    face.setFillColor(couleur);
    window.draw(face);
}

Board::Board() : highScore(0), timerAnimation(0.f) { 
    chargerHighScore();
    reset(); 
}

void Board::chargerHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

void Board::sauvegarderHighScore() {
    if (score > highScore) {
        highScore = score;
        std::ofstream file("highscore.txt");
        if (file.is_open()) {
            file << highScore;
            file.close();
        }
    }
}

void Board::reset() {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grille[r][c] = sf::Color::Transparent;
    score           = 0;
    niveau          = 1;
    lignesDetruites = 0;
    lignesAAnimer.clear();
    timerAnimation = 0.f;
}

bool Board::estVide(int row, int col) const {
    return grille[row][col] == sf::Color::Transparent;
}

bool Board::isValid(const Piece& piece) const {
    for (const Block& b : piece.getBlocks()) {
        if (b.x < 0 || b.x >= COLS || b.y >= ROWS) return false;
        if (b.y < 0) continue;
        if (!estVide(b.y, b.x)) return false;
    }
    return true;
}

void Board::placerPiece(const Piece& piece) {
    for (const Block& b : piece.getBlocks())
        if (b.y >= 0 && b.y < ROWS && b.x >= 0 && b.x < COLS)
            grille[b.y][b.x] = piece.getColor();
}

int Board::supprimerLignes() {
    int compteur = 0;
    std::vector<int> aSupprimer;

    for (int r = ROWS - 1; r >= 0; r--) {
        bool complete = true;
        for (int c = 0; c < COLS; c++)
            if (estVide(r, c)) { complete = false; break; }
        
        if (complete) {
            compteur++;
            aSupprimer.push_back(r);
        }
    }

    if (compteur > 0) {
        lignesAAnimer = aSupprimer;
        timerAnimation = 200.f; // 200ms de flash

        for (int r : aSupprimer) {
            for (int row = r; row > 0; row--)
                for (int c = 0; c < COLS; c++)
                    grille[row][c] = grille[row - 1][c];
            for (int c = 0; c < COLS; c++)
                grille[0][c] = sf::Color::Transparent;
            
            // On ajuste les indices car tout a bougé
            for (int& otherR : aSupprimer) if (otherR < r) otherR++;
        }

        static const int points[] = { 0, 100, 300, 500, 800 };
        if (compteur >= 1 && compteur <= 4)
            score += points[compteur] * niveau;
        
        lignesDetruites += compteur;
        niveau = (lignesDetruites / 10) + 1;
        sauvegarderHighScore();
    }

    return compteur;
}

void Board::updateAnimation(float deltaMs) {
    if (timerAnimation > 0.f) {
        timerAnimation -= deltaMs;
        if (timerAnimation <= 0.f) {
            lignesAAnimer.clear();
        }
    }
}

bool Board::isGameOver(const Piece& piece) const {
    return !isValid(piece);
}

void Board::draw(sf::RenderWindow& window, int offsetX, int offsetY) const {
    // Fond du plateau beige
    sf::RectangleShape background(sf::Vector2f(COLS * TILE, ROWS * TILE));
    background.setPosition(sf::Vector2f(offsetX, offsetY));
    background.setFillColor(sf::Color(245, 245, 245));
    window.draw(background);

    sf::RectangleShape cell(sf::Vector2f(TILE - 1.f, TILE - 1.f));

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            float px = static_cast<float>(offsetX + c * TILE);
            float py = static_cast<float>(offsetY + r * TILE);
            
            if (estVide(r, c)) {
                cell.setPosition(sf::Vector2f(px, py));
                cell.setFillColor(sf::Color(235, 235, 235)); // Grille légère
                window.draw(cell);
            } else {
                drawFlatBlock(window, px, py, grille[r][c]);
            }
        }
    }

    // Animation de flash pour les lignes supprimées
    if (timerAnimation > 0.f) {
        sf::RectangleShape flash(sf::Vector2f(COLS * TILE, TILE));
        flash.setFillColor(sf::Color(255, 255, 255, 180));
        for (int r : lignesAAnimer) {
            flash.setPosition(offsetX, offsetY + r * TILE);
            window.draw(flash);
        }
    }

    sf::RectangleShape bordure(sf::Vector2f(
        static_cast<float>(COLS * TILE),
        static_cast<float>(ROWS * TILE)));
    bordure.setPosition(sf::Vector2f(
        static_cast<float>(offsetX),
        static_cast<float>(offsetY)));
    bordure.setFillColor(sf::Color::Transparent);
    bordure.setOutlineColor(sf::Color(200, 200, 200));
    bordure.setOutlineThickness(1);
    window.draw(bordure);
}

int Board::getScore()      const { return score; }
int Board::getHighScore()  const { return highScore; }
int Board::getNiveau()     const { return niveau; }
int Board::getLignes()     const { return lignesDetruites; }
