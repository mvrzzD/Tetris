#include "Game.h"
#include <random>
#include <algorithm>
#include <cstdint>

// ─────────────────────────────────────────────
//  Helper — effet 3D sur un bloc
// ─────────────────────────────────────────────
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
        std::min(255, couleur.r + 80),
        std::min(255, couleur.g + 80),
        std::min(255, couleur.b + 80)
    );

    sf::RectangleShape bordHaut(sf::Vector2f(taille - 1.f, bevel));
    bordHaut.setPosition({x, y});
    bordHaut.setFillColor(clair);
    window.draw(bordHaut);

    sf::RectangleShape bordGauche(sf::Vector2f(bevel, taille - 1.f));
    bordGauche.setPosition({x, y});
    bordGauche.setFillColor(clair);
    window.draw(bordGauche);

    sf::Color sombre(
        std::max(0, couleur.r - 80),
        std::max(0, couleur.g - 80),
        std::max(0, couleur.b - 80)
    );

    sf::RectangleShape bordBas(sf::Vector2f(taille - 1.f, bevel));
    bordBas.setPosition({x, y + taille - 1.f - bevel});
    bordBas.setFillColor(sombre);
    window.draw(bordBas);

    sf::RectangleShape bordDroite(sf::Vector2f(bevel, taille - 1.f));
    bordDroite.setPosition({x + taille - 1.f - bevel, y});
    bordDroite.setFillColor(sombre);
    window.draw(bordDroite);
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
    pieceActuelle   = genererPiece();
    pieceSuivante   = genererPiece();
    tempsEcoule     = 0.f;
    niveauPrecedent = 1;
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
            GameState suivant = menu.handleWelcome(event);
            if (suivant == MODE_SELECTION) audio.jouerMenuValid();
            etat = suivant;
        }
        else if (etat == MODE_SELECTION) {
            GameState suivant = menu.handleModeSelection(event, vitesse, modeEvolutif);
            if (suivant == PLAYING) {
                audio.jouerMenuValid();
                resetPartie();
                audio.jouerMusiqueJeu();
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
            if (suivant == MODE_SELECTION)
            {
                audio.jouerMenuValid();
                resetPartie();
                audio.jouerMusiqueMenu();
            }
            etat = suivant;
        }
        else if (etat == GAME_OVER) {
            GameState suivant = menu.handleGameOver(event, window);
            if (suivant == WELCOME) {
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
                        break;
                    case sf::Keyboard::Right:
                        pieceActuelle.move(1, 0);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(-1, 0);
                        break;
                    case sf::Keyboard::Down:
                        pieceActuelle.move(0, 1);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(0, -1);
                        break;
                    case sf::Keyboard::Up: {
                        auto sauv = pieceActuelle.getRelativeBlocs();
                        pieceActuelle.rotate();
                        if (!board.isValid(pieceActuelle))
                            pieceActuelle.setBlocs(sauv);
                        break;
                    }
                    case sf::Keyboard::Space: {
                        while (board.isValid(pieceActuelle)) pieceActuelle.move(0, 1);
                        pieceActuelle.move(0, -1);
                        board.placerPiece(pieceActuelle);
                        board.supprimerLignes();
                        pieceActuelle = pieceSuivante;
                        pieceSuivante = genererPiece();
                        if (board.isGameOver(pieceActuelle))
                            etat = GAME_OVER;
                        tempsEcoule = 0.f;
                        break;
                    }
                    case sf::Keyboard::Escape:
                        etat = PAUSE;
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
//  Correction : UN SEUL clock.restart() par frame
//  mesuré en secondes puis converti en ms
// ─────────────────────────────────────────────
void Game::update() {
    // On mesure le delta UNE SEULE FOIS, toujours
    float deltaMs = clock.restart().asSeconds() * 1000.f;

    if (etat != PLAYING)
        return;  // on a quand même consommé le clock, pas de double restart

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

    if (tempsEcoule >= static_cast<float>(delai)) {
        tempsEcoule = 0.f;
        pieceActuelle.move(0, 1);

        if (!board.isValid(pieceActuelle)) {
            pieceActuelle.move(0, -1);
            board.placerPiece(pieceActuelle);
            board.supprimerLignes();
            pieceActuelle = pieceSuivante;
            pieceSuivante = genererPiece();
            if (board.isGameOver(pieceActuelle))
                etat = GAME_OVER;
        }
    }
}

// ─────────────────────────────────────────────
//  Rendu
// ─────────────────────────────────────────────
void Game::render() {
    window.clear(sf::Color(15, 15, 15)); // ← OBLIGATOIRE

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
    window.clear(sf::Color(15, 15, 15));

    board.draw(window, 0, 0);

    for (const Block& b : pieceActuelle.getBlocks()) {
        if (b.y >= 0)
            draw3DBlock(window,
                        b.x * TILE,
                        b.y * TILE,
                        pieceActuelle.getColor());
    }

    ui.draw(window,
            board.getScore(),
            board.getNiveau(),
            modeEvolutif,
            pieceSuivante);
}
