#include "../include/CGame.h"    // Adjust path as per your structure
#include "../include/ProcessManager.h"  // For ProcessMgr
#include "../include/MemoryManager.h"   // For memory operations
#include "../include/Renderer.h"        // For rendering ESP
#include "../include/Entity.h"
#include "../include/Offsets.h"
#include "../include/ImGuiManager.h"
#include <iostream>
#include <vector>


std::vector<Entity> GetEntities(MemoryManager& memMgr, DWORD64 entityListAddress, int maxEntities) {
    std::vector<Entity> entities;
    for (int i = 0; i < maxEntities; ++i) {
        DWORD64 entityAddress;
        if (!memMgr.Read(entityListAddress + i * sizeof(DWORD64), &entityAddress, sizeof(DWORD64)) || entityAddress == 0) {
            continue;
        }

        Entity entity;
        entity.address = entityAddress;

        int team, health;
        if (!memMgr.Read(entityAddress + m_iTeamNum, &team, sizeof(int)) ||
            !memMgr.Read(entityAddress + m_iHealth, &health, sizeof(int))) {
            continue;
        }

        entity.team = team;
        entity.health = health;

        if (!memMgr.Read(entityAddress + m_vecOrigin, &entity.position, sizeof(entity.position))) {
            continue;
        }

        entities.push_back(entity);
    }
    return entities;
}


// Initialize memory addresses
bool CGame::InitAddress(MemoryManager& memMgr) {
    DWORD processID = ProcessMgr.GetProcessID();
    if (processID == 0) {
        std::wcerr << L"[ERROR] Failed to retrieve process ID." << std::endl;
        return false;
    }

    if (!memMgr.Attach(processID)) {
        std::wcerr << L"[ERROR] Failed to attach to process ID: " << processID << std::endl;
        return false;
    }

    this->Address.ClientDLL = ProcessMgr.GetProcessModuleHandle(L"client.dll");
    if (this->Address.ClientDLL == 0) {
        std::wcerr << L"[ERROR] Failed to locate client.dll. Exiting..." << std::endl;
        return false;
    }

    this->Address.EntityList = this->Address.ClientDLL + dwEntityList;
    this->Address.LocalPlayer = this->Address.ClientDLL + dwLocalPlayer;
    this->Address.ViewMatrix = this->Address.ClientDLL + dwViewMatrix;

    std::wcout << L"[INFO] Addresses initialized successfully!" << std::endl;
    return true;
}

void CGame::SetESPState(bool state) {
    isESPEnabled = state;
    std::wcout << L"[INFO] ESP state set to: " << (isESPEnabled ? L"Enabled" : L"Disabled") << std::endl;
}

void AlignOverlayWithGame(HWND overlayWindow, HWND gameWindow) {
    RECT gameRect;
    GetWindowRect(gameWindow, &gameRect);

    SetWindowPos(overlayWindow, HWND_TOPMOST, gameRect.left, gameRect.top,
        gameRect.right - gameRect.left, gameRect.bottom - gameRect.top,
        SWP_NOACTIVATE | SWP_NOSENDCHANGING);
}

void CGame::ProcessInput() {
    if (GetAsyncKeyState(VK_F1) & 0x1) { // Toggle ESP with F1
        SetESPState(!isESPEnabled);
    }

    if (GetAsyncKeyState(VK_ESCAPE) & 0x1) { // Stop game loop with Escape
        Stop();
    }
}


// Main game loop
void CGame::Run(MemoryManager& memMgr, HWND overlayWindow) {
    ImGuiManager imguiManager;
    

    // Initialize ImGuiManager
    if (!imguiManager.Initialize(overlayWindow)) {
        std::wcerr << L"[ERROR] Failed to initialize ImGuiManager." << std::endl;
        return;
    }

    while (isRunning) {
        HWND gameWindow = FindWindow(NULL, L"Counter-Strike 2");
        AlignOverlayWithGame(overlayWindow, gameWindow);

        if (!g_mainRenderTargetView) {
            std::wcerr << L"[ERROR] Render target view is null. Unable to render." << std::endl;
            break;
        }

        // Clear screen
        g_pd3dContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        FLOAT clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Dark gray
        g_pd3dContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);

        // ImGui rendering
        std::wcout << L"[DEBUG] Starting ImGui frame." << std::endl;
        imguiManager.BeginFrame();

        std::wcout << L"[DEBUG] Rendering ImGui UI." << std::endl;
        imguiManager.RenderUI(*this);

        std::wcout << L"[DEBUG] Ending ImGui frame." << std::endl;
        imguiManager.EndFrame();

        ProcessInput();

        // Present frame
        HRESULT hr = swapChain->Present(1, 0);
        if (FAILED(hr)) {
            std::wcerr << L"[ERROR] Swap chain present failed with HRESULT: " << std::hex << hr << std::endl;
            break;
        }
    }

    imguiManager.Cleanup();
    std::wcout << L"[INFO] Game loop exited successfully." << std::endl;
}

bool CGame::Initialize() {
    std::wcout << L"[INFO] Initializing game components..." << std::endl;
    return true;
}

void CGame::DrawESP(MemoryManager& memMgr) {
    for (int i = 0; i < 64; ++i) {
        DWORD64 entityAddress;
        if (!memMgr.Read(this->Address.EntityList + i * sizeof(DWORD64), &entityAddress, sizeof(DWORD64)) || entityAddress == 0) {
            continue;
        }

        int health;
        if (!memMgr.Read(entityAddress + m_iHealth, &health, sizeof(int)) || health <= 0) {
            continue;
        }

        float position[3];
        if (!memMgr.Read(entityAddress + m_vecOrigin, position, sizeof(position))) {
            continue;
        }

        Renderer::DrawBox(position[0], position[1], 50, 100, health);
    }
}


void CGame::Update(MemoryManager& memMgr) {
    const int MAX_ENTITIES = 128; // Replace with actual max entities
    const DWORD64 entityBaseOffset = 0x10; // Replace with actual offset

    std::cout << "[DEBUG] Updating entities..." << std::endl;
    for (int index = 0; index < MAX_ENTITIES; ++index) {
        DWORD64 entityAddress;
        if (!memMgr.Read(this->Address.EntityList + (index * entityBaseOffset), &entityAddress, sizeof(DWORD64)) || entityAddress == 0) {
            continue;
        }

        int health;
        if (!memMgr.Read(entityAddress + m_iHealth, &health, sizeof(int)) || health <= 0) {
            continue;
        }

        float position[3];
        if (!memMgr.Read(entityAddress + m_vecOrigin, position, sizeof(position))) {
            continue;
        }

        std::cout << "Entity " << index << ": Health = " << health
            << ", Position = [" << position[0] << ", " << position[1] << ", " << position[2] << "]" << std::endl;
    }
}


void CGame::Render() {
    // Placeholder for rendering additional visuals
    std::wcout << L"[INFO] Rendering frame..." << std::endl;
}

void CGame::Cleanup() {
    std::wcout << L"[INFO] Cleaning up resources..." << std::endl;
}