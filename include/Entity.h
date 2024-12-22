#pragma once
#include <string>
#include <cstdint>


struct Entity {
    DWORD64 address;
    int team;
    int health;
    float position[3];
};

