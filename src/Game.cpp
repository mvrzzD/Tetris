#include "Game.h"
#include <random>
#include <algorithm>
#include <cstdint>

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

Piece Game::genererPiece() {
    return Piece(dist(rng), 3, 0);
}

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

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    // SFML 3 : pollEvent() retourne un optional
    while (auto optEvent = window.pollEvent()) {
        auto& event = *optEvent;

        if (event.is<sf::Event::Closed>())
            window.close();

        if (etat == WELCOME) {
            if (auto* key = event.getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Enter) {
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
                else              audio.jouerMusiqueTypeA();
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
                else              audio.jouerMusiqueTypeA();
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
                else              audio.jouerMusiqueTypeA();
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
            if (auto* key = event.getIf<sf::Event::KeyPressed>()) {
                switch (key->code) {
                    case sf::Keyboard::Key::Left:
                        pieceActuelle.move(-1, 0);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(1, 0);
                        else {
                            audio.jouerDeplacement();
                            if (enVerrouillage) timerVerrouillage = 0.f;
                        }
                        break;
                    case sf::Keyboard::Key::Right:
                        pieceActuelle.move(1, 0);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(-1, 0);
                        else {
                            audio.jouerDeplacement();
                            if (enVerrouillage) timerVerrouillage = 0.f;
                        }
                        break;
                    case sf::Keyboard::Key::Down:
                        pieceActuelle.move(0, 1);
                        if (!board.isValid(pieceActuelle)) pieceActuelle.move(0, -1);
                        else {
                            audio.jouerDeplacement();
                            tempsEcoule = 0.f;
                        }
                        break;
                    case sf::Keyboard::Key::Up: {
                        auto sauvBlocs = pieceActuelle.getRelativeBlocs();
                        pieceActuelle.rotate();

                        bool valide = false;
                        int decalages[] = {0, 1, -1, 2, -2};
                        for (int dx : decalages) {
                            pieceActuelle.move(dx, 0);
                            if (board.isValid(pieceActuelle)) {
                                valide = true;
                                break;
                            }
                            pieceActuelle.move(-dx, 0);
                        }
                        if (!valide) pieceActuelle.setBlocs(sauvBlocs);
                        else {
                            audio.jouerRotation();
                            if (enVerrouillage) timerVerrouillage = 0.f;
                        }
                        break;
                    }
                    case sf::Keyboard::Key::Space: {
                        while (board.isValid(pieceActuelle)) pieceActuelle.move(0, 1);
                        pieceActuelle.move(0, -1);
                        board.placerPiece(pieceActuelle);
                        audio.jouerPose();
                        int nbLignes = board.supprimerLignes();
                        if (nbLignes == 4)    audio.jouerTetris();
                        else if (nbLignes > 0) audio.jouerLigne();
                        pieceActuelle = pieceSuivante;
                        pieceSuivante = genererPiece();
                        if (board.isGameOver(pieceActuelle)) {
                            etat = GAME_OVER;
                            audio.jouerGameOver();
                        }
                        tempsEcoule    = 0.f;
                        enVerrouillage = false;
                        break;
                    }
                    case sf::Keyboard::Key::Escape:
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

void Game::update() {
    float deltaMs = clock.restart().asSeconds() * 1000.f;

    if (etat != PLAYING) return;

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

    if (tempsEcoule >= static_cast<float>(delai)) {
        tempsEcoule = 0.f;
        pieceActuelle.move(0, 1);
        if (!board.isValid(pieceActuelle)) {
            pieceActuelle.move(0, -1);
            enVerrouillage = true;
        } else {
            enVerrouillage    = false;
            timerVerrouillage = 0.f;
        }
    }

    if (enVerrouillage) {
        timerVerrouillage += deltaMs;
        if (timerVerrouillage >= 500.f) {
            board.placerPiece(pieceActuelle);
            audio.jouerPose();
            int nbLignes = board.supprimerLignes();
            if (nbLignes == 4)    audio.jouerTetris();
            else if (nbLignes > 0) audio.jouerLigne();
            pieceActuelle = pieceSuivante;
            pieceSuivante = genererPiece();
            if (board.isGameOver(pieceActuelle)) {
                etat = GAME_OVER;
                audio.jouerGameOver();
            }
            enVerrouillage    = false;
            timerVerrouillage = 0.f;
        }
    }
}

void Game::render() {
    window.clear(sf::Color(245, 245, 245));

    if (etat == WELCOME)
        menu.drawWelcome(window);
    else if (etat == MODE_SELECTION)
        menu.drawModeSelection(window);
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
    window.display();
}

void Game::renderJeu() {
    window.clear(sf::Color(245, 245, 245));
    board.draw(window, 0, 0);
    drawGhostPiece();

    for (const Block& b : pieceActuelle.getBlocks()) {
        if (b.y >= 0)
            drawFlatBlock(window,
                         static_cast<float>(b.x * TILE),
                         static_cast<float>(b.y * TILE),
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
    while (board.isValid(ghost)) ghost.move(0, 1);
    ghost.move(0, -1);

    sf::Color c = pieceActuelle.getColor();
    c.a = 60;

    for (const Block& b : ghost.getBlocks()) {
        if (b.y >= 0)
            drawFlatBlock(window,
                         static_cast<float>(b.x * TILE),
                         static_cast<float>(b.y * TILE),
                         c);
    }
}
