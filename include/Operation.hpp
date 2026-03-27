#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <string>
#include <vector>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Operation {
public:
    enum class TileType {
        OBSTACLE, // x
        GROUND,   // f
        HIGH_GROUND, // h
        SPAWN,    // r
        BASE      // b
    };

    Operation(const std::string& mapPath, const std::vector<glm::vec2>& waypoints);

    std::shared_ptr<Util::GameObject> GetMap() const { return m_Map; }
    const std::vector<glm::vec2>& GetWaypoints() const { return m_Waypoints; }

    TileType GetTileType(int r, int c) const;
    glm::vec2 GetTileCenterPos(int r, int c) const;
    bool GetTileIndices(const glm::vec2& worldPos, int& r, int& c) const;

private:
    void InitTiles();
    void PlaceTile(TileType type, const glm::vec2& pos);
    void ComputeHomography();

    std::shared_ptr<Util::GameObject> m_Map;
    std::vector<glm::vec2> m_Waypoints;

    glm::mat3 m_Homography;
    glm::mat3 m_InvHomography;

    static const std::vector<std::vector<TileType>> s_MapMatrix;
};

#endif
