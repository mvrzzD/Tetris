#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Piece.h"

class UI {
private:
    sf::Font font;
    bool fontChargee;

    int offsetX;

    void drawTexte(sf::RenderWindow& window, const std::string& texte,
                   float x, float y, int taille,
                   sf::Color couleur = sf::Color::White) const;

public:
    UI(int offsetX);

    void draw(sf::RenderWindow& window,
              int score, int highScore, int niveau, bool modeEvolutif,
              const Piece& pieceSuivante) const;
};


