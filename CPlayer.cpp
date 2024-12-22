#include "CPlayer.hpp"
#include "Config.hpp"
#include <string>
#include <cmath>


bool CPlayer::update(HANDLE hProcess, uintptr_t baseClient, int index) {
    uintptr_t entityAddress;
    if (!ReadProcessMemory(hProcess, (LPCVOID)(baseClient + DW_ENTITY_LIST + index * 0x10), &entityAddress, sizeof(entityAddress), nullptr) || entityAddress == 0) {
        isValid = false;
        return false;
    }

    entity = entityAddress;

    // Read health
    if (!ReadProcessMemory(hProcess, (LPCVOID)(entity + M_IHEALTH), &health, sizeof(health), nullptr) || health <= 0) {
        isValid = false;
        return false;
    }

    // Read position
    if (!ReadProcessMemory(hProcess, (LPCVOID)(entity + M_VECORIGIN), &position, sizeof(position), nullptr)) {
        isValid = false;
        return false;
    }

    // Read head position (add height to position)
    head = position;
    head.z += 75.0f; // Adjust this based on game-specific offsets

    // Simulate reading player name (or use your method to fetch names)
    name = "Player_" + std::to_string(index);

    isValid = true;
    return true;
}

void CPlayer::render(const ViewMatrix& viewMatrix) {
    if (!isValid) return;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    Vector3 screenPos, screenHead;
    if (!WorldToScreen(position, screenPos, viewMatrix, screenWidth, screenHeight) ||
        !WorldToScreen(head, screenHead, viewMatrix, screenWidth, screenHeight)) {
        return;
    }

    float height = screenPos.y - screenHead.y;
    float width = height / 2.4f;

    // ESP Box
    if (Config::showBox) {
        ImGui::GetForegroundDrawList()->AddRect(
            ImVec2(screenHead.x - width / 2, screenHead.y),
            ImVec2(screenHead.x + width / 2, screenPos.y),
            IM_COL32(255, 0, 0, 255)
        );
    }

    // Health Bar
    if (Config::showHealthBar) {
        float healthHeight = height * (health / 100.0f);
        ImGui::GetForegroundDrawList()->AddRectFilled(
            ImVec2(screenHead.x - (width / 2 + 5), screenPos.y - healthHeight),
            ImVec2(screenHead.x - (width / 2 + 3), screenPos.y),
            IM_COL32(0, 255, 0, 255)
        );
    }

    // Name and Distance
    if (Config::showName || Config::showDistance) {
        std::string label = Config::showName ? name : "";
        if (Config::showDistance) {
            float distance = sqrt(pow(position.x, 2) + pow(position.y, 2) + pow(position.z, 2)); // Distance calculation
            label += " (" + std::to_string(static_cast<int>(distance)) + "m)";
        }
        ImGui::GetForegroundDrawList()->AddText(
            ImVec2(screenHead.x - width / 2, screenHead.y - 15),
            IM_COL32(255, 255, 255, 255),
            label.c_str()
        );
    }
}

bool CPlayer::WorldToScreen(Vector3 pos, Vector3& screen, ViewMatrix matrix, int screenWidth, int screenHeight) {
    float w = matrix.matrix[3][0] * pos.x + matrix.matrix[3][1] * pos.y + matrix.matrix[3][2] * pos.z + matrix.matrix[3][3];
    if (w < 0.1f) return false;

    screen.x = (matrix.matrix[0][0] * pos.x + matrix.matrix[0][1] * pos.y + matrix.matrix[0][2] * pos.z + matrix.matrix[0][3]) / w;
    screen.y = (matrix.matrix[1][0] * pos.x + matrix.matrix[1][1] * pos.y + matrix.matrix[1][2] * pos.z + matrix.matrix[1][3]) / w;

    screen.x = (screenWidth / 2.0f) * (1.0f + screen.x);
    screen.y = (screenHeight / 2.0f) * (1.0f - screen.y);

    return true;
}