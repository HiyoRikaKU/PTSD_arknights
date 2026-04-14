#ifndef ARKNIGHTS_OPERATION_HPP
#define ARKNIGHTS_OPERATION_HPP

#include <string>
#include <vector>
#include <memory>
#include <array>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

namespace Arknights {

struct SpawnEvent {
    float timestamp;
    std::string enemyType;
    float hp;
    float speed;
};

class WaveManager {
public:
    void addSpawnEvent(float timestamp, const std::string& enemyType, float hp, float speed) {
        m_Timeline.push_back({timestamp, enemyType, hp, speed});
    }

    bool shouldSpawn(float currentTime, SpawnEvent& outEvent) {
        if (m_CurrentEventIndex < m_Timeline.size() && currentTime >= m_Timeline[m_CurrentEventIndex].timestamp) {
            outEvent = m_Timeline[m_CurrentEventIndex];
            m_CurrentEventIndex++;
            return true;
        }
        return false;
    }

    std::size_t getTotalSpawnCount() const { return m_Timeline.size(); }
    std::size_t getSpawnedCount() const { return m_CurrentEventIndex; }
    bool isAllSpawned() const { return m_CurrentEventIndex >= m_Timeline.size(); }

    void reset() {
        m_CurrentEventIndex = 0;
    }

private:
    std::vector<SpawnEvent> m_Timeline;
    std::size_t m_CurrentEventIndex = 0;
};

class Operation {
public:
    enum class TileType {
        OBSTACLE, // x
        GROUND,   // f
        HIGH_GROUND, // h
        SPAWN,    // r
        BASE      // b
    };

    Operation(const std::string& mapPath, 
              const std::vector<glm::vec2>& waypoints,
              const std::vector<std::vector<TileType>>& mapMatrix,
              const std::array<glm::vec2, 4>& srcPoints,
              const std::array<glm::vec2, 4>& dstPoints);

    virtual ~Operation() = default;

    std::shared_ptr<Util::GameObject> getMap() const { return m_Map; }
    const std::vector<glm::vec2>& getWaypoints() const { return m_Waypoints; }

    TileType getTileType(int r, int c) const;
    glm::vec2 getTileCenterPos(int r, int c) const;
    glm::vec2 getTileCenterPos(float r, float c) const;
    bool getTileIndices(const glm::vec2& worldPos, int& r, int& c) const;
    const glm::mat3& getHomography() const { return m_Homography; }

    WaveManager& getWaveManager() { return m_WaveManager; }
    const WaveManager& getWaveManager() const { return m_WaveManager; }

protected:
    void initTiles();
    void placeTile(TileType type, const glm::vec2& pos);
    void computeHomography(const std::array<glm::vec2, 4>& src, const std::array<glm::vec2, 4>& dst);

private:
    std::shared_ptr<Util::GameObject> m_Map;
    std::vector<glm::vec2> m_Waypoints;

    glm::mat3 m_Homography;
    glm::mat3 m_InvHomography;

    std::vector<std::vector<TileType>> m_MapMatrix;
    WaveManager m_WaveManager;
};

} // namespace Arknights

#endif
