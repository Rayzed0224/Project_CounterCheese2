#include "../include/CGame.h"    // Adjust path as per your structure
#include "../include/ProcessManager.h"  // For ProcessMgr
#include "../include/MemoryManager.h"   // For memory operations
#include "../include/Renderer.h"        // For rendering ESP
#include "../include/Entity.h"
#include <iostream>
#include <vector>

std::vector<Entity> GetEntities(DWORD64 entityListAddress, int maxEntities) {
    std::vector<Entity> entities;
    for (int i = 0; i < maxEntities; ++i) {
        DWORD64 entityAddress = MemoryManager::Read<DWORD64>(entityListAddress + i * sizeof(DWORD64));
        if (entityAddress == 0) continue;

        Entity entity;
        entity.address = entityAddress;
        entity.team = MemoryManager::Read<int>(entityAddress + 0x3E3); // Replace with actual offset
        entity.health = MemoryManager::Read<int>(entityAddress + 0x344); // Replace with actual offset
        MemoryManager::Read(entityAddress + 0x88, &entity.position, sizeof(entity.position)); // Replace with actual offset

        entities.push_back(entity);
    }
    return entities;
}

bool CGame::InitAddress() {
    // Fetch ClientDLL base address
    this->Address.ClientDLL = ProcessMgr.GetProcessModuleHandle(L"client.dll");
    if (this->Address.ClientDLL == 0) {
        std::wcerr << L"[ERROR] Failed to locate client.dll. Exiting..." << std::endl;
        return false;
    }

    // Initialize key offsets (replace these with YOUR offsets)
    this->Address.EntityList = this->Address.ClientDLL + 0x1A146E8; // Replace with your EntityList offset
    this->Address.LocalPlayer = this->Address.ClientDLL + 0x1868CC8; // Replace with your LocalPlayer offset

    // Validate calculated addresses
    if (Address.EntityList == 0 || Address.LocalPlayer == 0) {
        std::wcerr << L"[ERROR] Calculated addresses are invalid. Exiting..." << std::endl;
        return false;
    }

    std::wcout << L"[INFO] Game addresses initialized successfully!" << std::endl;
    return true;
}

void CGame::Run() {
    while (isRunning) {
        if (!isESPEnabled) continue;

        std::vector<Entity> entities = GetEntities();

        // Fetch view matrix
        float viewMatrix[16];
        MemoryManager::Read(Address.ClientDLL + 0x1A7F610, &viewMatrix, sizeof(viewMatrix)); // Replace with your view matrix offset

        // Render ESP
        Renderer::RenderESP(entities, viewMatrix, 1920, 1080); // Replace with your screen resolution
    }
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

void CGame::DrawESP() {
    DWORD64 entityList = this->Address.EntityList;
    DWORD64 localPlayer = this->Address.LocalPlayer;

    for (int i = 0; i < 64; ++i) { // Loop through entities
        DWORD64 entity = MemoryManager::Read<DWORD64>(entityList + i * 0x10);

        if (entity == 0) continue; // Skip invalid entities

        int health = MemoryManager::Read<int>(entity + 100); // Replace with your health offset
        if (health <= 0) continue;

        float posX = MemoryManager::Read<float>(entity + 308); // Replace with your position offsets
        float posY = MemoryManager::Read<float>(entity + 312);
        float posZ = MemoryManager::Read<float>(entity + 316);

        // Render ESP box
        Renderer::DrawBox(posX, posY, 50, 100, health);
    }
}

void CGame::Update() {
    const int MAX_ENTITIES = 128; // Example max entities, replace with actual value
    const DWORD64 entityBaseOffset = 0x10; // Replace with actual offset

    for (int index = 0; index < MAX_ENTITIES; ++index) {
        DWORD64 entityAddress = MemoryManager::Read<DWORD64>(Address.EntityList + (index * entityBaseOffset));
        int health = MemoryManager::Read<int>(entityAddress + 0x20); // Replace 0x20 with health offset
        float xPos = MemoryManager::Read<float>(entityAddress + 0x50); // Replace 0x50 with X position offset

        std::cout << "Entity " << index << ": Health = " << health << ", Position X = " << xPos << std::endl;
    }
}

void CGame::Render() {
    // Placeholder for rendering additional visuals
    std::wcout << L"[INFO] Rendering frame..." << std::endl;
}

void CGame::Cleanup() {
    std::wcout << L"[INFO] Cleaning up resources..." << std::endl;
}