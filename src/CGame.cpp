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



// Main game loop
void CGame::Run(MemoryManager& memMgr) {
    ImGuiManager imguiManager;
    std::wcerr << L"[Info] Calling run" << std::endl;

    // Initialize ImGuiManager
    if (!imguiManager.Initialize()) {
        std::wcerr << L"[ERROR] Failed to initialize ImGuiManager." << std::endl;
        return;
    }

    try {
        while (isRunning) {
            std::wcout << L"[DEBUG] New frame started." << std::endl;

            imguiManager.BeginFrame();
            imguiManager.RenderUI(*this);
            imguiManager.EndFrame();

            if (isESPEnabled) {
                std::vector<Entity> entities = GetEntities(memMgr, this->Address.EntityList, 64);

                float viewMatrix[16];
                if (!memMgr.Read(this->Address.ViewMatrix, viewMatrix, sizeof(viewMatrix))) {
                    std::wcerr << L"[ERROR] Failed to read view matrix." << std::endl;
                    continue;
                }

                Renderer::RenderESP(entities, viewMatrix, 1920, 1080);
                std::wcout << L"[DEBUG] ESP rendered." << std::endl;
            }

            swapChain->Present(1, 0);
            std::wcout << L"[DEBUG] Frame presented." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::wcerr << L"[ERROR] Exception caught: " << e.what() << std::endl;
    }
    imguiManager.Cleanup();
    std::wcout << L"[INFO] Run method exited." << std::endl;
}

bool CGame::Initialize() {
    std::wcout << L"[INFO] Initializing game components..." << std::endl;
    return true;
}

bool CGame::ProcessInput() {
    if (GetAsyncKeyState(VK_F1)) {
        std::wcout << L"[INFO] Toggling ESP on/off." << std::endl;
        isESPEnabled = !isESPEnabled;
    }

    if (GetAsyncKeyState(VK_ESCAPE)) {
        std::wcout << L"[INFO] Escape key pressed. Exiting game loop." << std::endl;
        return false;
    }

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