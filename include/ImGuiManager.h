#pragma once

class ImGuiManager {
public:
    static bool Initialize();   // Initialize ImGui
    static void BeginFrame();   // Start a new ImGui frame
    static void RenderUI();     // Render ImGui UI elements
    static void EndFrame();     // End the ImGui frame and render
    static void Cleanup();      // Cleanup ImGui resources
};

