#include "UI.h"
#include "AssetManager.h"
#include "Board.h"
#include <string>

UI::UI(int offsetX) : offsetX(offsetX) {
    fontChargee = AssetManager::loadFont(font, "assets/fonts/arial.ttf");
}

void UI::drawTexte(sf::RenderWindow& window, const std::string& texte,
                   float x, float y, int taille, sf::Color couleur) const {
    if (!fontChargee) return;
    sf::Text t(texte, font, static_cast<unsigned int>(taille));
    t.setPosition(sf::Vector2f(x, y));
    t.setFillColor(couleur);
    window.draw(t);
}

void UI::draw(sf::RenderWindow& window,
              int score, int highScore, int niveau, bool modeEvolutif,
              const Piece& pieceSuivante) const {

    float panelWidth = 310.f;
    float centerX = offsetX + panelWidth / 2.f;

    // ── Fond du panneau ──────────────────────
    sf::RectangleShape fond(sf::Vector2f(panelWidth, static_cast<float>(ROWS * TILE)));
    fond.setPosition(sf::Vector2f(static_cast<float>(offsetX), 0.f));
    fond.setFillColor(sf::Color(245, 245, 245));
    window.draw(fond);

    auto drawCenteredText = [&](const std::string& str, float y, int size, sf::Color color) {
        sf::Text t(str, font, static_cast<unsigned int>(size));
        float width = t.getLocalBounds().width;
        t.setPosition(sf::Vector2f(centerX - width / 2.f, y));
        t.setFillColor(color);
        window.draw(t);
    };

    // ── High Score ───────────────────────────
    drawCenteredText("BEST SCORE", 30, 20, sf::Color(120, 120, 120));
    drawCenteredText(std::to_string(highScore), 60, 32, sf::Color(32, 156, 115));

    // ── Separateur ───────────────────────────
    sf::RectangleShape sep1(sf::Vector2f(200.f, 1.f));
    sep1.setPosition(sf::Vector2f(centerX - 100.f, 110.f));
    sep1.setFillColor(sf::Color(220, 220, 220));
    window.draw(sep1);

    // ── Score ────────────────────────────────
    drawCenteredText("CURRENT SCORE", 130, 20, sf::Color(120, 120, 120));
    drawCenteredText(std::to_string(score), 160, 36, sf::Color::Black);

    float nextSectionY = 260.f;

    // ── Niveau (uniquement en mode évolutif) ─
    if (modeEvolutif) {
        sf::RectangleShape sep2(sf::Vector2f(200.f, 1.f));
        sep2.setPosition(sf::Vector2f(centerX - 100.f, 220.f));
        sep2.setFillColor(sf::Color(220, 220, 220));
        window.draw(sep2);

        drawCenteredText("LEVEL", 240, 20, sf::Color(120, 120, 120));
        drawCenteredText(std::to_string(niveau), 270, 36, sf::Color::Black);
        nextSectionY = 360.f;
    }

    float cadreSize = 5.f * TILE;
    float cadreX = centerX - cadreSize / 2.f;

    sf::RectangleShape cell(sf::Vector2f(TILE - 1.f, TILE - 1.f));

    // ── Pièce suivante ───────────────────────
    drawCenteredText("NEXT PIECE", nextSectionY, 20, sf::Color(120, 120, 120));

    float cadreY = nextSectionY + 35.f;

    sf::RectangleShape cadre(sf::Vector2f(cadreSize, 4.f * TILE));
    cadre.setPosition(sf::Vector2f(cadreX, cadreY));
    cadre.setFillColor(sf::Color(255, 255, 255));
    cadre.setOutlineColor(sf::Color(220, 220, 220));
    cadre.setOutlineThickness(2);
    window.draw(cadre);

    cell.setFillColor(pieceSuivante.getColor());
    for (const Block& b : pieceSuivante.getRelativeBlocs()) {
        float bx = cadreX + (b.x + 2.0f) * TILE;
        float by = cadreY + (b.y + 1.5f) * TILE;
        cell.setPosition(sf::Vector2f(bx, by));
        window.draw(cell);
    }

    // ── Contrôles ────────────────────────────
    float ctrlY = static_cast<float>(ROWS * TILE) - 220.f;
    drawCenteredText("CONTROLS", ctrlY, 18, sf::Color(100, 100, 100));

    float textX = centerX - 90.f;
    auto drawControl = [&](const std::string& str, float y) {
        drawTexte(window, str, textX, y, 15, sf::Color(130, 130, 130));
    };

    drawControl("<>  Move",      ctrlY + 35);
    drawControl("^   Rotate",    ctrlY + 55);
    drawControl("v   Soft Drop", ctrlY + 75);
    drawControl("Spc Hard Drop", ctrlY + 95);
    drawControl("Esc Pause",     ctrlY + 115);
}
