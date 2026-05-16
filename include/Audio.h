#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <string>

class Audio {
private:
    sf::Music musique;
    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, sf::Sound*>      sons;

    bool  musiqueActive;
    float pitchActuel;
    bool  sonActif;

    void chargerSon(const std::string& nom, const std::string& fichier);

public:
    Audio();
    ~Audio();

    void jouerMusiqueMenu();
    void jouerMusiqueJeu();
    void stopMusique();
    void pauseMusique();
    void resumeMusique();
    void setPitchMusique(float pitch);
    void updatePitchEvolutif(int niveau);

    void jouerDeplacement();
    void jouerRotation();
    void jouerPose();
    void jouerLigne();
    void jouerMenuValid();
    void jouerGameOver();

    void setVolumeSons(float volume);
    void setVolumeMusique(float volume);
};
