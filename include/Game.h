#pragma once
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Piece.h"
#include "Menu.h"
#include "UI.h"
#include "Audio.h"
#include <random>

class Game {
private:
    sf::RenderWindow& window;
    Board  board;
    Menu   menu;
    UI     ui;
    Audio  audio;

    Piece pieceActuelle;
    Piece pieceSuivante;

    GameState etat;
    int       vitesse;
    bool      modeEvolutif;
    int       niveauPrecedent;

    sf::Clock clock;
    float     tempsEcoule;
    float     timerVerrouillage;
    bool      enVerrouillage;

    // Générateur aléatoire moderne (meilleur que rand())
    std::mt19937                    rng;
    std::uniform_int_distribution<> dist;

    Piece genererPiece();
    void  resetPartie();

    void handleEvents();
    void update();
    void render();
    void renderJeu();
    void drawGhostPiece();

public:
    Game(sf::RenderWindow& window);
    void run();
};
