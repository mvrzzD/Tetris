#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Structure repr�sentant un bloc
struct Block {
    int x;
    int y;
};

class Piece {
private:
    int type;
    int posX;
    int posY;

    std::vector<Block> blocs;
    sf::Color couleur;

    void initForme();

public:
    Piece(int type, int startX, int startY);

    void rotate();
    void move(int dx, int dy);

    std::vector<Block> getBlocks() const;
    std::vector<Block> getRelativeBlocs() const;

    void setBlocs(const std::vector<Block>& b);

    sf::Color getColor() const;

    int getType() const;
    int getPosX() const;
    int getPosY() const;
};

