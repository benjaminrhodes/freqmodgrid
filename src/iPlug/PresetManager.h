#pragma once

#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>

enum class PresetCategory { Lead, Pad, Bass, Keys, FX, Init, User };

struct PresetParameter {
    std::string name;
    float value;
};

struct Preset {
    std::string name;
    PresetCategory category = PresetCategory::Init;
    bool isFavorite = false;
    bool isUserPreset = false;
    std::vector<PresetParameter> parameters;
};

class PresetManager {
public:
    PresetManager();
    
    void loadFactoryPresets(const char* jsonPath);
    void loadUserPresets();
    void saveUserPresets();
    
    const std::vector<Preset>& getPresets() const { return presets_; }
    std::vector<Preset> getPresetsByCategory(PresetCategory cat) const;
    std::vector<Preset> searchPresets(const std::string& query) const;
    void addUserPreset(const Preset& preset);
    void deleteUserPreset(int index);
    void toggleFavorite(int index);
    std::optional<Preset> getPresetByName(const std::string& name) const;
    std::string getUserPresetsPath() const;
    
    static const char* categoryToString(PresetCategory cat);
    static PresetCategory stringToCategory(const std::string& str);

private:
    std::vector<Preset> presets_;
    std::string userPresetsPath_;
    
    std::string readFile(const char* path);
    std::vector<std::string> split(const std::string& s, char delim);
    std::string trim(const std::string& s);
};
