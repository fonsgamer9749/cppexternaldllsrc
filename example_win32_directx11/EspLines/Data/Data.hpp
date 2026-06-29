#pragma once
#include <cstdint>
#include <EspLines/Math/Vector/Vector3.hpp>
#include <EspLines/Player.h>
#include <src/Globals.hpp>
#include <EspLines/Memory/Memory.hpp>

namespace FWork {
    class Data {
    public:
        static void Work();
        static void FindClosestEnemy();
    private:
        static bool EntityData(uint32_t entity, Player& player, Vector3& mainPos);
        static void Reset();;
    };
}