#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <SFML/System/Clock.hpp>

enum GameState {
    WELCOME,
    MODE_SELECTION,
    PLAYING,
    PAUSE,
    GAME_OVER,
    RESTART
};

enum GameSpeed {
    TRES_LENT = 900,
    RAPIDE = 350,
    TRES_RAPIDE = 150,
    INSTANTANE = 0,
    EVOLUTIF = -1,
};

class Menu {
private:
    sf::Font font;
    sf::Font fontArial;
    sf::Clock clock; // OK utilis pour animation
    bool fontChargee;
    bool fontArialChargee;

    int indexSelection;
    int indexSelectionPause;

    std::vector<std::string> nomsModes;
    std::vector<int> vitessesModes;

    void drawTexte(sf::RenderWindow& window, const std::string& texte,
                   float x, float y, int taille,
                   sf::Color couleur = sf::Color::White, bool useArial = false) const;


    void drawTitre(sf::RenderWindow& window, float y) const;

public:
    Menu();

    void drawWelcome(sf::RenderWindow& window) const;
    void drawModeSelection(sf::RenderWindow& window) const;
    void drawPause(sf::RenderWindow& window, float volumeSons, float volumeMusique,
                   bool muteSons, bool muteMusique) const;
    void drawGameOver(sf::RenderWindow& window, int score,
                      int niveau, bool modeEvolutif) const;

    GameState handleWelcome(const sf::Event& event) const;
    GameState handleModeSelection(const sf::Event& event,
                                  sf::RenderWindow& window,
                                  int& vitesse_out, bool& evolutif_out);
    GameState handlePause(const sf::Event& event,
                          sf::RenderWindow& window,
                          float& volumeSons, float& volumeMusique,
                          bool& muteSons, bool& muteMusique);
    GameState handleGameOver(const sf::Event& event,
                             sf::RenderWindow& window);

    void resetSelectionPause() { indexSelectionPause = 0; }
};
