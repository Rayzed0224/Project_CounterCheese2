#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <array>
#include <string>

namespace Config {
    // ESP Toggles
    extern bool teamESP;
    extern bool showSkeleton;
    extern bool showBox;
    extern bool showHealthBar;
    extern bool showName;
    extern bool showDistance;

    // Rendering Options
    extern int renderDistance;
    extern std::array<int, 3> boxColorTeam;  // RGB
    extern std::array<int, 3> boxColorEnemy; // RGB

    // Methods for Configuration Management
    bool loadFromFile(const std::string& filePath);
    void saveToFile(const std::string& filePath);
}

// Offsets
#define DW_VIEW_MATRIX 0x1A7F610
#define DW_ENTITY_LIST 0x1A146E8
#define DW_LOCAL_PLAYER 0x1868CC8
#define M_IHEALTH 0x344
#define M_VECORIGIN 0x1324

#endif