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

    // G�nérateur aléatoire moderne (meilleur que rand())
    std::mt19937           rng;
    std::vector<int>       sacPieces;
    std::size_t            indiceSac;

    bool leftHeld;
    bool rightHeld;
    bool downHeld;
    float leftHoldTimer;
    float rightHoldTimer;
    float downHoldTimer;
    float leftHoldDelay;
    float rightHoldDelay;
    float downHoldDelay;

    Piece  genererPiece();
    void   remplirSac();
    void   resetPartie();
    bool   deplacerPiece(int dx, int dy);
    bool   rotationAvecKick();
    void   handleInputHold(float deltaMs);

    void handleEvents();
    void update();
    void render();
    void renderJeu();
    void drawGhostPiece();

public:
    Game(sf::RenderWindow& window);
    void run();
};
