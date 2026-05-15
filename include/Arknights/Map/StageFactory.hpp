#ifndef ARKNIGHTS_MAP_STAGEFACTORY_HPP
#define ARKNIGHTS_MAP_STAGEFACTORY_HPP

#include <memory>
#include <string>

#include "Arknights/Map/Operation.hpp"
#include "Arknights/Map/StageDefinition.hpp"

namespace Arknights::Map {

class StageFactory {
public:
    static std::unique_ptr<Operation> createOperation(const StageDefinition& stageDefinition);
    static std::unique_ptr<Operation> createOperationByStageId(const std::string& stageId);
};

} // namespace Arknights::Map

#endif
