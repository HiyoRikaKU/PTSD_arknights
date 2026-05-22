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

const std::vector<std::vector<Operation::TileType>> s_Stage03MapMatrix = {
    {x, x, x, x, x, x, x, x, x, x},
    {x, h, h, h, h, h, h, h, h, x},
    {r, h, f, f, h, f, f, f, f, x},
    {r, f, f, f, f, f, h, h, f, x},
    {x, h, h, h, h, h, h, h, f, x},
    {x, x, x, x, x, x, x, x, b, x},
};

const std::vector<std::vector<Operation::TileType>> s_Stage04MapMatrix = {
    {x, x, x, x, x, x, x, x, x, x},
    {r, x, h, h, h, h, h, h, h, x},
    {x, x, f, f, f, f, f, f, h, x},
    {x, f, f, f, h, h, f, f, f, x},
    {r, f, f, h, h, h, h, f, f, b},
    {x, x, h, h, h, h, h, h, h, x},
    {x, x, x, x, x, x, x, x, x, x},
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

std::vector<glm::vec2> makeStage03Waypoints() {
    // Stage 0-3 enemy path must stay on GROUND(f) tiles only.
    const glm::vec2 spawnPos = {3.5f, 0.5f};
    const std::vector<glm::vec2> steps = {
        {1,  0}, {1,  0}, {1,  0}, {1,  0}, {1,  0}, 
        {0,  1}, 
        {1,  0}, {1, 0}, {1, 0}, 
        {0, -1}, {0, -1}, {0, -1}
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

std::vector<glm::vec2> makeStage04Waypoints() {
    // Stage 0-4 enemy path must stay on GROUND(f) tiles only.
    const glm::vec2 spawnPos = {4.5f, 0.5f};
    const std::vector<glm::vec2> steps = {
        {1,  0}, {1,  0},
        {0,  1}, 
        {1,  0},
        {0,  1},
        {1,  0}, {1,  0},{1,  0}, {1,  0},
        {0,  -1},{0,  -1},
        {1,  0}
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

const std::array<glm::vec2, 4> s_Stage03SrcPoints = {
    glm::vec2{1, 0}, {8, 0}, {1, 5}, {8, 5}
};

const std::array<glm::vec2, 4> s_Stage03DstPoints = {
    glm::vec2{-513, 373}, {357, 366}, {-664, -261}, {460, -255}
};

const std::array<glm::vec2, 4> s_Stage04SrcPoints = {
    glm::vec2{1, 0.5}, {8, 0.5}, {1, 5.5}, {8, 5.5}
};

const std::array<glm::vec2, 4> s_Stage04DstPoints = {
    glm::vec2{-513, 373}, {357, 366}, {-664, -261}, {460, -255}
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

const std::vector<SpawnEvent> s_Stage03Wave = {
    // Wave 1: trslim * 4
    { 2000.0f, "trslim", 100.0f, 0.0018f},
    { 3000.0f, "trslim", 100.0f, 0.0018f},
    { 4000.0f, "trslim", 100.0f, 0.0018f},

    // Wave 2: trslim * 5 + bigbo * 2
    { 9000.0f, "trslim", 110.0f, 0.0018f},
    {11000.0f, "trslim", 110.0f, 0.0018f},
    {13000.0f, "trslim", 110.0f, 0.0018f},
    {14000.0f, "trslim", 110.0f, 0.0018f},
    {11500.0f, "bigbo",  420.0f, 0.0005f},
    {14500.0f, "bigbo",  420.0f, 0.0005f},

    // Wave 3: trslim * 5 + bigbo * 3 + yokai * 2
    {20000.0f, "trslim", 120.0f, 0.0019f},
    {21000.0f, "trslim", 120.0f, 0.0019f},
    {22000.0f, "trslim", 120.0f, 0.0019f},
    {23000.0f, "trslim", 120.0f, 0.0019f},
    {19500.0f, "bigbo",  520.0f, 0.0004f},
    {21500.0f, "bigbo",  520.0f, 0.0004f},
    {23500.0f, "bigbo",  520.0f, 0.0004f},
    {20500.0f, "yokai",  360.0f, 0.0012f},

    // Wave 4: trslim * 5 + gopro * 4
    {31000.0f, "trslim", 140.0f, 0.0020f},
    {32000.0f, "trslim", 140.0f, 0.0020f},
    {33000.0f, "trslim", 140.0f, 0.0020f},
    {34000.0f, "trslim", 140.0f, 0.0020f},
    {30500.0f, "gopro",  170.0f, 0.0011f},
    {32500.0f, "gopro",  170.0f, 0.0011f},
    {33500.0f, "gopro",  170.0f, 0.0011f},
    {30000.0f, "yokai",  420.0f, 0.0013f},

    // Wave 5: yokai focus
    {40000.0f, "yokai",  480.0f, 0.0014f},
    {46000.0f, "bigbo",  650.0f, 0.0004f},
};

const std::vector<SpawnEvent> s_Stage04Wave = {
    // Wave 1: trslim * 4
    { 2000.0f, "trslim", 100.0f, 0.0018f},
    { 3000.0f, "trslim", 100.0f, 0.0018f},

    { 5000.0f, "trslim", 100.0f, 0.0018f},
    { 5500.0f, "trslim", 100.0f, 0.0018f},

    // Wave 2: trslim * 5 + bigbo * 2
    { 9000.0f, "trslim", 110.0f, 0.0018f},
    {11000.0f, "trslim", 110.0f, 0.0018f},
    {13000.0f, "trslim", 110.0f, 0.0018f},
    {14000.0f, "trslim", 110.0f, 0.0018f},
    {11500.0f, "bigbo",  420.0f, 0.0005f},
    {14500.0f, "bigbo",  420.0f, 0.0005f},

    // Wave 3: trslim * 5 + bigbo * 3 + yokai * 2
    {20000.0f, "trslim", 120.0f, 0.0019f},
    {21000.0f, "trslim", 120.0f, 0.0019f},
    {22000.0f, "trslim", 120.0f, 0.0019f},
    {23000.0f, "trslim", 120.0f, 0.0019f},
    {19500.0f, "bigbo",  520.0f, 0.0004f},
    {21500.0f, "bigbo",  520.0f, 0.0004f},
    {23500.0f, "bigbo",  520.0f, 0.0004f},
    {20500.0f, "yokai",  360.0f, 0.0012f},

    // Wave 4: trslim * 5 + gopro * 4
    {31000.0f, "trslim", 140.0f, 0.0020f},
    {32000.0f, "trslim", 140.0f, 0.0020f},
    {33000.0f, "trslim", 140.0f, 0.0020f},
    {34000.0f, "trslim", 140.0f, 0.0020f},
    {30500.0f, "gopro",  170.0f, 0.0011f},
    {32500.0f, "gopro",  170.0f, 0.0011f},
    {33500.0f, "gopro",  170.0f, 0.0011f},
    {30000.0f, "yokai",  420.0f, 0.0013f},

    // Wave 5: yokai focus
    {40000.0f, "yokai",  480.0f, 0.0014f},
    {46000.0f, "bigbo",  650.0f, 0.0004f},
};

const std::vector<StageDefinition>& getStageData() {
    static const std::vector<StageDefinition> stages = {
        StageDefinition{
            "0-2",
            "守衛",
            std::string(RESOURCE_DIR) + "/map/operation0-2.jpg",
            s_Stage02MapMatrix,
            makeStage02Waypoints(),
            StageHomographyDefinition{
                s_Stage02SrcPoints,
                s_Stage02DstPoints
            },
            s_Stage02Wave
        },
        StageDefinition{
            "0-3",
            "追擊",
            std::string(RESOURCE_DIR) + "/map/operation0-3.png",
            s_Stage03MapMatrix,
            makeStage03Waypoints(),
            StageHomographyDefinition{
                s_Stage03SrcPoints,
                s_Stage03DstPoints
            },
            s_Stage03Wave
        },
        StageDefinition{
            "0-4",
            "混戰",
            std::string(RESOURCE_DIR) + "/map/operation0-4.png",
            s_Stage04MapMatrix,
            makeStage04Waypoints(),
            StageHomographyDefinition{
                s_Stage04SrcPoints,
                s_Stage04DstPoints
            },
            s_Stage04Wave
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
