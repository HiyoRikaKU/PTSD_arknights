#include "Arknights/Map/StageFactory.hpp"

#include "Arknights/Map/StageRepository.hpp"

namespace Arknights::Map {

std::unique_ptr<Operation> StageFactory::createOperation(const StageDefinition& stageDefinition) {
    auto operation = std::make_unique<Operation>(
        stageDefinition.mapImagePath,
        stageDefinition.waypoints,
        stageDefinition.tileMatrix,
        stageDefinition.homography.srcPoints,
        stageDefinition.homography.dstPoints
    );

    auto& waveManager = operation->getWaveManager();
    for (const auto& event : stageDefinition.wave) {
        waveManager.addSpawnEvent(event.timestamp, event.enemyType, event.hp, event.speed);
    }

    return operation;
}

std::unique_ptr<Operation> StageFactory::createOperationByStageId(const std::string& stageId) {
    return createOperation(StageRepository::getStageById(stageId));
}

} // namespace Arknights::Map
