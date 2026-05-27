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
      sacPieces(),
      indiceSac(0),
      leftHeld(false),
      rightHeld(false),
      downHeld(false),
      leftHoldTimer(0.f),
      rightHoldTimer(0.f),
      downHoldTimer(0.f),
      leftHoldDelay(150.f),
      rightHoldDelay(150.f),
      downHoldDelay(50.f)
{
    remplirSac();
    pieceActuelle = genererPiece();
    pieceSuivante = genererPiece();
    audio.jouerMusiqueMenu();
    clock.restart();
}

void Game::remplirSac() {
    sacPieces.clear();
    sacPieces.reserve(7);
    for (int i = 0; i < 7; i++)
        sacPieces.push_back(i);
    std::shuffle(sacPieces.begin(), sacPieces.end(), rng);
    indiceSac = 0;
}

Piece Game::genererPiece() {
    if (indiceSac >= sacPieces.size())
        remplirSac();
    int type = sacPieces[indiceSac++];
    return Piece(type, 3, 0);
}

void Game::resetPartie() {
    board.reset();
    remplirSac();
    pieceActuelle     = genererPiece();
    pieceSuivante     = genererPiece();
    tempsEcoule       = 0.f;
    timerVerrouillage = 0.f;
    enVerrouillage    = false;
    niveauPrecedent   = 1;
    leftHeld          = false;
    rightHeld         = false;
    downHeld          = false;
    leftHoldTimer     = 0.f;
    rightHoldTimer    = 0.f;
    downHoldTimer     = 0.f;
    leftHoldDelay     = 150.f;
    rightHoldDelay    = 150.f;
    downHoldDelay     = 50.f;
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
                else              audio.jouerMusiqueTypeA();
            }
            etat = suivant;
        }
        else if (etat == PAUSE) {
            float volumeSons = audio.getVolumeSons();
            float volumeMusique = audio.getVolumeMusique();
            bool muteSons = audio.isSonsMuted();
            bool muteMusique = audio.isMusiqueMuted();
            GameState suivant = menu.handlePause(event, window, volumeSons, volumeMusique, muteSons, muteMusique);
            audio.setMuteSons(muteSons);
            audio.setMuteMusique(muteMusique);
            audio.setVolumeSons(volumeSons);
            audio.setVolumeMusique(volumeMusique);
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
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Left:
                        if (leftHeld) break;
                        leftHeld = true;
                        leftHoldTimer = 0.f;
                        leftHoldDelay = 150.f;
                        if (deplacerPiece(-1, 0)) {
                            audio.jouerDeplacement();
                            if (enVerrouillage) timerVerrouillage = 0.f;
                        }
                        break;
                    case sf::Keyboard::Right:
                        if (rightHeld) break;
                        rightHeld = true;
                        rightHoldTimer = 0.f;
                        rightHoldDelay = 150.f;
                        if (deplacerPiece(1, 0)) {
                            audio.jouerDeplacement();
                            if (enVerrouillage) timerVerrouillage = 0.f;
                        }
                        break;
                    case sf::Keyboard::Down:
                        if (downHeld) break;
                        downHeld = true;
                        downHoldTimer = 0.f;
                        downHoldDelay = 50.f;
                        if (deplacerPiece(0, 1)) {
                            audio.jouerDeplacement();
                            tempsEcoule = 0.f;
                        }
                        break;
                    case sf::Keyboard::Up: {
                        if (rotationAvecKick()) {
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
                    case sf::Keyboard::Escape:
                        etat = PAUSE;
                        menu.resetSelectionPause();
                        audio.pauseMusique();
                        break;
                    default: break;
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Left)
                    leftHeld = false;
                else if (event.key.code == sf::Keyboard::Right)
                    rightHeld = false;
                else if (event.key.code == sf::Keyboard::Down)
                    downHeld = false;
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

    handleInputHold(deltaMs);

    tempsEcoule += deltaMs;

    if (tempsEcoule >= static_cast<float>(delai)) {
        tempsEcoule = 0.f;
        if (!deplacerPiece(0, 1)) {
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

bool Game::deplacerPiece(int dx, int dy) {
    pieceActuelle.move(dx, dy);
    if (!board.isValid(pieceActuelle)) {
        pieceActuelle.move(-dx, -dy);
        return false;
    }
    return true;
}

bool Game::rotationAvecKick() {
    auto sauvBlocs = pieceActuelle.getRelativeBlocs();
    pieceActuelle.rotate();

    constexpr int decalages[] = {0, 1, -1, 2, -2};
    for (int dx : decalages) {
        pieceActuelle.move(dx, 0);
        if (board.isValid(pieceActuelle))
            return true;
        pieceActuelle.move(-dx, 0);
    }

    pieceActuelle.setBlocs(sauvBlocs);
    return false;
}

void Game::handleInputHold(float deltaMs) {
    if (leftHeld) {
        leftHoldTimer += deltaMs;
        if (leftHoldTimer >= leftHoldDelay) {
            leftHoldTimer -= leftHoldDelay;
            leftHoldDelay = 60.f;
            if (deplacerPiece(-1, 0)) {
                audio.jouerDeplacement();
                if (enVerrouillage) timerVerrouillage = 0.f;
            }
        }
    }

    if (rightHeld) {
        rightHoldTimer += deltaMs;
        if (rightHoldTimer >= rightHoldDelay) {
            rightHoldTimer -= rightHoldDelay;
            rightHoldDelay = 60.f;
            if (deplacerPiece(1, 0)) {
                audio.jouerDeplacement();
                if (enVerrouillage) timerVerrouillage = 0.f;
            }
        }
    }

    if (downHeld) {
        downHoldTimer += deltaMs;
        if (downHoldTimer >= downHoldDelay) {
            downHoldTimer -= downHoldDelay;
            if (deplacerPiece(0, 1)) {
                audio.jouerDeplacement();
                tempsEcoule = 0.f;
            }
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
            menu.drawPause(window,
                           audio.getVolumeSons(),
                           audio.getVolumeMusique(),
                           audio.isSonsMuted(),
                           audio.isMusiqueMuted());
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
