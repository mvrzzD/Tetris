#include "Piece.h"

Piece::Piece(int type, int startX, int startY)
    : type(type), posX(startX), posY(startY)
{
    initForme();
}

void Piece::initForme() {
    switch (type) {
        case 0: // O — Carré
            blocs   = { {0,0},{1,0},{0,1},{1,1} };
            couleur = sf::Color(255, 213, 0);   // Jaune
            break;
        case 1: // I — Bâton
            blocs   = { {-1,0},{0,0},{1,0},{2,0} };
            couleur = sf::Color(0, 220, 220);   // Cyan
            break;
        case 2: // T
            blocs   = { {0,-1},{-1,0},{0,0},{1,0} };
            couleur = sf::Color(160, 0, 200);   // Violet
            break;
        case 3: // L
            blocs   = { {-1,-1},{-1,0},{0,0},{1,0} };
            couleur = sf::Color(255, 140, 0);   // Orange
            break;
        case 4: // J
            blocs   = { {1,-1},{-1,0},{0,0},{1,0} };
            couleur = sf::Color(0, 80, 220);    // Bleu
            break;
        case 5: // S
            blocs   = { {0,-1},{1,-1},{-1,0},{0,0} };
            couleur = sf::Color(0, 200, 60);    // Vert
            break;
        case 6: // Z
            blocs   = { {-1,-1},{0,-1},{0,0},{1,0} };
            couleur = sf::Color(220, 30, 30);   // Rouge
            break;
        default:
            blocs   = { {0,0},{1,0},{0,1},{1,1} };
            couleur = sf::Color::White;
            break;
    }
}

// Rotation 90° horaire : (x,y) → (y,−x)
void Piece::rotate() {
    if (type == 0) return; // Le carré ne tourne pas
    std::vector<Block> r;
    for (const Block& b : blocs)
        r.push_back({ b.y, -b.x });
    blocs = r;
}

void Piece::move(int dx, int dy) {
    posX += dx;
    posY += dy;
}

std::vector<Block> Piece::getBlocks() const {
    std::vector<Block> abs;
    for (const Block& b : blocs)
        abs.push_back({ posX + b.x, posY + b.y });
    return abs;
}

std::vector<Block> Piece::getRelativeBlocs() const { return blocs; }
void               Piece::setBlocs(const std::vector<Block>& b) { blocs = b; }
sf::Color          Piece::getColor() const { return couleur; }
int                Piece::getType()  const { return type; }
int                Piece::getPosX()  const { return posX; }
int                Piece::getPosY()  const { return posY; }
