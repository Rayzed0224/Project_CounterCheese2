#include "../include/Renderer.h"
#include "../imgui/backends/imgui_impl_dx11.h"  // Ensure ImGui is properly set up
#include "../imgui/imgui.h"
#include "../include/MemoryManager.h"
#include "../include/CGame.h"
#include "../include/Entity.h"
#include "../include/Offsets.h"
#include <d3d11.h>  // DirectX 11 header
#include <windows.h>
#include <string>
#include <vector>
#include <cmath>

extern ID3D11Device* g_pd3dDevice;          // DirectX device
extern ID3D11DeviceContext* g_pd3dContext; // DirectX device context

// Declare external Address instance from CGame
extern CGame::Addresses Address;

// Local player team variable
int localPlayerTeam = 0;

void Renderer::DrawBox(float x, float y, float width, float height, int color) {
    // Convert color to ImGui format
    ImU32 imguiColor = IM_COL32((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);

    // Use ImGui to draw the box
    ImGui::GetForegroundDrawList()->AddRect(
        ImVec2(x, y),                      // Top-left corner
        ImVec2(x + width, y + height),     // Bottom-right corner
        imguiColor,                        // Color
        0.0f,                              // Rounding
        ImDrawFlags_None,                  // Flags (e.g., rounded corners)
        2.0f                               // Thickness
    );
}

void Renderer::DrawLine(float x1, float y1, float x2, float y2, int color) {
    // Convert color to ImGui format
    ImU32 imguiColor = IM_COL32((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);

    // Use ImGui to draw the line
    ImGui::GetForegroundDrawList()->AddLine(
        ImVec2(x1, y1),
        ImVec2(x2, y2),
        imguiColor,
        2.0f // Line thickness
    );
}

// Helper function: Convert std::wstring to std::string (UTF-8)
std::string WStringToUTF8(const std::wstring& wideString) {
    if (wideString.empty()) return {};

    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8String(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &utf8String[0], sizeNeeded, nullptr, nullptr);
    return utf8String;
}

// Function implementation
void Renderer::RenderText(float x, float y, const wchar_t* text, int color) {
    // Convert wide text to UTF-8
    std::string utf8Text = WStringToUTF8(std::wstring(text));

    // Convert color to ImGui format
    ImU32 imguiColor = IM_COL32((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);

    // Use ImGui to draw the text
    ImGui::GetForegroundDrawList()->AddText(
        ImVec2(x, y),
        imguiColor,
        utf8Text.c_str()
    );
}

bool WorldToScreen(const float* worldPos, float* screenPos, const float* viewMatrix, int screenWidth, int screenHeight) {
    float w = viewMatrix[3] * worldPos[0] + viewMatrix[7] * worldPos[1] + viewMatrix[11] * worldPos[2] + viewMatrix[15];
    if (w < 0.1f) return false;

    screenPos[0] = (viewMatrix[0] * worldPos[0] + viewMatrix[4] * worldPos[1] + viewMatrix[8] * worldPos[2] + viewMatrix[12]) / w;
    screenPos[1] = (viewMatrix[1] * worldPos[0] + viewMatrix[5] * worldPos[1] + viewMatrix[9] * worldPos[2] + viewMatrix[13]) / w;

    screenPos[0] = (screenWidth / 2.0f) + (screenWidth / 2.0f) * screenPos[0];
    screenPos[1] = (screenHeight / 2.0f) - (screenHeight / 2.0f) * screenPos[1];

    return true;
}

void Renderer::RenderESP(const std::vector<Entity>& entities, const float* viewMatrix, int screenWidth, int screenHeight) {
    for (const auto& entity : entities) {
        if (entity.health <= 0) continue; // Skip dead entities

        // Skip teammates
        if (entity.team == localPlayerTeam) continue;

        float screenPos[2];
        if (WorldToScreen(entity.position, screenPos, viewMatrix, screenWidth, screenHeight)) {
            // Draw a red box for enemies
            DrawBox(screenPos[0] - 25, screenPos[1] - 50, 50, 100, 0xFF0000);

            // Draw a green health bar
            DrawBox(screenPos[0] - 30, screenPos[1] - 50, 5, 100 * (entity.health / 100.0f), 0x00FF00);
        }
    }
}