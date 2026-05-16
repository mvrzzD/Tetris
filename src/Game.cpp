#include "Game.h"
#include <random>
#include <algorithm>
#include <cstdint>

// ─────────────────────────────────────────────
//  Helper — effet 3D sur un bloc
// ─────────────────────────────────────────────
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

// ─────────────────────────────────────────────
//  Constructeur
// ─────────────────────────────────────────────
Game::Game(sf::RenderWindow& window)
    : window(window),
      ui(COLS * TILE),
      pieceActuelle(Piece(0, 3, 0)),
      pieceSuivante(Piece(0, 3, 0)),
      etat(WELCOME),
      vitesse(TRES_LENT),
      modeEvolutif(false),
      niveauPrecedent(1),
      tempsEcoule(0.f),
      timerVerrouillage(0.f),
      enVerrouillage(false),
      rng(std::random_device{}()),
      dist(0, 6)
{
    pieceActuelle = genererPiece();
    pieceSuivante = genererPiece();

    audio.jouerMusiqueMenu();
    clock.restart();
}

// ─────────────────────────────────────────────
//  Génération aléatoire
// ─────────────────────────────────────────────
Piece Game::genererPiece() {
    return Piece(dist(rng), 3, 0);
}

// ─────────────────────────────────────────────
//  Reset complet
// ─────────────────────────────────────────────
void Game::resetPartie() {
    board.reset();
    pieceActuelle     = genererPiece();
    pieceSuivante     = genererPiece();
    tempsEcoule       = 0.f;
    timerVerrouillage = 0.f;
    enVerrouillage    = false;
    niveauPrecedent   = 1;
    clock.restart();
}

// ─────────────────────────────────────────────
//  Boucle principale
// ─────────────────────────────────────────────
void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

// ─────────────────────────────────────────────
//  Gestion des événements
// ─────────────────────────────────────────────
void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (etat == WELCOME) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                etat = MODE_SELECTION;
                audio.jouerMenuValid();
                clock.restart();
            }
        }
        else if (etat == MODE_SELECTION) {
            GameState suivant = menu.handleModeSelection(event, window, vitesse, modeEvolutif);
            if (suivant == PLAYING) {
                audio.jouerMenuValid();
                resetPartie();
                if (modeEvolutif) audio.jouerMusiqueTypeB();
                else audio.jouerMusiqueTypeA();
            }
            etat = suivant;
        }
        else if (etat == PAUSE) {
            GameState suivant = menu.handlePause(event, window);
            if (suivant == PLAYING) {
                audio.jouerMenuValid();
                audio.resumeMusique();
                clock.restart();
            }
            else if (suivant == RESTART) {
                audio.jouerMenuValid();
                resetPartie();
                if (modeEvolutif) audio.jouerMusiqueTypeB();
                else audio.jouerMusiqueTypeA();
                clock.restart();
                suivant = PLAYING; 
            }
            else if (suivant == MODE_SELECTION) {
                audio.jouerMenuValid();
                resetPartie();
                audio.jouerMusiqueMenu();
            }
            etat = suivant;
        }
        else if (etat == GAME_OVER) {
            GameState suivant = menu.handleGameOver(event, window);
            if (suivant == RESTART) {
                audio.jouerMenuValid();
                resetPartie();
                if (modeEvolutif) audio.jouerMusiqueTypeB();
                else audio.jouerMusiqueTypeA();
                clock.restart();
                suivant = PLAYING;
            }
            else if (suivant == MODE_SELECTION) {
                audio.jouerMenuValid();
                resetPartie();
                audio.jouerMusiqueMenu();
            }
            etat = suivant;
        }
        else if (etat == PLAYING) {
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Left:
                        pieceActuelle.move(-1, 0);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(1, 0);
                        else {
                            audio.jouerDeplacement();
                            if (enVerrouillage) timerVerrouillage = 0.f; // Reset delay
                        }
                        break;
                    case sf::Keyboard::Right:
                        pieceActuelle.move(1, 0);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(-1, 0);
                        else {
                            audio.jouerDeplacement();
                            if (enVerrouillage) timerVerrouillage = 0.f;
                        }
                        break;
                    case sf::Keyboard::Down:
                        pieceActuelle.move(0, 1);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(0, -1);
                        else {
                            audio.jouerDeplacement();
                            tempsEcoule = 0.f; // Reset fall timer
                        }
                        break;
                    case sf::Keyboard::Up: {
                        auto sauvBlocs = pieceActuelle.getRelativeBlocs();
                        int  sauvX     = pieceActuelle.getPosX();
                        int  sauvY     = pieceActuelle.getPosY();

                        pieceActuelle.rotate();

                        // Système de "Wall Kick" (décalage si rotation contre un mur)
                        bool valide = false;
                        int decalages[] = {0, 1, -1, 2, -2}; // Essayer sur place, puis décalages latéraux

                        for (int dx : decalages) {
                            pieceActuelle.move(dx, 0);
                            if (board.isValid(pieceActuelle)) {
                                valide = true;
                                break;
                            }
                            pieceActuelle.move(-dx, 0); // Revenir si pas valide
                        }

                        if (!valide) {
                            pieceActuelle.setBlocs(sauvBlocs);
                        } else {
                            audio.jouerRotation();
                            if (enVerrouillage) timerVerrouillage = 0.f;
                        }
                        break;
                    }
                    case sf::Keyboard::Space: {
                        while (board.isValid(pieceActuelle)) pieceActuelle.move(0, 1);
                        pieceActuelle.move(0, -1);
                        board.placerPiece(pieceActuelle);
                        audio.jouerPose();
                        
                        int nbLignes = board.supprimerLignes();
                        if (nbLignes == 4) audio.jouerTetris();
                        else if (nbLignes > 0) audio.jouerLigne();
                        
                        pieceActuelle = pieceSuivante;
                        pieceSuivante = genererPiece();
                        if (board.isGameOver(pieceActuelle)) {
                            etat = GAME_OVER;
                            audio.jouerGameOver();
                        }
                        tempsEcoule = 0.f;
                        enVerrouillage = false;
                        break;
                    }
                    case sf::Keyboard::Escape:
                        etat = PAUSE;
                        menu.resetSelectionPause();
                        audio.pauseMusique();
                        break;
                    default: break;
                }
            }
        }
    }
}

// ─────────────────────────────────────────────
//  Mise à jour
// ─────────────────────────────────────────────
void Game::update() {
    float deltaMs = clock.restart().asSeconds() * 1000.f;

    if (etat != PLAYING)
        return;

    board.updateAnimation(deltaMs);

    if (modeEvolutif) {
        int niveauActuel = board.getNiveau();
        if (niveauActuel != niveauPrecedent) {
            audio.updatePitchEvolutif(niveauActuel);
            niveauPrecedent = niveauActuel;
        }
    }

    int delai = modeEvolutif
        ? std::max(100, TRES_LENT - (board.getNiveau() - 1) * 60)
        : vitesse;

    tempsEcoule += deltaMs;

    // Logique de descente
    if (tempsEcoule >= static_cast<float>(delai)) {
        tempsEcoule = 0.f;
        pieceActuelle.move(0, 1);

        if (!board.isValid(pieceActuelle)) {
            pieceActuelle.move(0, -1);
            enVerrouillage = true;
        } else {
            enVerrouillage = false;
            timerVerrouillage = 0.f;
        }
    }

    // Logique de verrouillage (Lock Delay)
    if (enVerrouillage) {
        timerVerrouillage += deltaMs;
        if (timerVerrouillage >= 500.f) { // 500ms de délai
            board.placerPiece(pieceActuelle);
            audio.jouerPose();
            
            int nbLignes = board.supprimerLignes();
            if (nbLignes == 4) audio.jouerTetris();
            else if (nbLignes > 0) audio.jouerLigne();
            
            pieceActuelle = pieceSuivante;
            pieceSuivante = genererPiece();
            if (board.isGameOver(pieceActuelle)) {
                etat = GAME_OVER;
                audio.jouerGameOver();
            }
            enVerrouillage = false;
            timerVerrouillage = 0.f;
        }
    }
}

// ─────────────────────────────────────────────
//  Rendu
// ─────────────────────────────────────────────
void Game::render() {
    window.clear(sf::Color(245, 245, 245)); // Beige

    if (etat == WELCOME) {
        menu.drawWelcome(window);
    }
    else if (etat == MODE_SELECTION) {
        menu.drawModeSelection(window);
    }
    else {
        renderJeu();

        if (etat == PAUSE)
            menu.drawPause(window);

        if (etat == GAME_OVER)
            menu.drawGameOver(window,
                              board.getScore(),
                              board.getNiveau(),
                              modeEvolutif);
    }

    window.display(); // ← UNE SEULE FOIS
}

// ─────────────────────────────────────────────
//  Rendu jeu
// ─────────────────────────────────────────────
void Game::renderJeu() {
    window.clear(sf::Color(245, 245, 245));

    board.draw(window, 0, 0);
    drawGhostPiece();

    for (const Block& b : pieceActuelle.getBlocks()) {
        if (b.y >= 0)
            drawFlatBlock(window,
                        b.x * TILE,
                        b.y * TILE,
                        pieceActuelle.getColor());
    }

    ui.draw(window,
            board.getScore(),
            board.getHighScore(),
            board.getNiveau(),
            modeEvolutif,
            pieceSuivante);
}

void Game::drawGhostPiece() {
    Piece ghost = pieceActuelle;
    while (board.isValid(ghost)) {
        ghost.move(0, 1);
    }
    ghost.move(0, -1);

    sf::Color c = pieceActuelle.getColor();
    c.a = 60; // Semi-transparent

    for (const Block& b : ghost.getBlocks()) {
        if (b.y >= 0) {
            drawFlatBlock(window, b.x * TILE, b.y * TILE, c);
        }
    }
}
