#include "Arknights/Map/StageRepository.hpp"

#include <algorithm>
#include <stdexcept>

#include "config.hpp"

namespace Arknights::Map {

namespace {

constexpr Operation::TileType x = Operation::TileType::OBSTACLE;
constexpr Operation::TileType f = Operation::TileType::GROUND;
constexpr Operation::TileType h = Operation::TileType::HIGH_GROUND;
constexpr Operation::TileType r = Operation::TileType::SPAWN;
constexpr Operation::TileType b = Operation::TileType::BASE;

const std::vector<std::vector<Operation::TileType>> s_Stage02MapMatrix = {
    {x, h, h, h, h, h, h, h, b, x},
    {x, h, f, f, f, f, h, f, f, r},
    {b, f, f, h, f, f, f, f, f, r},
    {x, h, h, h, h, h, h, h, x, x},
};

std::vector<glm::vec2> makeStage02Waypoints() {
    const glm::vec2 spawnPos = {1.5f, 9.5f};
    const std::vector<glm::vec2> steps = {
        {-1,  0}, {-1,  0},
        { 0, -1},
        {-1,  0}, {-1,  0},
        {-1,  1},
        {-1,  0}, {-1,  0},
        { 0, -1},
        {-1,  0}, {-1,  0}
    };

    std::vector<glm::vec2> waypoints;
    waypoints.push_back(spawnPos);
    glm::vec2 currentPos = spawnPos;
    for (const auto& step : steps) {
        currentPos.x -= step.y;
        currentPos.y += step.x;
        waypoints.push_back(currentPos);
    }
    return waypoints;
}

const std::array<glm::vec2, 4> s_Stage02SrcPoints = {
    glm::vec2{1, 0}, {7, 0}, {1, 3}, {7, 3}
};

const std::array<glm::vec2, 4> s_Stage02DstPoints = {
    glm::vec2{-493, 253}, {255, 247}, {-574, -86}, {295, -87}
};

const std::vector<SpawnEvent> s_Stage02Wave = {
    {2000.0f, "gopro", 150.0f, 0.001f},
    {5000.0f, "gopro", 150.0f, 0.001f},
    {7000.0f, "gopro", 150.0f, 0.001f},
    {12000.0f, "bigbo", 400.0f, 0.0005f},
    {14000.0f, "gopro", 170.0f, 0.0011f},
    {16000.0f, "gopro", 170.0f, 0.0011f},
    {16000.0f, "gopro", 170.0f, 0.0011f},
    {22000.0f, "trslim", 100.0f, 0.0018f},
    {23000.0f, "trslim", 100.0f, 0.0018f},
    {24000.0f, "trslim", 100.0f, 0.0018f},
    {25000.0f, "trslim", 100.0f, 0.0018f},
    {32000.0f, "bigbo", 500.0f, 0.0004f},
    {33000.0f, "trslim", 120.0f, 0.0015f},
    {34000.0f, "trslim", 120.0f, 0.0015f},
    {37000.0f, "bigbo", 500.0f, 0.0004f},
    {45000.0f, "bigbo", 1200.0f, 0.0003f},
    {46000.0f, "trslim", 150.0f, 0.002f},
    {47000.0f, "trslim", 150.0f, 0.002f},
    {48000.0f, "trslim", 150.0f, 0.002f},
    {49000.0f, "trslim", 150.0f, 0.002f},
};

const std::vector<StageDefinition>& getStageData() {
    static const std::vector<StageDefinition> stages = {
        StageDefinition{
            "0-2",
            "戰術演習",
            std::string(RESOURCE_DIR) + "/map/operation1.jpg",
            s_Stage02MapMatrix,
            makeStage02Waypoints(),
            StageHomographyDefinition{
                s_Stage02SrcPoints,
                s_Stage02DstPoints
            },
            s_Stage02Wave
        }
    };
    return stages;
}

} // namespace

const std::vector<StageDefinition>& StageRepository::getAllStages() {
    return getStageData();
}

const StageDefinition& StageRepository::getStageById(const std::string& stageId) {
    const auto& stages = getStageData();
    const auto it = std::find_if(stages.begin(), stages.end(), [&stageId](const StageDefinition& definition) {
        return definition.id == stageId;
    });
    if (it == stages.end()) {
        throw std::runtime_error("Unknown stage id: " + stageId);
    }
    return *it;
}

} // namespace Arknights::Map
