#include <SFML/Graphics.hpp>
#include "Game.h"

int main() {
    const unsigned int width  = COLS * TILE + 310;
    const unsigned int height = ROWS * TILE;

    sf::RenderWindow window(
        sf::VideoMode(width, height),
        "TETRIS",
        sf::Style::Default
    );
    window.setFramerateLimit(60);

    Game game(window);
    game.run();

    return 0;
}
