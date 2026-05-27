#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <string>

class Audio {
private:
    sf::Music musique;
    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, sf::Sound>       sons;

    bool  musiqueActive;
    float pitchActuel;
    bool  sonActif;
    bool  muteSons;
    bool  muteMusique;
    float volumeSons;
    float volumeMusique;

    void chargerSon(const std::string& nom, const std::string& fichier);

public:
    Audio();
    ~Audio();

    void jouerMusiqueMenu();
    void jouerMusiqueTypeA();
    void jouerMusiqueTypeB();
    void jouerMusiqueJeu(); // Gardée pour compatibilité si besoin
    void stopMusique();
    void pauseMusique();
    void resumeMusique();
    void setPitchMusique(float pitch);
    void updatePitchEvolutif(int niveau);

    void jouerDeplacement();
    void jouerRotation();
    void jouerPose();
    void jouerLigne();
    void jouerTetris();
    void jouerMenuValid();
    void jouerGameOver();

    void setVolumeSons(float volume);
    void setVolumeMusique(float volume);
    void setMuteSons(bool mute);
    void setMuteMusique(bool mute);

    float getVolumeSons() const;
    float getVolumeMusique() const;
    bool  isSonsMuted() const;
    bool  isMusiqueMuted() const;
};
