#include "Board.h"
#include <algorithm>
#include <cstdint>

static void draw3DBlock(sf::RenderWindow& window,
                        float x, float y,
                        sf::Color couleur,
                        float taille = static_cast<float>(TILE))
{
    const float bevel = taille * 0.18f;

    sf::RectangleShape face(sf::Vector2f(taille - 1.f, taille - 1.f));
    face.setPosition(sf::Vector2f(x, y));
    face.setFillColor(couleur);
    window.draw(face);

    sf::Color clair(
        static_cast<uint8_t>(std::min(255, (int)couleur.r + 80)),
        static_cast<uint8_t>(std::min(255, (int)couleur.g + 80)),
        static_cast<uint8_t>(std::min(255, (int)couleur.b + 80))
    );
    sf::RectangleShape bordHaut(sf::Vector2f(taille - 1.f, bevel));
    bordHaut.setPosition(sf::Vector2f(x, y));
    bordHaut.setFillColor(clair);
    window.draw(bordHaut);

    sf::RectangleShape bordGauche(sf::Vector2f(bevel, taille - 1.f));
    bordGauche.setPosition(sf::Vector2f(x, y));
    bordGauche.setFillColor(clair);
    window.draw(bordGauche);

    sf::Color sombre(
        static_cast<uint8_t>(std::max(0, (int)couleur.r - 80)),
        static_cast<uint8_t>(std::max(0, (int)couleur.g - 80)),
        static_cast<uint8_t>(std::max(0, (int)couleur.b - 80))
    );
    sf::RectangleShape bordBas(sf::Vector2f(taille - 1.f, bevel));
    bordBas.setPosition(sf::Vector2f(x, y + taille - 1.f - bevel));
    bordBas.setFillColor(sombre);
    window.draw(bordBas);

    sf::RectangleShape bordDroite(sf::Vector2f(bevel, taille - 1.f));
    bordDroite.setPosition(sf::Vector2f(x + taille - 1.f - bevel, y));
    bordDroite.setFillColor(sombre);
    window.draw(bordDroite);
}

Board::Board() { reset(); }

void Board::reset() {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grille[r][c] = sf::Color::Black;
    score           = 0;
    niveau          = 1;
    lignesDetruites = 0;
}

bool Board::estVide(int row, int col) const {
    return grille[row][col] == sf::Color::Black;
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
    for (int r = ROWS - 1; r >= 0; r--) {
        bool complete = true;
        for (int c = 0; c < COLS; c++)
            if (estVide(r, c)) { complete = false; break; }
        if (complete) {
            compteur++;
            for (int row = r; row > 0; row--)
                for (int c = 0; c < COLS; c++)
                    grille[row][c] = grille[row - 1][c];
            for (int c = 0; c < COLS; c++)
                grille[0][c] = sf::Color::Black;
            r++;
        }
    }
    static const int points[] = { 0, 100, 300, 500, 800 };
    if (compteur >= 1 && compteur <= 4)
        score += points[compteur] * niveau;
    lignesDetruites += compteur;
    niveau = (lignesDetruites / 10) + 1;
    return compteur;
}

bool Board::isGameOver(const Piece& piece) const {
    return !isValid(piece);
}

void Board::draw(sf::RenderWindow& window, int offsetX, int offsetY) const {
    sf::RectangleShape cell(sf::Vector2f(TILE - 1.f, TILE - 1.f));

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            float px = static_cast<float>(offsetX + c * TILE);
            float py = static_cast<float>(offsetY + r * TILE);
            if (estVide(r, c)) {
                cell.setPosition(sf::Vector2f(px, py));
                cell.setFillColor(sf::Color(30, 30, 30));
                window.draw(cell);
            } else {
                draw3DBlock(window, px, py, grille[r][c]);
            }
        }
    }

    sf::RectangleShape bordure(sf::Vector2f(
        static_cast<float>(COLS * TILE),
        static_cast<float>(ROWS * TILE)));
    bordure.setPosition(sf::Vector2f(
        static_cast<float>(offsetX),
        static_cast<float>(offsetY)));
    bordure.setFillColor(sf::Color::Transparent);
    bordure.setOutlineColor(sf::Color(80, 80, 80));
    bordure.setOutlineThickness(2);
    window.draw(bordure);
}

int Board::getScore()  const { return score; }
int Board::getNiveau() const { return niveau; }
int Board::getLignes() const { return lignesDetruites; }
