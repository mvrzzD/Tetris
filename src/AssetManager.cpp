#include "AssetManager.h"
#include <iostream>

namespace fs = std::filesystem;

static std::vector<fs::path> buildAssetSearchPaths() {
    std::vector<fs::path> paths;
    try {
        paths.push_back(fs::current_path());
    } catch (...) {
    }

#if defined(__linux__)
    try {
        fs::path exe = fs::read_symlink("/proc/self/exe");
        paths.push_back(exe.parent_path());
    } catch (...) {
    }
#endif

    try {
        fs::path path = fs::current_path();
        for (int i = 0; i < 3; ++i) {
            paths.push_back(path);
            paths.push_back(path / "assets");
            if (path.has_parent_path())
                path = path.parent_path();
        }
    } catch (...) {
    }

    return paths;
}

std::string AssetManager::resolveAssetPath(const std::string& filename) {
    if (filename.empty())
        return filename;

    fs::path requested(filename);
    if (requested.is_absolute() && fs::exists(requested))
        return requested.string();

    if (fs::exists(requested))
        return requested.string();

    static const std::vector<fs::path> searchPaths = buildAssetSearchPaths();

    for (const fs::path& base : searchPaths) {
        fs::path candidate = base / requested;
        if (fs::exists(candidate))
            return candidate.string();
    }

    std::cerr << "[AssetManager] asset introuvable : " << filename << "\n";
    return filename;
}

bool AssetManager::loadFont(sf::Font& font, const std::string& filename) {
    std::string path = resolveAssetPath(filename);
    if (font.loadFromFile(path))
        return true;

    std::cerr << "[AssetManager] Impossible de charger la police : " << path << "\n";
    return false;
}

bool AssetManager::loadSoundBuffer(sf::SoundBuffer& buffer, const std::string& filename) {
    std::string path = resolveAssetPath(filename);
    if (buffer.loadFromFile(path))
        return true;

    std::cerr << "[AssetManager] Impossible de charger le son : " << path << "\n";
    return false;
}

bool AssetManager::openMusic(sf::Music& music, const std::string& filename) {
    std::string path = resolveAssetPath(filename);
    if (music.openFromFile(path))
        return true;

    std::cerr << "[AssetManager] Impossible d'ouvrir la musique : " << path << "\n";
    return false;
}
