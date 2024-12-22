#include "Config.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>


namespace Config {
    // ESP Toggles
    bool teamESP = false;
    bool showSkeleton = true;
    bool showBox = true;
    bool showHealthBar = true;
    bool showName = true;
    bool showDistance = true;

    // Rendering Options
    int renderDistance = 1000;
    std::array<int, 3> boxColorTeam = { 0, 255, 0 };  // Green
    std::array<int, 3> boxColorEnemy = { 255, 0, 0 }; // Red

    // JSON Configuration Management
    bool loadFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file: " << filePath << std::endl;
            return false;
        }

        try {
            nlohmann::json configData;
            file >> configData;

            if (configData.contains("teamESP")) teamESP = configData["teamESP"];
            if (configData.contains("showSkeleton")) showSkeleton = configData["showSkeleton"];
            if (configData.contains("showBox")) showBox = configData["showBox"];
            if (configData.contains("showHealthBar")) showHealthBar = configData["showHealthBar"];
            if (configData.contains("showName")) showName = configData["showName"];
            if (configData.contains("showDistance")) showDistance = configData["showDistance"];
            if (configData.contains("renderDistance")) renderDistance = configData["renderDistance"];
            if (configData.contains("boxColorTeam")) boxColorTeam = configData["boxColorTeam"].get<std::array<int, 3>>();
            if (configData.contains("boxColorEnemy")) boxColorEnemy = configData["boxColorEnemy"].get<std::array<int, 3>>();

            file.close();
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading config file: " << e.what() << std::endl;
            return false;
        }
    }

    void saveToFile(const std::string& filePath) {
        try {
            nlohmann::json configData;

            configData["teamESP"] = teamESP;
            configData["showSkeleton"] = showSkeleton;
            configData["showBox"] = showBox;
            configData["showHealthBar"] = showHealthBar;
            configData["showName"] = showName;
            configData["showDistance"] = showDistance;
            configData["renderDistance"] = renderDistance;
            configData["boxColorTeam"] = boxColorTeam;
            configData["boxColorEnemy"] = boxColorEnemy;

            std::ofstream file(filePath);
            file << configData.dump(4); // Pretty print with 4 spaces
            file.close();
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving config file: " << e.what() << std::endl;
        }
    }
}