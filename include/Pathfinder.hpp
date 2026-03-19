#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <queue>
#include <vector>
#include <limits>

#include "pch.hpp"

// 定義格子的類型
enum class TileType {
    Walkable,
    Blocked,
    Spawn,
    Goal
};

// A* 演算法需要的節點結構
struct Node {
    int x = 0;
    int y = 0;
    TileType type = TileType::Walkable;

    int gCost = std::numeric_limits<int>::max();
    int hCost = 0;
    Node *parent = nullptr;

    int fCost() const { return gCost + hCost; }
};

/**
 * @brief 以四向移動 (無斜向) 的 A* 尋路器。
 *
 * - 使用 priority_queue 作為 open list。
 * - 使用 2D bool 陣列作為 closed list (O(1) 查詢)。
 * - 只考慮上下左右相鄰節點，忽略 Blocked 格子。
 */
class Pathfinder {
public:
    explicit Pathfinder(std::vector<std::vector<Node>> &grid);

    /**
     * @brief 計算從 start 到 goal 的路徑。
     * @return 依序 waypoint（以格子中心的世界座標表示）。找不到則回傳空陣列。
     */
    std::vector<glm::vec2> FindPath(Node *startNode, Node *targetNode);

private:
    bool InBounds(int x, int y) const;
    std::vector<Node *> GetNeighbors(Node *node);
    int Manhattan(const Node *a, const Node *b) const;
    void ResetState();

    std::vector<std::vector<Node>> &m_Grid;
    int m_Width = 0;
    int m_Height = 0;
    std::vector<std::vector<bool>> m_Closed;
};

#endif

