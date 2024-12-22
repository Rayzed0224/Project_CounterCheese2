#include "../include/ImGuiManager.h"
#include "../imgui/imgui.h"                  // Core ImGui
#include "../imgui/backends/imgui_impl_dx11.h"        // ImGui DirectX11 backend
#include "../imgui/backends/imgui_impl_win32.h"       // ImGui Win32 backend
#include <Windows.h>
#include <iostream>

bool ImGuiManager::Initialize() {
    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Initialize ImGui backends
    if (!ImGui_ImplWin32_Init(GetActiveWindow())) { // Replace GetActiveWindow with your target window handle if necessary
        std::wcerr << L"[ERROR] ImGui Win32 initialization failed." << std::endl;
        return false;
    }
    if (!ImGui_ImplDX11_Init(nullptr, nullptr)) { // Replace nullptrs with your DirectX device and context
        std::wcerr << L"[ERROR] ImGui DirectX11 initialization failed." << std::endl;
        return false;
    }

    std::wcout << L"[INFO] ImGui initialized successfully." << std::endl;
    return true;
}

void ImGuiManager::BeginFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::RenderUI() {
    ImGui::Begin("ESP Settings");
    static bool enableESP = true;
    ImGui::Checkbox("Enable ESP", &enableESP); // Toggle ESP rendering
    ImGui::End();

    // Pass ESP toggle state to your game class
    // Example: CGame::SetESPState(enableESP);
}

void ImGuiManager::EndFrame() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::Cleanup() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    std::wcout << L"[INFO] ImGui cleaned up successfully." << std::endl;
}
