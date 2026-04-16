#include "pch.hpp"
#include "Arknights/Map/Operation1.hpp"
#include <vector>
#include <string>

namespace Arknights {

namespace {
    constexpr Operation::TileType x = Operation::TileType::OBSTACLE;
    constexpr Operation::TileType f = Operation::TileType::GROUND;
    constexpr Operation::TileType h = Operation::TileType::HIGH_GROUND;
    constexpr Operation::TileType r = Operation::TileType::SPAWN;
    constexpr Operation::TileType b = Operation::TileType::BASE;

    const std::vector<std::vector<Operation::TileType>> s_MapMatrix = {
        {x, h, h, h, h, h, h, h, b, x},
        {x, h, f, f, f, f, h, f, f, r},
        {b, f, f, h, f, f, f, f, f, r},
        {x, h, h, h, h, h, h, h, x, x},
    };

    std::vector<glm::vec2> CalculateWaypoints() {
        glm::vec2 spawnPos = {1.5f, 9.5f}; // {row, col}
        std::vector<glm::vec2> steps = {
            {-1,  0}, {-1,  0}, // left left
            { 0, -1},           // down
            {-1,  0}, {-1,  0}, // left left
            {-1,  1},           // up-left
            {-1,  0}, {-1,  0}, // left left
            { 0, -1},           // down
            {-1,  0}, {-1,  0}  // left left (disappear)
        };

        std::vector<glm::vec2> waypoints;
        waypoints.push_back(spawnPos);
        glm::vec2 currentPos = spawnPos;
        for (const auto& s : steps) {
            currentPos.x -= s.y; // +y is Up -> Row decreases
            currentPos.y += s.x; // +x is Right -> Col increases
            waypoints.push_back(currentPos);
        }
        return waypoints;
    }

    const std::array<glm::vec2, 4> s_SrcPoints = {
        glm::vec2{1, 0}, {7, 0}, {1, 3}, {7, 3}
    };

    const std::array<glm::vec2, 4> s_DstPoints = {
        glm::vec2{-493, 253}, {255, 247}, {-574, -86}, {295, -87}
    };
}

Operation1::Operation1() 
    : Operation(std::string(RESOURCE_DIR) + "/map/operation1.jpg", 
                CalculateWaypoints(), 
                s_MapMatrix, 
                s_SrcPoints, 
                s_DstPoints) {
    
    auto& wm = getWaveManager();
    // Wave 1: Intro
    wm.addSpawnEvent(2000.0f, "gopro", 150.0f, 0.001f);
    wm.addSpawnEvent(5000.0f, "gopro", 150.0f, 0.001f);
    wm.addSpawnEvent(7000.0f, "gopro", 150.0f, 0.001f);

    // Wave 2: Bigbo arrival
    wm.addSpawnEvent(12000.0f, "bigbo", 400.0f, 0.0005f);
    wm.addSpawnEvent(14000.0f, "gopro", 170.0f, 0.0011f);
    wm.addSpawnEvent(16000.0f, "gopro", 170.0f, 0.0011f);
    wm.addSpawnEvent(16000.0f, "gopro", 170.0f, 0.0011f);

    // Wave 3: Trslim (fast and weak)
    wm.addSpawnEvent(22000.0f, "trslim", 100.0f, 0.0018f);
    wm.addSpawnEvent(23000.0f, "trslim", 100.0f, 0.0018f);
    wm.addSpawnEvent(24000.0f, "trslim", 100.0f, 0.0018f);
    wm.addSpawnEvent(25000.0f, "trslim", 100.0f, 0.0018f);

    // Wave 4: Mixed assault
    wm.addSpawnEvent(32000.0f, "bigbo", 500.0f, 0.0004f);
    wm.addSpawnEvent(33000.0f, "trslim", 120.0f, 0.0015f);
    wm.addSpawnEvent(34000.0f, "trslim", 120.0f, 0.0015f);
    wm.addSpawnEvent(37000.0f, "bigbo", 500.0f, 0.0004f);

    // Final Wave
    wm.addSpawnEvent(45000.0f, "bigbo", 1200.0f, 0.0003f); // Boss
    wm.addSpawnEvent(46000.0f, "trslim", 150.0f, 0.002f);
    wm.addSpawnEvent(47000.0f, "trslim", 150.0f, 0.002f);
    wm.addSpawnEvent(48000.0f, "trslim", 150.0f, 0.002f);
    wm.addSpawnEvent(49000.0f, "trslim", 150.0f, 0.002f);
}

} // namespace Arknights
