#include "UI.h"
#include "Board.h"
#include <string>

UI::UI(int offsetX) : offsetX(offsetX) {
    fontChargee = font.loadFromFile("assets/fonts/arial.ttf");
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
              int score, int niveau, bool modeEvolutif,
              const Piece& pieceSuivante) const {

    float px = static_cast<float>(offsetX + 12);

    // ── Fond du panneau ──────────────────────
    sf::RectangleShape fond(sf::Vector2f(188, static_cast<float>(ROWS * TILE)));
    fond.setPosition(sf::Vector2f(static_cast<float>(offsetX), 0.f));
    fond.setFillColor(sf::Color(20, 20, 30));
    window.draw(fond);

    // ── Score ────────────────────────────────
    drawTexte(window, "SCORE", px, 20, 18, sf::Color(160, 160, 160));
    drawTexte(window, std::to_string(score), px, 45, 28, sf::Color::White);

    // ── Niveau (uniquement en mode évolutif) ─
    if (modeEvolutif) {
        drawTexte(window, "NIVEAU", px, 100, 18, sf::Color(160, 160, 160));
        drawTexte(window, std::to_string(niveau), px, 125, 28, sf::Color(255, 213, 0));
    }

    // ── Pièce suivante ───────────────────────
    float nextY = modeEvolutif ? 185.f : 110.f;
    drawTexte(window, "SUIVANT", px, nextY, 18, sf::Color(160, 160, 160));

    // Cadre de la pièce suivante
    float cadreX = px;
    float cadreY = nextY + 28;
    sf::RectangleShape cadre(sf::Vector2f(6 * TILE, 4 * TILE));
    cadre.setPosition(sf::Vector2f(cadreX, cadreY));
    cadre.setFillColor(sf::Color(30, 30, 40));
    cadre.setOutlineColor(sf::Color(70, 70, 90));
    cadre.setOutlineThickness(1);
    window.draw(cadre);

    // Dessin des blocs de la pièce suivante (centrés dans le cadre)
    sf::RectangleShape cell(sf::Vector2f(TILE - 1.f, TILE - 1.f));
    cell.setFillColor(pieceSuivante.getColor());
    for (const Block& b : pieceSuivante.getRelativeBlocs()) {
        float bx = cadreX + (b.x + 2) * TILE;
        float by = cadreY + (b.y + 2) * TILE;
        cell.setPosition(sf::Vector2f(bx, by));
        window.draw(cell);
    }

    // ── Contrôles (rappel) ───────────────────
    float ctrlY = static_cast<float>(ROWS * TILE) - 160.f;
    drawTexte(window, "CONTROLES",     px, ctrlY,        14, sf::Color(100, 100, 100));
    drawTexte(window, "<>  Deplacer",  px, ctrlY + 22,   13, sf::Color(120, 120, 120));
    drawTexte(window, "^   Rotation",  px, ctrlY + 40,   13, sf::Color(120, 120, 120));
    drawTexte(window, "v   Descendre", px, ctrlY + 58,   13, sf::Color(120, 120, 120));
    drawTexte(window, "Esp Hard drop", px, ctrlY + 76,   13, sf::Color(120, 120, 120));
    drawTexte(window, "Ech Pause",     px, ctrlY + 94,   13, sf::Color(120, 120, 120));
}
