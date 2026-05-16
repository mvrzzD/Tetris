#include "Menu.h"
#include "Board.h"
#include <cmath>
#include <cstdint>
#include <ctime>

// ─────────────────────────────────────────────
//  Constructeur
// ─────────────────────────────────────────────
Menu::Menu() : indexSelection(0), indexSelectionPause(0) {
    fontChargee = font.loadFromFile("assets/fonts/PressStart2P-Regular.ttf");

    nomsModes = {
        "Tres lent",
        "Rapide",
        "Tres rapide",
        "Instantane",
        "Evolutif"
    };

    vitessesModes = {
        TRES_LENT,
        RAPIDE,
        TRES_RAPIDE,
        INSTANTANE,
        EVOLUTIF
    };
}

// ─────────────────────────────────────────────
//  Texte
// ─────────────────────────────────────────────
void Menu::drawTexte(sf::RenderWindow& window,
                     const std::string& texte,
                     float x, float y,
                     int taille,
                     sf::Color couleur) const {
    if (!fontChargee) return;

    sf::Text t(texte, font, static_cast<unsigned int>(taille));
    t.setPosition(sf::Vector2f(x, y));
    t.setFillColor(couleur);

    window.draw(t);
}

// ─────────────────────────────────────────────
//  Titre
// ─────────────────────────────────────────────
void Menu::drawTitre(sf::RenderWindow& window, float y) const {
    float larg = static_cast<float>(window.getSize().x);

    drawTexte(window, "TETRIS", larg / 2 - 200.f, y + 2, 100, sf::Color(0,0,0,30));

    std::vector<sf::Color> couleurs = {
        sf::Color(32, 156, 115),
        sf::Color(0, 200, 220),
        sf::Color(160, 80, 200),
        sf::Color(240, 150, 40),
        sf::Color(60, 120, 210),
        sf::Color(242, 84, 91)
    };

    std::string titre = "TETRIS";
    float startX = larg / 2 - 202.f;

    for (int i = 0; i < 6; i++) {
        drawTexte(window,
                  std::string(1, titre[i]),
                  startX + i * 68.f,
                  y,
                  100,
                  couleurs[i]);
    }
}

// ─────────────────────────────────────────────
//  WELCOME
// ─────────────────────────────────────────────
void Menu::drawWelcome(sf::RenderWindow& window) const {
    float larg = static_cast<float>(window.getSize().x);
    float haut = static_cast<float>(window.getSize().y);

    window.clear(sf::Color(245, 245, 245));

    drawTitre(window, 150.f);

    drawTexte(window,
              "Emmanuel & Martinaud",
              larg/2 - 180.f,
              300.f,
              30,
              sf::Color(80, 80, 80));

    sf::RectangleShape bloc(sf::Vector2f(TILE - 2.f, TILE - 2.f));

    float patternX = larg/2 - (2 * TILE);
    float patternY = 480.f;

    bloc.setFillColor(sf::Color(32, 156, 115));
    for (int i=0;i<4;i++){
        bloc.setPosition(sf::Vector2f(patternX + i*TILE, patternY));
        window.draw(bloc);
    }

    bloc.setFillColor(sf::Color(240, 150, 40));
    for(int r=0;r<2;r++)
        for(int c=0;c<2;c++){
            bloc.setPosition(sf::Vector2f(patternX + c*TILE, patternY + 50.f + r*TILE));
            window.draw(bloc);
        }

    float t = std::sin(static_cast<float>(std::time(nullptr)));
    int alpha = 128 + static_cast<int>(127 * t);

    drawTexte(window,
              "Appuyez sur ENTREE pour commencer",
              larg/2 - 280.f,
              haut - 120.f,
              26,
              sf::Color(50, 50, 50, (uint8_t)alpha));
}

// ─────────────────────────────────────────────
//  MODE SELECTION
// ─────────────────────────────────────────────
void Menu::drawModeSelection(sf::RenderWindow& window) const {
    float larg = static_cast<float>(window.getSize().x);

    window.clear(sf::Color(245, 245, 245));

    drawTitre(window, 80.f);

    drawTexte(window,
              "Choisissez votre mode de jeu :",
              larg/2 - 250.f,
              220.f,
              28,
              sf::Color(60, 60, 60));

    std::vector<std::string> descriptions = {
        "Vitesse lente et constante",
        "Vitesse constante elevee",
        "Vitesse constante tres elevee",
        "Pieces placees directement en bas",
        "Vitesse croissante - niveau affiche"
    };

    for(int i=0;i<(int)nomsModes.size();i++){
        bool sel = (i==indexSelection);
        float y = 280.f + i*80.f;

        if(sel){
            sf::RectangleShape fond(sf::Vector2f(600.f, 70.f));
            fond.setPosition(sf::Vector2f(larg/2 - 300.f, y - 5.f));
            fond.setFillColor(sf::Color(220, 220, 220));
            window.draw(fond);
        }

        sf::Color c = sel ? sf::Color(32, 156, 115)
                          : sf::Color(100, 100, 100);

        drawTexte(window,(sel?"> ":"  ")+nomsModes[i], larg/2 - 280.f, y, 32, c);
        drawTexte(window,"    "+descriptions[i], larg/2 - 280.f, y + 38, 18, sf::Color(120, 120, 120));
    }
}

// ─────────────────────────────────────────────
//  PAUSE
// ─────────────────────────────────────────────
void Menu::drawPause(sf::RenderWindow& window) const {
    float larg = static_cast<float>(window.getSize().x);
    float haut = static_cast<float>(window.getSize().y);

    sf::RectangleShape overlay(sf::Vector2f(larg, haut));
    overlay.setFillColor(sf::Color(0, 0, 0, 80));
    window.draw(overlay);

    sf::RectangleShape cadre(sf::Vector2f(400, 320));
    cadre.setPosition(sf::Vector2f(larg / 2 - 200, haut / 2 - 160));
    cadre.setFillColor(sf::Color(255, 255, 255));
    cadre.setOutlineColor(sf::Color(200, 200, 200));
    cadre.setOutlineThickness(2);
    window.draw(cadre);

    drawTexte(window, "PAUSE", larg / 2 - 90, haut / 2 - 140, 40, sf::Color(32, 156, 115));

    std::vector<std::string> options = {
        "REPRENDRE",
        "RECOMMENCER",
        "MENU PRINCIPAL",
        "QUITTER"
    };

    for (int i = 0; i < (int)options.size(); i++) {
        bool sel = (i == indexSelectionPause);
        float y = haut / 2 - 50 + i * 50;

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

// ─────────────────────────────────────────────
//  GAME OVER
// ─────────────────────────────────────────────
void Menu::drawGameOver(sf::RenderWindow& window,int score,int niveau,bool modeEvolutif) const {
    float larg = window.getSize().x;
    float haut = window.getSize().y;

    sf::RectangleShape overlay(sf::Vector2f(larg,haut));
    overlay.setFillColor(sf::Color(0,0,0,100));
    window.draw(overlay);

    float cadreH = modeEvolutif ? 230.f : 200.f;

    sf::RectangleShape cadre(sf::Vector2f(320,cadreH));
    cadre.setPosition(sf::Vector2f(larg/2-160,haut/2-cadreH/2));
    cadre.setFillColor(sf::Color(255,255,255));
    cadre.setOutlineColor(sf::Color(200, 200, 200));
    cadre.setOutlineThickness(2);
    window.draw(cadre);

    float baseY = haut/2-cadreH/2;

    drawTexte(window,"GAME OVER",larg/2-100,baseY+15,38,sf::Color(216, 84, 131));
    drawTexte(window,"Score : "+std::to_string(score),larg/2-75,baseY+70,26, sf::Color(60, 60, 60));

    if(modeEvolutif)
        drawTexte(window,"Niveau : "+std::to_string(niveau),larg/2-75,baseY+105,24, sf::Color(60, 60, 60));
}

// ─────────────────────────────────────────────
//  HANDLERS (OBLIGATOIRES POUR LINKER)
// ─────────────────────────────────────────────
GameState Menu::handleWelcome(const sf::Event& event) const {
    if(event.type == sf::Event::KeyPressed)
        if(event.key.code == sf::Keyboard::Enter)
            return MODE_SELECTION;

    return WELCOME;
}

GameState Menu::handleModeSelection(const sf::Event& event,int& v,bool& e) {
    if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::Up)
            indexSelection = (indexSelection-1+nomsModes.size())%nomsModes.size();

        if(event.key.code == sf::Keyboard::Down)
            indexSelection = (indexSelection+1)%nomsModes.size();

        if(event.key.code == sf::Keyboard::Enter){
            int val = vitessesModes[indexSelection];
            if(val==EVOLUTIF){ v=TRES_LENT; e=true; }
            else { v=val; e=false; }
            return PLAYING;
        }
    }
    return MODE_SELECTION;
}

GameState Menu::handlePause(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up)
            indexSelectionPause = (indexSelectionPause - 1 + 4) % 4;

        if (event.key.code == sf::Keyboard::Down)
            indexSelectionPause = (indexSelectionPause + 1) % 4;

        if (event.key.code == sf::Keyboard::Enter) {
            if (indexSelectionPause == 0) return PLAYING;
            if (indexSelectionPause == 1) return RESTART;
            if (indexSelectionPause == 2) return MODE_SELECTION;
            if (indexSelectionPause == 3) window.close();
        }

        if (event.key.code == sf::Keyboard::Escape)
            return PLAYING;
    }
    return PAUSE;
}

GameState Menu::handleGameOver(const sf::Event& event,sf::RenderWindow& w) const {
    if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::R) return WELCOME;
        if(event.key.code == sf::Keyboard::Q) w.close();
    }
    return GAME_OVER;
}
