#include "Operation.hpp"
#include <array>
#include <cmath>
#include <algorithm>

// Define TileType aliases to match instruction.md
namespace {
    constexpr Operation::TileType x = Operation::TileType::OBSTACLE;
    constexpr Operation::TileType f = Operation::TileType::GROUND;
    constexpr Operation::TileType h = Operation::TileType::HIGH_GROUND;
    constexpr Operation::TileType r = Operation::TileType::SPAWN;
    constexpr Operation::TileType b = Operation::TileType::BASE;
}

const std::vector<std::vector<Operation::TileType>> Operation::s_MapMatrix = {
    {x, h, h, h, h, h, h, h, b, x},
    {x, h, f, f, f, f, h, f, f, r},
    {b, f, f, h, f, f, f, f, f, r},
    {x, h, h, h, h, h, h, h, x, x},
};

Operation::Operation(const std::string& mapPath, const std::vector<glm::vec2>& waypoints)
    : m_Waypoints(waypoints) {
    m_Map = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(mapPath),
        0
    );
    // Scale map to 1600x900 as per App.cpp requirement
    m_Map->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Map->GetScaledSize();
    m_Map->SetVisible(false);

    ComputeHomography();
    InitTiles();
}

void Operation::ComputeHomography() {
    // Four corner measurement points (x = col, y = row)
    // P(row=0, col=1) = (-493, 253)
    // P(row=0, col=7) = ( 255, 247)
    // P(row=3, col=1) = (-574, -86)
    // P(row=3, col=7) = ( 295, -87)
    std::array<glm::vec2, 4> src = {
        glm::vec2{1, 0}, {7, 0}, {1, 3}, {7, 3}
    };
    std::array<glm::vec2, 4> dst = {
        glm::vec2{-493, 253}, {255, 247}, {-574, -86}, {295, -87}
    };

    // Construct 8x8 matrix to solve for homography parameters h0...h7 (h8=1)
    // X = (h0*x + h1*y + h2) / (h6*x + h7*y + 1)
    // Y = (h3*x + h4*y + h5) / (h6*x + h7*y + 1)
    // Equations:
    // x*h0 + y*h1 + 1*h2 + 0*h3 + 0*h4 + 0*h5 - x*X*h6 - y*X*h7 = X
    // 0*h0 + 0*h1 + 0*h2 + x*h3 + y*h4 + 1*h5 - x*Y*h6 - y*Y*h7 = Y

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

    // Gaussian Elimination
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

    // Build H matrix (column-major for GLM)
    // h_res: h0 h1 h2 h3 h4 h5 h6 h7 h8
    // Matrix:
    // h0 h1 h2
    // h3 h4 h5
    // h6 h7 h8
    // GLM mat3 is col-major: m[col][row]
    m_Homography = glm::mat3(
        h_res[0], h_res[3], h_res[6],
        h_res[1], h_res[4], h_res[7],
        h_res[2], h_res[5], h_res[8]
    );
    m_InvHomography = glm::inverse(m_Homography);
}

void Operation::InitTiles() {
    for (int r_idx = 0; r_idx < 4; ++r_idx) {
        for (int c_idx = 0; c_idx < 10; ++c_idx) {
            PlaceTile(s_MapMatrix[r_idx][c_idx], GetTileCenterPos(r_idx, c_idx));
        }
    }
}

void Operation::PlaceTile(TileType type, const glm::vec2& pos) {
    (void)type;
    (void)pos;
}

Operation::TileType Operation::GetTileType(int r_idx, int c_idx) const {
    if (r_idx < 0 || r_idx >= 4 || c_idx < 0 || c_idx >= 10) return TileType::OBSTACLE;
    return s_MapMatrix[r_idx][c_idx];
}

glm::vec2 Operation::GetTileCenterPos(int r_idx, int c_idx) const {
    // Tile center: (col + 0.5, row + 0.5)
    glm::vec3 p = m_Homography * glm::vec3(static_cast<float>(c_idx) + 0.5f, static_cast<float>(r_idx) + 0.5f, 1.0f);
    return {p.x / p.z, p.y / p.z};
}

bool Operation::GetTileIndices(const glm::vec2& pos, int& r_idx, int& c_idx) const {
    glm::vec3 p = m_InvHomography * glm::vec3(pos.x, pos.y, 1.0f);
    float fc = p.x / p.z;
    float fr = p.y / p.z;
    
    c_idx = static_cast<int>(std::floor(fc));
    r_idx = static_cast<int>(std::floor(fr));

    return (r_idx >= 0 && r_idx < 4 && c_idx >= 0 && c_idx < 10);
}
