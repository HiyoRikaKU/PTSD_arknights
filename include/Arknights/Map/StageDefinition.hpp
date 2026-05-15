#ifndef ARKNIGHTS_MAP_STAGEDEFINITION_HPP
#define ARKNIGHTS_MAP_STAGEDEFINITION_HPP

#include <array>
#include <string>
#include <vector>

#include "Arknights/Map/Operation.hpp"
 
namespace Arknights::Map {

struct StageHomographyDefinition {
    std::array<glm::vec2, 4> srcPoints;
    std::array<glm::vec2, 4> dstPoints;
};

struct StageDefinition {
    std::string id;
    std::string name;
    std::string mapImagePath;
    std::vector<std::vector<Operation::TileType>> tileMatrix;
    std::vector<glm::vec2> waypoints;
    StageHomographyDefinition homography;
    std::vector<SpawnEvent> wave;
};

} // namespace Arknights::Map

#endif
