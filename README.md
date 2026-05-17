# 🎮 Tetris — C++ / SFML

> Un clone complet du célèbre jeu Tetris développé en **C++ moderne** avec la bibliothèque graphique **SFML 2**, proposant plusieurs modes de jeu, des effets sonores, de la musique et une interface soignée.

---

## 📋 Table des matières

- [Aperçu du projet](#-aperçu-du-projet)
- [Fonctionnalités](#-fonctionnalités)
- [Technologies utilisées](#-technologies-utilisées)
- [Architecture du projet](#-architecture-du-projet)
- [Description des modules](#-description-des-modules)
  - [main.cpp](#maincpp)
  - [Game](#game)
  - [Board](#board)
  - [Piece](#piece)
  - [Menu](#menu)
  - [UI](#ui)
  - [Audio](#audio)
- [Les tétrominos](#-les-tétrominos)
- [Modes de jeu](#-modes-de-jeu)
- [Système de score](#-système-de-score)
- [Contrôles](#-contrôles)
- [Structure des fichiers](#-structure-des-fichiers)
- [Compilation et exécution](#-compilation-et-exécution)
- [Assets](#-assets)
- [Mécaniques avancées](#-mécaniques-avancées)

---

## 🕹️ Aperçu du projet

Ce projet est une réimplémentation fidèle du Tetris original, enrichie de fonctionnalités modernes :

- Interface graphique fluide à **60 FPS**
- **4 modes de vitesse** dont un mode évolutif
- **Pièce fantôme** (ghost piece) pour anticiper la pose
- **Système de Wall Kick** pour les rotations près des bords
- **Lock Delay** de 500 ms avant verrouillage d'une pièce
- **Animation de flash** lors de la suppression de lignes
- **Sauvegarde du meilleur score** entre les parties
- **Musiques et effets sonores** gérés indépendamment

---

## ✨ Fonctionnalités

| Fonctionnalité | Détail |
|---|---|
| 🎯 Gameplay complet | Déplacement, rotation, descente rapide, hard drop |
| 👻 Ghost Piece | Prévisualisation semi-transparente de la position de chute |
| 🔄 Wall Kick | Décalage automatique lors de rotations contre un mur |
| ⏱️ Lock Delay | 500 ms de délai avant de verrouiller une pièce au sol |
| 📈 Mode évolutif | La vitesse et le pitch musical augmentent avec le niveau |
| 🏆 High Score | Score maximum persisté dans `highscore.txt` |
| 🎵 Audio complet | Musiques de menu, Type A & B, effets pour chaque action |
| ⏸️ Pause | Menu de pause avec navigation au clavier |
| 🔢 Pièce suivante | Affichage de la prochaine pièce dans le panneau latéral |
| 🌊 Animation lignes | Flash visuel lors de la suppression de lignes complètes |

---

## 🛠️ Technologies utilisées

### Langage
- **C++17** — Utilisation de fonctionnalités modernes (`std::mt19937`, structured bindings, lambdas)

### Bibliothèque principale
- **SFML 2.x** *(Simple and Fast Multimedia Library)* — Utilisée pour :
  - `sfml-graphics` → Rendu 2D (fenêtre, formes, texte)
  - `sfml-window` → Gestion de la fenêtre et des événements clavier
  - `sfml-system` → Horloge (`sf::Clock`), types vectoriels
  - `sfml-audio` → Musique en streaming (`sf::Music`) et effets sonores (`sf::Sound`, `sf::SoundBuffer`)

### Outils de build
- **g++ (GCC)** — Compilateur C++
- **Code::Blocks** — IDE utilisé pour le développement (fichier `.cbp` inclus)
- **VS Code** — Environnement alternatif (configuration `.zed` incluse)

### Formats de fichiers
- `.mp3` — Fichiers audio (musiques et effets sonores)
- `.ttf` — Polices de caractères (Arial, Press Start 2P)
- `.txt` — Persistance du meilleur score

---

## 🏗️ Architecture du projet

Le projet suit une **architecture orientée objet** avec une séparation claire des responsabilités. Chaque module (classe) a un rôle précis et bien défini.

```
┌─────────────────────────────────────────────────────────┐
│                        main.cpp                          │
│         Crée la fenêtre SFML et lance Game::run()        │
└───────────────────────┬─────────────────────────────────┘
                        │ possède
                        ▼
┌─────────────────────────────────────────────────────────┐
│                        Game                              │
│   Boucle principale : handleEvents / update / render     │
│   Gère l'état global (GameState) et la logique           │
└──┬──────────┬──────────┬──────────┬──────────┬──────────┘
   │          │          │          │          │
   ▼          ▼          ▼          ▼          ▼
 Board      Menu        UI        Audio     Piece(s)
(grille)  (menus)   (panneau)   (son)   (actuelle/suivante)
```

### Diagramme des états du jeu (GameState)

```
         ┌──────────┐
  Start  │          │  Entrée
─────────► WELCOME  ├──────────► MODE_SELECTION
         │          │
         └──────────┘                │ Sélection mode
                                     ▼
                              ┌─────────────┐
                    ┌─────────►   PLAYING   ◄──────────┐
                    │         └──────┬──────┘           │
                    │                │ Escape            │
                    │                ▼                  │ Reprendre
                    │          ┌──────────┐             │
                    │          │  PAUSE   ├─────────────┘
                    │          └──────┬───┘
                    │                │ Restart → RESTART
                    │                │ Menu → MODE_SELECTION
                    │                │ Quitter → ferme
                    │
               RESTART ────────────► (reset + PLAYING)
                    │
                    ▼
             ┌───────────┐
             │ GAME_OVER │
             └─────┬─────┘
                   │
           Recommencer / Menu / Quitter
```

---

## 📦 Description des modules

### `main.cpp`

Point d'entrée du programme. Rôle minimal et propre :

1. Calcule les dimensions de la fenêtre à partir des constantes `COLS`, `ROWS`, `TILE` (définies dans `Board.h`) et de la largeur du panneau latéral (310 px).
2. Crée la fenêtre SFML avec un style par défaut et limite le framerate à **60 FPS**.
3. Instancie un objet `Game` et appelle `game.run()`.

```cpp
const unsigned int width  = COLS * TILE + 310;  // 10*44 + 310 = 750 px
const unsigned int height = ROWS * TILE;         // 20*44 = 880 px
```

---

### `Game`

**Fichiers :** `include/Game.h` — `src/Game.cpp`

Chef d'orchestre du jeu. Contient la boucle principale et coordonne tous les autres modules.

#### Attributs principaux

| Attribut | Type | Rôle |
|---|---|---|
| `window` | `sf::RenderWindow&` | Référence à la fenêtre de rendu |
| `board` | `Board` | Plateau de jeu |
| `menu` | `Menu` | Gestion des menus |
| `ui` | `UI` | Panneau latéral (score, pièce suivante) |
| `audio` | `Audio` | Gestion des sons |
| `pieceActuelle` | `Piece` | Tétromino en cours de jeu |
| `pieceSuivante` | `Piece` | Prochain tétromino à apparaître |
| `etat` | `GameState` | État courant du jeu |
| `vitesse` | `int` | Délai de chute en ms (mode fixe) |
| `modeEvolutif` | `bool` | Actif si le mode "Évolutif" est sélectionné |
| `clock` | `sf::Clock` | Horloge pour mesurer le delta-temps |
| `tempsEcoule` | `float` | Accumulateur de temps pour la chute automatique |
| `timerVerrouillage` | `float` | Compteur du Lock Delay |
| `rng` | `std::mt19937` | Générateur pseudo-aléatoire Mersenne Twister |

#### Méthodes clés

- **`run()`** — Boucle principale `while(window.isOpen())` appelant `handleEvents()`, `update()`, `render()`.
- **`handleEvents()`** — Traite les événements SFML selon l'état courant (navigation menus, contrôles jeu, pause…).
- **`update()`** — Met à jour la logique : descente automatique des pièces, gestion du Lock Delay, mode évolutif (vitesse + pitch audio).
- **`render()`** — Efface l'écran et délègue l'affichage selon l'état.
- **`renderJeu()`** — Affiche le plateau, la ghost piece, la pièce actuelle et le panneau UI.
- **`drawGhostPiece()`** — Calcule et affiche la ghost piece semi-transparente (alpha = 60).
- **`genererPiece()`** — Génère un tétromino aléatoire avec `std::mt19937`.
- **`resetPartie()`** — Réinitialise le plateau et les pièces pour une nouvelle partie.

---

### `Board`

**Fichiers :** `include/Board.h` — `src/Board.cpp`

Représente et gère la grille de jeu.

#### Constantes globales (définies dans `Board.h`)

```cpp
const int ROWS = 20;   // Nombre de lignes
const int COLS = 10;   // Nombre de colonnes
const int TILE = 44;   // Taille d'une case en pixels
```

#### Grille interne

La grille est représentée par un tableau 2D de `sf::Color` :
```cpp
sf::Color grille[ROWS][COLS];
```
Une cellule vide contient `sf::Color::Transparent`. Une cellule occupée contient la couleur de la pièce qui l'a remplie.

#### Système de score

| Lignes supprimées | Points de base |
|---|---|
| 1 ligne | 100 × niveau |
| 2 lignes | 300 × niveau |
| 3 lignes | 500 × niveau |
| 4 lignes (Tetris) | 800 × niveau |

La progression de niveau : `niveau = (lignesDetruites / 10) + 1`

#### Méthodes clés

- **`isValid(piece)`** — Vérifie si une pièce est dans une position légale (hors limites, collision).
- **`placerPiece(piece)`** — Copie les couleurs des blocs de la pièce dans la grille.
- **`supprimerLignes()`** — Détecte les lignes complètes, les supprime, fait descendre les lignes supérieures, calcule le score.
- **`updateAnimation(deltaMs)`** — Gère le timer du flash d'animation des lignes supprimées (200 ms).
- **`isGameOver(piece)`** — Retourne `true` si la nouvelle pièce générée est déjà en position invalide.
- **`chargerHighScore()` / `sauvegarderHighScore()`** — Lecture/écriture dans `highscore.txt`.
- **`draw()`** — Rendu du plateau : fond, cellules vides (gris clair), blocs colorés, animation de flash, bordure.

---

### `Piece`

**Fichiers :** `include/Piece.h` — `src/Piece.cpp`

Représente un tétromino (pièce de Tetris).

#### Structure `Block`

```cpp
struct Block { int x; int y; };
```

Chaque pièce est composée de 4 blocs dont les positions sont **relatives** au point d'ancrage `(posX, posY)`.

#### Rotation

La rotation 90° dans le sens horaire est appliquée avec la formule mathématique :
```
(x, y) → (y, −x)
```
La pièce **O (carré)** est exclue de la rotation car elle est symétrique.

#### Méthodes

- **`rotate()`** — Applique la rotation à tous les blocs relatifs.
- **`move(dx, dy)`** — Déplace la pièce en modifiant `posX` et `posY`.
- **`getBlocks()`** — Retourne les positions **absolues** (posX + bloc.x, posY + bloc.y).
- **`getRelativeBlocs()`** — Retourne les positions relatives (utilisé pour l'affichage de la pièce suivante).

---

### `Menu`

**Fichiers :** `include/Menu.h` — `src/Menu.cpp`

Gère tous les écrans de menu avec navigation au clavier.

#### Enum `GameState`

```cpp
enum GameState {
    WELCOME,        // Écran titre
    MODE_SELECTION, // Choix du mode de jeu
    PLAYING,        // Partie en cours
    PAUSE,          // Jeu en pause
    GAME_OVER,      // Fin de partie
    RESTART         // Signal de redémarrage
};
```

#### Enum `GameSpeed`

```cpp
enum GameSpeed {
    TRES_LENT  = 900,   // Très lent (délai 900ms)
    RAPIDE     = 350,   // Rapide (délai 350ms)
    TRES_RAPIDE = 150,  // Très rapide (délai 150ms)
    INSTANTANE = 0,     // Instantané
    EVOLUTIF   = -1,    // Valeur sentinelle pour le mode évolutif
};
```

#### Fonctionnalités

- Navigation avec `↑`/`↓` et validation avec `Entrée`
- Mise en surbrillance de l'option sélectionnée (fond gris + flèche `>`)
- Deux polices chargées : **Arial** (texte courant) et **Press Start 2P** (titre stylisé)
- Animation du titre basée sur `sf::Clock`
- Gestion séparée des indices de sélection pour le menu pause et le menu Game Over

---

### `UI`

**Fichiers :** `include/UI.h` — `src/UI.cpp`

Panneau latéral affiché à droite du plateau de jeu (310 px de large).

#### Contenu affiché

1. **BEST SCORE** — Meilleur score historique (en vert)
2. **CURRENT SCORE** — Score de la partie en cours
3. **LEVEL** *(uniquement en mode évolutif)* — Niveau actuel
4. **NEXT PIECE** — Aperçu du prochain tétromino dans un cadre blanc
5. **CONTROLS** — Rappel des contrôles du clavier en bas du panneau

#### Disposition

Le panneau utilise un alignement centré calculé dynamiquement pour chaque texte :
```cpp
float width = t.getLocalBounds().width;
t.setPosition(centerX - width / 2.f, y);
```

---

### `Audio`

**Fichiers :** `include/Audio.h` — `src/Audio.cpp`

Gestion complète de l'audio via SFML.

#### Architecture audio

| Composant SFML | Usage | Avantage |
|---|---|---|
| `sf::Music` | Musiques de fond (streaming) | Pas de chargement en mémoire RAM |
| `sf::SoundBuffer` | Effets sonores courts (chargés en RAM) | Lecture instantanée |
| `sf::Sound` | Lecteur d'un `SoundBuffer` | Permet plusieurs sons simultanés |

#### Musiques disponibles

| Fichier | Contexte |
|---|---|
| `Main Menu.mp3` | Écran d'accueil et sélection de mode |
| `Type A.mp3` | Musique de jeu (thème Tetris classique A) |
| `Type B.mp3` | Musique de jeu alternative (thème B) |

#### Effets sonores

| Son | Déclencheur |
|---|---|
| `click.mp3` | Déplacement d'une pièce |
| `rotation.mp3` | Rotation d'une pièce |
| `drop.mp3` | Pose d'une pièce (Lock ou Hard Drop) |
| `line_clear.mp3` | Suppression de 1 à 3 lignes |
| `tetris_clear.mp3` | Suppression de 4 lignes (Tetris !) |
| `hold.mp3` | Validation dans les menus |
| `game_over.mp3` | Fin de partie |

#### Mode évolutif — Pitch dynamique

En mode évolutif, le tempo apparent de la musique augmente avec le niveau :
```cpp
float pitch = min(1.0f + (niveau - 1) * 0.05f, 1.5f);
```
Le pitch est plafonné à **1.5** (soit +50 % de vitesse au maximum, niveau 11).

---

## 🧩 Les tétrominos

Le jeu implémente les 7 tétrominos officiels du Tetris, chacun avec une couleur unique :

| # | Nom | Forme | Couleur |
|---|---|---|---|
| 0 | **O** (Carré) | `██` | Vert `(32, 156, 115)` |
| 1 | **I** (Bâton) | `████` | Cyan `(0, 200, 220)` |
| 2 | **T** | `_█_` / `███` | Violet `(160, 80, 200)` |
| 3 | **L** | `█__` / `███` | Orange `(240, 150, 40)` |
| 4 | **J** | `__█` / `███` | Bleu `(60, 120, 210)` |
| 5 | **S** | `_██` / `██_` | Rouge/Rose `(242, 84, 91)` |
| 6 | **Z** | `██_` / `_██` | Rose `(216, 84, 131)` |

La génération aléatoire utilise `std::mt19937` (Mersenne Twister), bien plus fiable que `rand()`.

---

## 🎮 Modes de jeu

| Mode | Délai de chute | Description |
|---|---|---|
| **Très lent** | 900 ms | Idéal pour les débutants |
| **Rapide** | 350 ms | Niveau intermédiaire |
| **Très rapide** | 150 ms | Pour les joueurs expérimentés |
| **Instantané** | 0 ms | Chute immédiate, réflexes purs |
| **Évolutif** | Variable | Commence à 900 ms, accélère de 60 ms par niveau (minimum 100 ms) |

### Formule du mode évolutif

```cpp
int delai = std::max(100, TRES_LENT - (niveau - 1) * 60);
// Niveau 1 : 900ms | Niveau 5 : 660ms | Niveau 10 : 360ms | Niveau 14+ : 100ms
```

---

## 🏆 Système de score

- **Score de base** multiplié par le **niveau courant**
- Le niveau monte tous les **10 lignes** supprimées
- Le **meilleur score** est persisté dans `highscore.txt` et rechargé à chaque lancement
- Encouragement à enchaîner les **Tetris** (4 lignes = 800 pts × niveau, bien supérieur à 4 × 100 pts)

---

## ⌨️ Contrôles

| Touche | Action |
|---|---|
| `←` `→` | Déplacer la pièce latéralement |
| `↑` | Faire pivoter la pièce (rotation horaire) |
| `↓` | Descente douce accélérée (*Soft Drop*) |
| `Espace` | Chute instantanée (*Hard Drop*) |
| `Échap` | Mettre en pause / reprendre |
| `Entrée` | Valider dans les menus |
| `↑` `↓` | Naviguer dans les menus |
| `E` | Quitter (depuis la sélection de mode) |

---

## 📁 Structure des fichiers

```
Tetris/
├── main.cpp                    # Point d'entrée
│
├── include/                    # Fichiers d'en-tête (.h)
│   ├── Game.h                  # Classe Game (boucle principale)
│   ├── Board.h                 # Classe Board (grille + constantes ROWS/COLS/TILE)
│   ├── Piece.h                 # Classe Piece + struct Block
│   ├── Menu.h                  # Classe Menu + enums GameState/GameSpeed
│   ├── UI.h                    # Classe UI (panneau latéral)
│   └── Audio.h                 # Classe Audio (sons et musiques)
│
├── src/                        # Implémentations (.cpp)
│   ├── Game.cpp
│   ├── Board.cpp
│   ├── Piece.cpp
│   ├── Menu.cpp
│   ├── UI.cpp
│   └── Audio.cpp
│
├── assets/
│   ├── audios/                 # Fichiers audio
│   │   ├── Main Menu.mp3
│   │   ├── Type A.mp3
│   │   ├── Type B.mp3
│   │   ├── click.mp3
│   │   ├── rotation.mp3
│   │   ├── drop.mp3
│   │   ├── line_clear.mp3
│   │   ├── tetris_clear.mp3
│   │   ├── hold.mp3
│   │   └── game_over.mp3
│   └── fonts/                  # Polices de caractères
│       ├── arial.ttf            # Police principale (UI, menus)
│       ├── arialbd.ttf          # Arial Bold
│       └── PressStart2P-Regular.ttf  # Police rétro pour le titre
│
├── highscore.txt               # Sauvegarde du meilleur score
├── Tetris3.cbp                 # Fichier projet Code::Blocks
└── README.md                   # Ce fichier
```

---

## 🔨 Compilation et exécution

### Prérequis

- **g++** avec support C++17 (`-std=c++17`)
- **SFML 2.x** installée sur le système

```bash
# Ubuntu / Debian
sudo apt install libsfml-dev
```

### Compilation manuelle (g++)

```bash
g++ main.cpp src/*.cpp -o tetris_game \
    -Iinclude \
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

### Exécution

```bash
./tetris_game
```

> ⚠️ **Important** : Le jeu doit être lancé depuis le répertoire racine du projet pour que les chemins relatifs vers `assets/` et `highscore.txt` soient corrects.

### Avec Code::Blocks

Ouvrir le fichier `Tetris3.cbp` dans Code::Blocks et utiliser **Build & Run** (F9).

---

## 🎵 Assets

### Polices
- **Arial** (famille complète) — Utilisée pour les menus, le panneau UI et les informations de score
- **Press Start 2P** — Police pixel-art rétro utilisée pour le titre "TETRIS"

### Audio
Tous les fichiers audio sont au format **MP3** :
- Les **musiques** utilisent `sf::Music` (streaming direct depuis le disque, économie de RAM)
- Les **effets sonores** utilisent `sf::SoundBuffer` (chargés en mémoire pour une lecture instantanée)

---

## ⚙️ Mécaniques avancées

### 👻 Ghost Piece (Pièce fantôme)

La ghost piece est calculée en temps réel : on crée une copie de la pièce actuelle et on la déplace vers le bas jusqu'à ce qu'elle entre en collision. Elle est ensuite affichée avec un alpha à 60 (semi-transparente).

```cpp
void Game::drawGhostPiece() {
    Piece ghost = pieceActuelle;
    while (board.isValid(ghost)) ghost.move(0, 1);
    ghost.move(0, -1);
    // Affichage avec couleur.a = 60
}
```

### 🔄 Wall Kick

Lors d'une rotation, si la position résultante est invalide, le système tente automatiquement des décalages latéraux `{0, 1, -1, 2, -2}` pour valider la rotation. Ce système permet de faire pivoter les pièces contre les parois ou d'autres blocs.

```cpp
int decalages[] = {0, 1, -1, 2, -2};
for (int dx : decalages) {
    pieceActuelle.move(dx, 0);
    if (board.isValid(pieceActuelle)) { valide = true; break; }
    pieceActuelle.move(-dx, 0);
}
```

### ⏱️ Lock Delay

Quand une pièce touche le sol, elle n'est pas verrouillée immédiatement. Un timer de **500 ms** est déclenché. Si le joueur déplace ou fait pivoter la pièce pendant ce laps de temps, le timer est remis à zéro, lui donnant la possibilité de corriger sa position.

### 🎲 Générateur aléatoire

L'utilisation de `std::mt19937` (Mersenne Twister) garantit une distribution uniforme et évite les biais statistiques souvent présents avec `rand()`. Le générateur est seedé avec `std::random_device` pour des séquences différentes à chaque partie.

---

*Projet développé en C++ avec SFML — Tetris est une marque déposée de The Tetris Company.*
