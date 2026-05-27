#include "Audio.h"
#include "AssetManager.h"
#include <iostream>
#include <algorithm>
using namespace std;

Audio::Audio()
    : musiqueActive(false),
      pitchActuel(1.0f),
      sonActif(true),
      muteSons(false),
      muteMusique(false),
      volumeSons(70.f),
      volumeMusique(50.f)
{
    // Attribution précise de chaque fichier à sa fonction
    chargerSon("move",         "assets/audios/click.mp3");
    chargerSon("rotate",       "assets/audios/rotation.mp3");
    chargerSon("pose",         "assets/audios/drop.mp3");
    chargerSon("line",         "assets/audios/line_clear.mp3");
    chargerSon("tetris",       "assets/audios/tetris_clear.mp3");
    chargerSon("menu_valid",   "assets/audios/hold.mp3"); // Son de sélection
    chargerSon("gameover",     "assets/audios/game_over.mp3");
}

Audio::~Audio() = default;

void Audio::chargerSon(const string& nom, const string& fichier) {
    sf::SoundBuffer buf;
    if (!AssetManager::loadSoundBuffer(buf, fichier))
        return;
    buffers[nom] = std::move(buf);
    sons[nom].setBuffer(buffers[nom]);
    sons[nom].setVolume(volumeSons);
}

void Audio::jouerMusiqueMenu() {
    musique.stop();
    if (!AssetManager::openMusic(musique, "assets/audios/Main Menu.mp3")) return;
    musique.setLoop(true);
    musique.setVolume(muteMusique ? 0.f : volumeMusique);
    musique.setPitch(1.0f);
    pitchActuel = 1.0f;
    musique.play();
    musiqueActive = true;
}

void Audio::jouerMusiqueTypeA() {
    musique.stop();
    if (!AssetManager::openMusic(musique, "assets/audios/Type A.mp3")) return;
    musique.setLoop(true);
    musique.setVolume(muteMusique ? 0.f : volumeMusique);
    musique.setPitch(1.0f);
    pitchActuel = 1.0f;
    musique.play();
    musiqueActive = true;
}

void Audio::jouerMusiqueTypeB() {
    musique.stop();
    if (!AssetManager::openMusic(musique, "assets/audios/Type B.mp3")) return;
    musique.setLoop(true);
    musique.setVolume(muteMusique ? 0.f : volumeMusique);
    musique.setPitch(1.0f);
    pitchActuel = 1.0f;
    musique.play();
    musiqueActive = true;
}

void Audio::jouerMusiqueJeu() {
    jouerMusiqueTypeA();
}

void Audio::stopMusique()   { musique.stop();  musiqueActive = false; }
void Audio::pauseMusique()  { musique.pause(); musiqueActive = false; }
void Audio::resumeMusique() { musique.play();  musique.setPitch(pitchActuel); musique.setVolume(muteMusique ? 0.f : volumeMusique); musiqueActive = true; }

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
{
    if (sonActif && !muteSons && sons.count("move"))
        sons["move"].play();
}

void Audio::jouerRotation()
{
    if (sonActif && !muteSons && sons.count("rotate"))
        sons["rotate"].play();
}

void Audio::jouerPose()
{
    if (sonActif && !muteSons && sons.count("pose"))
        sons["pose"].play();
}

void Audio::jouerLigne()
{
    if (sonActif && !muteSons && sons.count("line"))
        sons["line"].play();
}

void Audio::jouerTetris()
{
    if (sonActif && !muteSons && sons.count("tetris"))
        sons["tetris"].play();
}

void Audio::jouerMenuValid()
{
    if (sonActif && !muteSons && sons.count("menu_valid"))
        sons["menu_valid"].play();
}

void Audio::jouerGameOver()
{
    if (sonActif && !muteSons && sons.count("gameover"))
        sons["gameover"].play();
}

void Audio::setVolumeSons(float v) {
    volumeSons = std::clamp(v, 0.f, 100.f);
    float actual = muteSons ? 0.f : volumeSons;
    for (auto& [n, s] : sons)
        s.setVolume(actual);
}

void Audio::setVolumeMusique(float v) {
    volumeMusique = std::clamp(v, 0.f, 100.f);
    musique.setVolume(muteMusique ? 0.f : volumeMusique);
}

void Audio::setMuteSons(bool mute) {
    muteSons = mute;
    float actual = muteSons ? 0.f : volumeSons;
    for (auto& [n, s] : sons)
        s.setVolume(actual);
}

void Audio::setMuteMusique(bool mute) {
    muteMusique = mute;
    musique.setVolume(muteMusique ? 0.f : volumeMusique);
}

float Audio::getVolumeSons() const { return volumeSons; }
float Audio::getVolumeMusique() const { return volumeMusique; }

bool Audio::isSonsMuted() const { return muteSons; }
bool Audio::isMusiqueMuted() const { return muteMusique; }
