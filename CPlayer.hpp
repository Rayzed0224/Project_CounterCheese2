#ifndef CPLAYER_HPP
#define CPLAYER_HPP

#include <d3d11.h>
#include "imgui/imgui.h"
#include "Config.hpp"
#include "Utils.hpp"

class CPlayer {
public:
    uintptr_t entity = 0;           // Initialize with 0
    Vector3 position = {};          // Default initialize
    Vector3 head = {};              // Default initialize
    int health = 0;                 // Initialize with 0
    bool isValid = false;           // Initialize with false
    std::string name = "";          // Initialize with an empty string

    bool update(HANDLE hProcess, uintptr_t baseClient, int index);
    void render(const ViewMatrix& viewMatrix);

private:
    bool WorldToScreen(Vector3 pos, Vector3& screen, ViewMatrix matrix, int screenWidth, int screenHeight);
};

#endif