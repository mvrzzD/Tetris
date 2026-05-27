#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>

namespace AssetManager {

bool loadFont(sf::Font& font, const std::string& filename);
bool loadSoundBuffer(sf::SoundBuffer& buffer, const std::string& filename);
bool openMusic(sf::Music& music, const std::string& filename);

std::string resolveAssetPath(const std::string& filename);

}
