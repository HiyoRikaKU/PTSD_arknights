#ifndef ARKNIGHTS_OPERATION_HPP
#define ARKNIGHTS_OPERATION_HPP

#include <string>
#include <vector>
#include <memory>
#include <array>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Arknights/WaveManager.hpp"

namespace Arknights {

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
