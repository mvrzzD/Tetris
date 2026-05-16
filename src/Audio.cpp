#include "Audio.h"
#include <iostream>
#include <algorithm>
using namespace std;

Audio::Audio()
    : musiqueActive(false),
      pitchActuel(1.0f),
      sonActif(true)
{
    // Use existing assets as placeholders
    chargerSon("move",       "assets/soundsEffects/skyscraper_seven-click-buttons-ui-menu-sounds-effects-button-7-203601.mp3");
    chargerSon("rotate",     "assets/soundsEffects/skyscraper_seven-click-buttons-ui-menu-sounds-effects-button-7-203601.mp3");
    chargerSon("pose",       "assets/soundsEffects/skyscraper_seven-click-buttons-ui-menu-sounds-effects-button-7-203601.mp3");
    chargerSon("line",       "assets/soundsEffects/skyscraper_seven-click-buttons-ui-menu-sounds-effects-button-7-203601.mp3");
    chargerSon("menu_valid", "assets/soundsEffects/driken5482-retro-select-236670.mp3");
    chargerSon("gameover",   "assets/soundsEffects/skyscraper_seven-click-buttons-ui-menu-sounds-effects-button-7-203601.mp3");
}

Audio::~Audio() {
    for (auto& [nom, son] : sons) {
        son->stop();
        delete son;
    }
}

void Audio::chargerSon(const string& nom, const string& fichier) {
    sf::SoundBuffer buf;
    if (!buf.loadFromFile(fichier)) {
        cerr << "[Audio] Impossible de charger : " << fichier << "\n";
        return;
    }
    buffers[nom] = buf; // sf::SoundBuffer is copyable in SFML 2
    sons[nom]    = new sf::Sound(buffers[nom]);
    sons[nom]->setVolume(70.f);
}

void Audio::jouerMusiqueMenu() {
    musique.stop();
    if (!musique.openFromFile("assets/soundsEffects/bobbeats-8-bit-loop-music-290770.mp3")) return;
    musique.setLoop(true);
    musique.setVolume(50.f);
    musique.setPitch(1.0f);
    pitchActuel = 1.0f;
    musique.play();
    musiqueActive = true;
}

void Audio::jouerMusiqueJeu() {
    musique.stop();
    if (!musique.openFromFile("assets/soundsEffects/bobbeats-8-bit-loop-music-290770.mp3")) return;
    musique.setLoop(true);
    musique.setVolume(50.f);
    musique.setPitch(1.0f);
    pitchActuel = 1.0f;
    musique.play();
    musiqueActive = true;
}

void Audio::stopMusique()   { musique.stop();  musiqueActive = false; }
void Audio::pauseMusique()  { musique.pause(); musiqueActive = false; }
void Audio::resumeMusique() { musique.play();  musique.setPitch(pitchActuel); musiqueActive = true; }

void Audio::setPitchMusique(float pitch) {
    pitchActuel = pitch;
    musique.setPitch(pitch);
}

void Audio::updatePitchEvolutif(int niveau)
{
    float pitch = min(1.0f + (niveau - 1) * 0.05f, 1.5f);
    if (pitch != pitchActuel) {
        pitchActuel = pitch;
        musique.setPitch(pitch);
    }
}

void Audio::jouerDeplacement()

{ if (sonActif && sons.count("move"))
  sons["move"]->play();
}
void Audio::jouerRotation()
  {
      if (sonActif && sons.count("rotate"))
        sons["rotate"]->play();
  }

void Audio::jouerPose()
 {
      if (sonActif && sons.count("pose"))
        sons["pose"]->play();
 }

void Audio::jouerLigne()
  {
      if (sonActif && sons.count("line"))
        sons["line"]->play();
  }

void Audio::jouerMenuValid()
 { if (sonActif && sons.count("menu_valid")) sons["menu_valid"]->play(); }

void Audio::jouerGameOver()
 { if (sonActif && sons.count("gameover"))   sons["gameover"]->play(); }

void Audio::setVolumeSons(float v)    { for (auto& [n, s] : sons) s->setVolume(v); }
void Audio::setVolumeMusique(float v) { musique.setVolume(v); }
