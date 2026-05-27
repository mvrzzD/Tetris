#include "Menu.h"
#include "AssetManager.h"
#include "Board.h"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ctime>

Menu::Menu() : indexSelection(0), indexSelectionPause(0) {
    fontChargee      = AssetManager::loadFont(font, "assets/fonts/PressStart2P-Regular.ttf");
    fontArialChargee = AssetManager::loadFont(fontArial, "assets/fonts/arial.ttf");

    nomsModes = { "Tres lent", "Rapide", "Tres rapide", "Instantane", "Evolutif" };
    vitessesModes = { TRES_LENT, RAPIDE, TRES_RAPIDE, INSTANTANE, EVOLUTIF };
}

void Menu::drawTexte(sf::RenderWindow& window,
                     const std::string& texte,
                     float x, float y,
                     int taille,
                     sf::Color couleur,
                     bool useArial) const {
    if (useArial) {
        if (!fontArialChargee) return;
        sf::Text t(texte, fontArial, static_cast<unsigned int>(taille));
        t.setPosition(sf::Vector2f(x, y));
        t.setFillColor(couleur);
        window.draw(t);
    } else {
        if (!fontChargee) return;
        sf::Text t(texte, font, static_cast<unsigned int>(taille));
        t.setPosition(sf::Vector2f(x, y));
        t.setFillColor(couleur);
        window.draw(t);
    }
}

void Menu::drawTitre(sf::RenderWindow& window, float y) const {
    float larg = static_cast<float>(window.getSize().x);
    std::vector<sf::Color> couleurs = {
        sf::Color(32, 156, 115),
        sf::Color(0, 200, 220),
        sf::Color(160, 80, 200),
        sf::Color(240, 150, 40),
        sf::Color(60, 120, 210),
        sf::Color(242, 84, 91)
    };
    std::string titre = "TETRIS";
    float startX = larg / 2 - 280.f;
    for (int i = 0; i < 6; i++)
        drawTexte(window, std::string(1, titre[i]), startX + i * 90.f, y, 100, couleurs[i]);
}

void Menu::drawWelcome(sf::RenderWindow& window) const {
    float larg = static_cast<float>(window.getSize().x);
    float haut = static_cast<float>(window.getSize().y);
    window.clear(sf::Color(245, 245, 245));
    drawTitre(window, 150.f);
    drawTexte(window, "Emmanuel & Martinaud", larg/2 - 250.f, 300.f, 25, sf::Color(80, 80, 80));

    sf::RectangleShape bloc(sf::Vector2f(TILE - 2.f, TILE - 2.f));
    float patternX = larg/2 - (2 * TILE);
    float patternY = 480.f;

    bloc.setFillColor(sf::Color(32, 156, 115));
    for (int i = 0; i < 4; i++) {
        bloc.setPosition(sf::Vector2f(patternX + i * TILE, patternY));
        window.draw(bloc);
    }
    bloc.setFillColor(sf::Color(240, 150, 40));
    for (int r = 0; r < 2; r++)
        for (int c = 0; c < 2; c++) {
            bloc.setPosition(sf::Vector2f(patternX + c * TILE, patternY + 50.f + r * TILE));
            window.draw(bloc);
        }

    drawTexte(window, "Appuyez sur ENTREE pour commencer",
              larg/2 - 330.f, haut - 100.f, 20, sf::Color(80, 80, 80));
}

void Menu::drawModeSelection(sf::RenderWindow& window) const {
    float larg = static_cast<float>(window.getSize().x);
    window.clear(sf::Color(245, 245, 245));
    drawTitre(window, 80.f);
    drawTexte(window, "Choisissez votre mode de jeu :",
              larg/2 - 325.f, 220.f, 22, sf::Color(60, 60, 60));

    std::vector<std::string> descriptions = {
        "Vitesse lente et constante",
        "Vitesse constante elevee",
        "Vitesse constante tres elevee",
        "Pieces placees directement en bas",
        "Vitesse croissante - niveau affiche"
    };

    for (int i = 0; i < (int)nomsModes.size(); i++) {
        bool sel = (i == indexSelection);
        float y = 280.f + i * 80.f;
        if (sel) {
            sf::RectangleShape fond(sf::Vector2f(600.f, 70.f));
            fond.setPosition(sf::Vector2f(larg/2 - 300.f, y - 5.f));
            fond.setFillColor(sf::Color(220, 220, 220));
            window.draw(fond);
        }
        sf::Color c = sel ? sf::Color(32, 156, 115) : sf::Color(100, 100, 100);
        drawTexte(window, (sel ? "> " : "  ") + nomsModes[i], larg/2 - 280.f, y, 32, c);
        drawTexte(window, "    " + descriptions[i], larg/2 - 280.f, y + 38, 18,
                  sf::Color(120, 120, 120), true);
    }
}

void Menu::drawPause(sf::RenderWindow& window, float volumeSons, float volumeMusique,
                      bool muteSons, bool muteMusique) const {
    float larg = static_cast<float>(window.getSize().x);
    float haut = static_cast<float>(window.getSize().y);

    sf::RectangleShape overlay(sf::Vector2f(larg, haut));
    overlay.setFillColor(sf::Color(0, 0, 0, 80));
    window.draw(overlay);

    sf::RectangleShape cadre(sf::Vector2f(460, 420));
    cadre.setPosition(sf::Vector2f(larg / 2 - 230, haut / 2 - 210));
    cadre.setFillColor(sf::Color(255, 255, 255));
    cadre.setOutlineColor(sf::Color(200, 200, 200));
    cadre.setOutlineThickness(2);
    window.draw(cadre);

    drawTexte(window, "PAUSE", larg / 2 - 90, haut / 2 - 190, 40, sf::Color(32, 156, 115));

    std::string sonsTexte = muteSons ? "SONS : OFF" : "SONS : " + std::to_string(static_cast<int>(volumeSons)) + "%";
    std::string musiqueTexte = muteMusique ? "MUSIQUE : OFF" : "MUSIQUE : " + std::to_string(static_cast<int>(volumeMusique)) + "%";

    std::vector<std::string> options = {
        "REPRENDRE",
        "RECOMMENCER",
        "MENU PRINCIPAL",
        sonsTexte,
        musiqueTexte,
        "QUITTER"
    };

    for (int i = 0; i < (int)options.size(); i++) {
        bool sel = (i == indexSelectionPause);
        float y = haut / 2 - 110 + i * 55;
        if (sel) {
            sf::RectangleShape fond(sf::Vector2f(420, 45));
            fond.setPosition(sf::Vector2f(larg / 2 - 210, y - 5));
            fond.setFillColor(sf::Color(230, 230, 230));
            window.draw(fond);
        }
        sf::Color c = sel ? sf::Color(32, 156, 115) : sf::Color(80, 80, 80);
        drawTexte(window, (sel ? "> " : "  ") + options[i], larg / 2 - 200, y, 22, c);
    }

    drawTexte(window, "Use Gauche/Droite pour regler, ENTREE pour couper", larg / 2 - 210, haut / 2 + 190, 16, sf::Color(120, 120, 120), true);
}

void Menu::drawGameOver(sf::RenderWindow& window, int score, int niveau, bool modeEvolutif) const {
    float larg = static_cast<float>(window.getSize().x);
    float haut = static_cast<float>(window.getSize().y);

    sf::RectangleShape overlay(sf::Vector2f(larg, haut));
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);

    sf::RectangleShape cadre(sf::Vector2f(400, 360));
    cadre.setPosition(sf::Vector2f(larg / 2 - 200, haut / 2 - 180));
    cadre.setFillColor(sf::Color(255, 255, 255));
    cadre.setOutlineColor(sf::Color(200, 200, 200));
    cadre.setOutlineThickness(2);
    window.draw(cadre);

    auto drawCenteredText = [&](const std::string& texte, float y, int taille, sf::Color couleur) {
        if (!fontChargee) return;
        sf::Text t(texte, font, static_cast<unsigned int>(taille));
        float width = t.getLocalBounds().width;
        t.setPosition(sf::Vector2f(larg / 2 - width / 2.f, y));
        t.setFillColor(couleur);
        window.draw(t);
    };

    float baseY = haut / 2 - 140;
    drawCenteredText("GAME OVER", baseY, 40, sf::Color(200, 0, 0));
    drawCenteredText("SCORE : " + std::to_string(score), baseY + 70, 22, sf::Color(80, 80, 80));
    drawCenteredText("NIVEAU : " + std::to_string(niveau), baseY + 100, 22, sf::Color(80, 80, 80));
    drawCenteredText(std::string("MODE : ") + (modeEvolutif ? "Evolutif" : "Normal"), baseY + 130, 22, sf::Color(80, 80, 80));

    std::vector<std::string> options = { "RECOMMENCER", "MENU PRINCIPAL", "QUITTER" };
    for (int i = 0; i < static_cast<int>(options.size()); i++) {
        bool sel = (i == indexSelectionPause);
        float y = baseY + 180 + i * 55;
        if (sel) {
            sf::RectangleShape fond(sf::Vector2f(360, 40));
            fond.setPosition(sf::Vector2f(larg / 2 - 180, y - 5));
            fond.setFillColor(sf::Color(230, 230, 230));
            window.draw(fond);
        }
        sf::Color c = sel ? sf::Color(32, 156, 115) : sf::Color(80, 80, 80);
        drawTexte(window, (sel ? "> " : "  ") + options[i], larg / 2 - 170, y, 22, c);
    }
}

// ── HANDLERS — SFML 2.6.2 ───────────────────────
GameState Menu::handleWelcome(const sf::Event& event) const {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
        return MODE_SELECTION;
    return WELCOME;
}

GameState Menu::handleModeSelection(const sf::Event& event, sf::RenderWindow& window,
                                    int& v, bool& e) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up)
            indexSelection = (indexSelection - 1 + nomsModes.size()) % nomsModes.size();
        if (event.key.code == sf::Keyboard::Down)
            indexSelection = (indexSelection + 1) % nomsModes.size();
        if (event.key.code == sf::Keyboard::Enter) {
            int val = vitessesModes[indexSelection];
            if (val == EVOLUTIF) { v = TRES_LENT; e = true; }
            else                 { v = val;        e = false; }
            return PLAYING;
        }
        if (event.key.code == sf::Keyboard::E)
            window.close();
    }
    return MODE_SELECTION;
}

GameState Menu::handlePause(const sf::Event& event, sf::RenderWindow& window,
                               float& volumeSons, float& volumeMusique,
                               bool& muteSons, bool& muteMusique) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up)
            indexSelectionPause = (indexSelectionPause - 1 + 6) % 6;
        if (event.key.code == sf::Keyboard::Down)
            indexSelectionPause = (indexSelectionPause + 1) % 6;

        if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
            float delta = (event.key.code == sf::Keyboard::Right) ? 5.f : -5.f;
            if (indexSelectionPause == 3) {
                volumeSons = std::clamp(volumeSons + delta, 0.f, 100.f);
                if (volumeSons > 0.f)
                    muteSons = false;
                else
                    muteSons = true;
            }
            if (indexSelectionPause == 4) {
                volumeMusique = std::clamp(volumeMusique + delta, 0.f, 100.f);
                if (volumeMusique > 0.f)
                    muteMusique = false;
                else
                    muteMusique = true;
            }
        }

        if (event.key.code == sf::Keyboard::Enter) {
            if (indexSelectionPause == 0) return PLAYING;
            if (indexSelectionPause == 1) return RESTART;
            if (indexSelectionPause == 2) return MODE_SELECTION;
            if (indexSelectionPause == 3) {
                muteSons = !muteSons;
                if (muteSons)
                    volumeSons = 0.f;
                else if (volumeSons <= 0.f)
                    volumeSons = 70.f;
            }
            if (indexSelectionPause == 4) {
                muteMusique = !muteMusique;
                if (muteMusique)
                    volumeMusique = 0.f;
                else if (volumeMusique <= 0.f)
                    volumeMusique = 50.f;
            }
            if (indexSelectionPause == 5) window.close();
        }
        if (event.key.code == sf::Keyboard::Escape)
            return PLAYING;
    }
    return PAUSE;
}

GameState Menu::handleGameOver(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up)
            indexSelectionPause = (indexSelectionPause - 1 + 3) % 3;
        if (event.key.code == sf::Keyboard::Down)
            indexSelectionPause = (indexSelectionPause + 1) % 3;
        if (event.key.code == sf::Keyboard::Enter) {
            if (indexSelectionPause == 0) return RESTART;
            if (indexSelectionPause == 1) return MODE_SELECTION;
            if (indexSelectionPause == 2) window.close();
        }
    }
    return GAME_OVER;
}
