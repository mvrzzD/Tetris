#include "Menu.h"
#include <cmath>
#include <cstdint>
#include <ctime>

// ─────────────────────────────────────────────
//  Constructeur
// ─────────────────────────────────────────────
Menu::Menu() : indexSelection(0) {
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

    drawTexte(window, "TETRIS", larg / 2 - 153.f, y + 2, 80, sf::Color(0,0,0,160));

    std::vector<sf::Color> couleurs = {
        sf::Color(0,220,220),
        sf::Color(255,213,0),
        sf::Color(160,0,200),
        sf::Color(0,200,60),
        sf::Color(255,140,0),
        sf::Color(220,30,30)
    };

    std::string titre = "TETRIS";
    float startX = larg / 2 - 155.f;

    for (int i = 0; i < 6; i++) {
        drawTexte(window,
                  std::string(1, titre[i]),
                  startX + i * 52.f,
                  y,
                  80,
                  couleurs[i]);
    }
}

// ─────────────────────────────────────────────
//  WELCOME
// ─────────────────────────────────────────────
void Menu::drawWelcome(sf::RenderWindow& window) const {
    float larg = window.getSize().x;
    float haut = window.getSize().y;

    window.clear(sf::Color(15,15,15));

    drawTitre(window, 80.f);

    drawTexte(window,
              "Emmanuel & Martinaud",
              larg/2 - 115.f,
              185.f,
              22,
              sf::Color(160,160,160));

    sf::RectangleShape bloc(sf::Vector2f(20,20));

    bloc.setFillColor(sf::Color(0,220,220));
    for (int i=0;i<4;i++){
        bloc.setPosition(sf::Vector2f(40.f+i*22,300.f));
        window.draw(bloc);
    }

    bloc.setFillColor(sf::Color(255,213,0));
    for(int r=0;r<2;r++)
        for(int c=0;c<2;c++){
            bloc.setPosition(sf::Vector2f(40.f+c*22,330.f+r*22));
            window.draw(bloc);
        }

    bloc.setFillColor(sf::Color(160,0,200));
    bloc.setPosition(sf::Vector2f(larg-100.f,300.f)); window.draw(bloc);
    bloc.setPosition(sf::Vector2f(larg-78.f,300.f)); window.draw(bloc);
    bloc.setPosition(sf::Vector2f(larg-56.f,300.f)); window.draw(bloc);
    bloc.setPosition(sf::Vector2f(larg-78.f,322.f)); window.draw(bloc);

    float t = std::sin(static_cast<float>(std::time(nullptr)));

    int alpha = 128 + static_cast<int>(127 * t);

    drawTexte(window,
              "Appuyez sur ENTREE pour commencer",
              larg/2 - 170.f,
              haut - 80.f,
              22,
              sf::Color(255,255,255,(uint8_t)alpha));
}

// ─────────────────────────────────────────────
//  MODE SELECTION
// ─────────────────────────────────────────────
void Menu::drawModeSelection(sf::RenderWindow& window) const {
    float larg = window.getSize().x;

    window.clear(sf::Color(15,15,15));

    drawTitre(window, 40.f);

    drawTexte(window,
              "Choisissez votre mode de jeu :",
              larg/2 - 145.f,
              145.f,
              22,
              sf::Color(200,200,200));

    std::vector<std::string> descriptions = {
        "Vitesse lente et constante",
        "Vitesse constante elevee",
        "Vitesse constante tres elevee",
        "Pieces placees directement en bas",
        "Vitesse croissante - niveau affiche"
    };

    for(int i=0;i<(int)nomsModes.size();i++){
        bool sel = (i==indexSelection);
        float y = 185.f + i*58.f;

        if(sel){
            sf::RectangleShape fond(sf::Vector2f(320,48));
            fond.setPosition(sf::Vector2f(larg/2 - 160.f,y-4));
            fond.setFillColor(sf::Color(40,40,70));
            window.draw(fond);
        }

        sf::Color c = sel ? sf::Color(255,213,0)
                          : sf::Color(220,220,220);

        drawTexte(window,(sel?"> ":"  ")+nomsModes[i],larg/2-150.f,y,26,c);
        drawTexte(window,"    "+descriptions[i],larg/2-150.f,y+28,14,sf::Color(140,140,140));
    }
}

// ─────────────────────────────────────────────
//  PAUSE
// ─────────────────────────────────────────────
void Menu::drawPause(sf::RenderWindow& window) const {
    float larg = window.getSize().x;
    float haut = window.getSize().y;

    sf::RectangleShape overlay(sf::Vector2f(larg,haut));
    overlay.setFillColor(sf::Color(0,0,0,160));
    window.draw(overlay);

    sf::RectangleShape cadre(sf::Vector2f(360,200));
    cadre.setPosition(sf::Vector2f(larg/2-180,haut/2-100));
    cadre.setFillColor(sf::Color(20,20,45));
    window.draw(cadre);

    drawTexte(window,"PAUSE",larg/2- 70,haut/2-85,40,sf::Color::Yellow);
    drawTexte(window,"Echap - Reprendre",larg/2-130,haut/2-10,22);
    drawTexte(window,"R - Recommencer", larg/2-130, haut/2+30,22);
    drawTexte(window,"Q - Quitter",larg/2-130,haut/2+70,22);
}

// ─────────────────────────────────────────────
//  GAME OVER
// ─────────────────────────────────────────────
void Menu::drawGameOver(sf::RenderWindow& window,int score,int niveau,bool modeEvolutif) const {
    float larg = window.getSize().x;
    float haut = window.getSize().y;

    sf::RectangleShape overlay(sf::Vector2f(larg,haut));
    overlay.setFillColor(sf::Color(0,0,0,180));
    window.draw(overlay);

    float cadreH = modeEvolutif ? 230.f : 200.f;

    sf::RectangleShape cadre(sf::Vector2f(320,cadreH));
    cadre.setPosition(sf::Vector2f(larg/2-160,haut/2-cadreH/2));
    cadre.setFillColor(sf::Color(25,10,10));
    window.draw(cadre);

    float baseY = haut/2-cadreH/2;

    drawTexte(window,"GAME OVER",larg/2-100,baseY+15,38,sf::Color::Red);
    drawTexte(window,"Score : "+std::to_string(score),larg/2-75,baseY+70,26);

    if(modeEvolutif)
        drawTexte(window,"Niveau : "+std::to_string(niveau),larg/2-75,baseY+105,24);
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

GameState Menu::handlePause(const sf::Event& event,sf::RenderWindow& window) const {
    if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::Escape)
            return PLAYING;

        if(event.key.code == sf::Keyboard::R)
            return MODE_SELECTION;

        if(event.key.code == sf::Keyboard::Q)
          window.close();
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
