#include "Arknights/Map/Operation.hpp"

#include <array>
#include <cmath>
#include <algorithm>

namespace Arknights {

// Define TileType aliases to match instruction.md
namespace {
    constexpr Operation::TileType x = Operation::TileType::OBSTACLE;
    constexpr Operation::TileType f = Operation::TileType::GROUND;
    constexpr Operation::TileType h = Operation::TileType::HIGH_GROUND;
    constexpr Operation::TileType r = Operation::TileType::SPAWN;
    constexpr Operation::TileType b = Operation::TileType::BASE;
}

Operation::Operation(const std::string& mapPath, 
                     const std::vector<glm::vec2>& waypoints,
                     const std::vector<std::vector<TileType>>& mapMatrix,
                     const std::array<glm::vec2, 4>& srcPoints,
                     const std::array<glm::vec2, 4>& dstPoints)
    : m_Waypoints(waypoints), m_MapMatrix(mapMatrix) {
    m_Map = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(mapPath),
        0
    );
    // Scale map to 1600x900 as per App.cpp requirement
    m_Map->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Map->GetScaledSize();
    m_Map->SetVisible(false);

    computeHomography(srcPoints, dstPoints);
    initTiles();
}

void Operation::computeHomography(const std::array<glm::vec2, 4>& src, const std::array<glm::vec2, 4>& dst) {
    double A[8][9];
    for (int i = 0; i < 4; ++i) {
        float x_src = src[i].x;
        float y_src = src[i].y;
        float X_dst = dst[i].x;
        float Y_dst = dst[i].y;

        A[i * 2][0] = x_src; A[i * 2][1] = y_src; A[i * 2][2] = 1.0;
        A[i * 2][3] = 0.0; A[i * 2][4] = 0.0; A[i * 2][5] = 0.0;
        A[i * 2][6] = -x_src * X_dst; A[i * 2][7] = -y_src * X_dst;
        A[i * 2][8] = X_dst;

        A[i * 2 + 1][0] = 0.0; A[i * 2 + 1][1] = 0.0; A[i * 2 + 1][2] = 0.0;
        A[i * 2 + 1][3] = x_src; A[i * 2 + 1][4] = y_src; A[i * 2 + 1][5] = 1.0;
        A[i * 2 + 1][6] = -x_src * Y_dst; A[i * 2 + 1][7] = -y_src * Y_dst;
        A[i * 2 + 1][8] = Y_dst;
    }

    for (int i = 0; i < 8; ++i) {
        int pivot = i;
        for (int j = i + 1; j < 8; ++j) {
            if (std::abs(A[j][i]) > std::abs(A[pivot][i])) pivot = j;
        }
        for (int j = 0; j < 9; ++j) std::swap(A[i][j], A[pivot][j]);

        for (int j = i + 1; j < 8; ++j) {
            double factor = A[j][i] / A[i][i];
            for (int k = i; k < 9; ++k) A[j][k] -= factor * A[i][k];
        }
    }

    float h_res[9];
    for (int i = 7; i >= 0; --i) {
        double sum = 0;
        for (int j = i + 1; j < 8; ++j) sum += A[i][j] * h_res[j];
        h_res[i] = static_cast<float>((A[i][8] - sum) / A[i][i]);
    }
    h_res[8] = 1.0f;

    m_Homography = glm::mat3(
        h_res[0], h_res[3], h_res[6],
        h_res[1], h_res[4], h_res[7],
        h_res[2], h_res[5], h_res[8]
    );
    m_InvHomography = glm::inverse(m_Homography);
}

void Operation::initTiles() {
    for (int r_idx = 0; r_idx < static_cast<int>(m_MapMatrix.size()); ++r_idx) {
        for (int c_idx = 0; c_idx < static_cast<int>(m_MapMatrix[r_idx].size()); ++c_idx) {
            placeTile(m_MapMatrix[r_idx][c_idx], getTileCenterPos(r_idx, c_idx));
        }
    }
}

void Operation::placeTile(TileType type, const glm::vec2& pos) {
    (void)type;
    (void)pos;
}

Operation::TileType Operation::getTileType(int r_idx, int c_idx) const {
    if (r_idx < 0 || r_idx >= static_cast<int>(m_MapMatrix.size()) || 
        c_idx < 0 || c_idx >= static_cast<int>(m_MapMatrix[r_idx].size())) return TileType::OBSTACLE;
    return m_MapMatrix[r_idx][c_idx];
}

glm::vec2 Operation::getTileCenterPos(int r_idx, int c_idx) const {
    return getTileCenterPos(static_cast<float>(r_idx) + 0.5f, static_cast<float>(c_idx) + 0.5f);
}

glm::vec2 Operation::getTileCenterPos(float r, float c) const {
    // Grid coordinates (c, r, 1) projected by Homography
    glm::vec3 p = m_Homography * glm::vec3(c, r, 1.0f);
    return {p.x / p.z, p.y / p.z};
}

bool Operation::getTileIndices(const glm::vec2& pos, int& r_idx, int& c_idx) const {
    glm::vec3 p = m_InvHomography * glm::vec3(pos.x, pos.y, 1.0f);
    float fc = p.x / p.z;
    float fr = p.y / p.z;
    
    c_idx = static_cast<int>(std::floor(fc));
    r_idx = static_cast<int>(std::floor(fr));

    return (r_idx >= 0 && r_idx < static_cast<int>(m_MapMatrix.size()) && 
            c_idx >= 0 && c_idx < static_cast<int>(m_MapMatrix[r_idx].size()));
}

} // namespace Arknights
