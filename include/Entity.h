#pragma once
#include <string>
#include <cstdint>
#include "Vector3.h" // Assuming you have a Vector3 structure

struct Entity {
    DWORD64 address; // Base address of the entity
    int team;        // Entity's team ID
    int health;      // Entity's health
    Vector3 position; // World position (3D)
};

