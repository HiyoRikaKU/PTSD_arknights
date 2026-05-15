#ifndef ARKNIGHTS_MAP_STAGEREPOSITORY_HPP
#define ARKNIGHTS_MAP_STAGEREPOSITORY_HPP

#include <string>
#include <vector>

#include "Arknights/Map/StageDefinition.hpp"

namespace Arknights::Map {

class StageRepository {
public:
    static const std::vector<StageDefinition>& getAllStages();
    static const StageDefinition& getStageById(const std::string& stageId);
};

} // namespace Arknights::Map

#endif
