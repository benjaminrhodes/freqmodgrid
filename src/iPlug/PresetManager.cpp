#include "PresetManager.h"
#include <iostream>
#include <cctype>

PresetManager::PresetManager() {
    char* homeDir = getenv("HOME");
    if (homeDir) {
        userPresetsPath_ = std::string(homeDir) + "/Library/Application Support/FreqmodGrid/user_presets.json";
    }
}

void PresetManager::loadFactoryPresets(const char* jsonPath) {
    std::string content = readFile(jsonPath);
    if (content.empty()) {
        std::cerr << "Failed to load factory presets from " << jsonPath << std::endl;
        return;
    }
    
    Preset preset;
    preset.isUserPreset = false;
    preset.isFavorite = false;
    preset.category = PresetCategory::Init;
    
    size_t pos = 0;
    while (pos < content.size()) {
        size_t namePos = content.find("\"name\"", pos);
        if (namePos == std::string::npos) break;
        
        size_t colonPos = content.find(":", namePos);
        if (colonPos == std::string::npos) break;
        
        size_t quoteStart = content.find("\"", colonPos + 1);
        size_t quoteEnd = content.find("\"", quoteStart + 1);
        
        if (quoteStart == std::string::npos || quoteEnd == std::string::npos) break;
        
        preset.name = content.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
        
        size_t catPos = content.find("\"category\"", pos);
        if (catPos != std::string::npos && catPos < namePos) {
            size_t catColon = content.find(":", catPos);
            size_t catQuoteStart = content.find("\"", catColon + 1);
            size_t catQuoteEnd = content.find("\"", catQuoteStart + 1);
            if (catQuoteStart != std::string::npos && catQuoteEnd != std::string::npos) {
                std::string catStr = content.substr(catQuoteStart + 1, catQuoteEnd - catQuoteStart - 1);
                preset.category = stringToCategory(catStr);
            }
        }
        
        presets_.push_back(preset);
        pos = quoteEnd + 1;
    }
    
    std::cout << "Loaded " << presets_.size() << " factory presets" << std::endl;
}

void PresetManager::loadUserPresets() {
    std::string content = readFile(userPresetsPath_.c_str());
    if (content.empty()) return;
    
    Preset preset;
    preset.isUserPreset = true;
    
    size_t pos = 0;
    while (pos < content.size()) {
        size_t namePos = content.find("\"name\"", pos);
        if (namePos == std::string::npos) break;
        
        size_t colonPos = content.find(":", namePos);
        if (colonPos == std::string::npos) break;
        
        size_t quoteStart = content.find("\"", colonPos + 1);
        size_t quoteEnd = content.find("\"", quoteStart + 1);
        
        if (quoteStart == std::string::npos || quoteEnd == std::string::npos) break;
        
        preset.name = content.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
        
        size_t favPos = content.find("\"isFavorite\"", pos);
        if (favPos != std::string::npos && favPos < namePos) {
            size_t favColon = content.find(":", favPos);
            std::string favVal = content.substr(favColon + 1, 4);
            preset.isFavorite = (favVal.find("true") != std::string::npos);
        }
        
        presets_.push_back(preset);
        pos = quoteEnd + 1;
    }
}

void PresetManager::saveUserPresets() {
    std::ofstream file(userPresetsPath_);
    if (!file.is_open()) {
        std::cerr << "Failed to save user presets to " << userPresetsPath_ << std::endl;
        return;
    }
    
    file << "{\n  \"presets\": [\n";
    
    bool first = true;
    for (const auto& preset : presets_) {
        if (!preset.isUserPreset) continue;
        
        if (!first) file << ",\n";
        first = false;
        
        file << "    {\n";
        file << "      \"name\": \"" << preset.name << "\",\n";
        file << "      \"category\": \"" << categoryToString(preset.category) << "\",\n";
        file << "      \"isFavorite\": " << (preset.isFavorite ? "true" : "false") << "\n";
        file << "    }";
    }
    
    file << "\n  ]\n}\n";
    file.close();
}

std::vector<Preset> PresetManager::getPresetsByCategory(PresetCategory cat) const {
    std::vector<Preset> result;
    for (const auto& preset : presets_) {
        if (preset.category == cat) {
            result.push_back(preset);
        }
    }
    return result;
}

std::vector<Preset> PresetManager::searchPresets(const std::string& query) const {
    std::vector<Preset> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    
    for (const auto& preset : presets_) {
        std::string lowerName = preset.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        
        if (lowerName.find(lowerQuery) != std::string::npos) {
            result.push_back(preset);
        }
    }
    return result;
}

void PresetManager::addUserPreset(const Preset& preset) {
    presets_.push_back(preset);
    saveUserPresets();
}

void PresetManager::deleteUserPreset(int index) {
    int userCount = 0;
    for (size_t i = 0; i < presets_.size(); ++i) {
        if (presets_[i].isUserPreset) {
            if (userCount == index) {
                presets_.erase(presets_.begin() + i);
                saveUserPresets();
                return;
            }
            userCount++;
        }
    }
}

void PresetManager::toggleFavorite(int index) {
    if (index >= 0 && index < static_cast<int>(presets_.size())) {
        presets_[index].isFavorite = !presets_[index].isFavorite;
        if (presets_[index].isUserPreset) {
            saveUserPresets();
        }
    }
}

std::optional<Preset> PresetManager::getPresetByName(const std::string& name) const {
    for (const auto& preset : presets_) {
        if (preset.name == name) {
            return preset;
        }
    }
    return std::nullopt;
}

std::string PresetManager::getUserPresetsPath() const {
    return userPresetsPath_;
}

const char* PresetManager::categoryToString(PresetCategory cat) {
    switch (cat) {
        case PresetCategory::Lead: return "Lead";
        case PresetCategory::Pad: return "Pad";
        case PresetCategory::Bass: return "Bass";
        case PresetCategory::Keys: return "Keys";
        case PresetCategory::FX: return "FX";
        case PresetCategory::Init: return "Init";
        case PresetCategory::User: return "User";
        default: return "Init";
    }
}

PresetCategory PresetManager::stringToCategory(const std::string& str) {
    if (str == "Lead") return PresetCategory::Lead;
    if (str == "Pad") return PresetCategory::Pad;
    if (str == "Bass") return PresetCategory::Bass;
    if (str == "Keys") return PresetCategory::Keys;
    if (str == "FX") return PresetCategory::FX;
    if (str == "User") return PresetCategory::User;
    return PresetCategory::Init;
}

std::string PresetManager::readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> PresetManager::split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

std::string PresetManager::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}
