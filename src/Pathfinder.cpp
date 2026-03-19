#include "Pathfinder.hpp"

#include <algorithm>
#include <limits>

namespace {
struct NodeCompare {
    bool operator()(const Node *lhs, const Node *rhs) const {
        const int lf = lhs->fCost();
        const int rf = rhs->fCost();
        if (lf == rf) {
            return lhs->hCost > rhs->hCost;
        }
        return lf > rf; // min-heap behavior via priority_queue
    }
};
} // namespace

Pathfinder::Pathfinder(std::vector<std::vector<Node>> &grid)
    : m_Grid(grid) {
    m_Height = static_cast<int>(m_Grid.size());
    m_Width = m_Height > 0 ? static_cast<int>(m_Grid[0].size()) : 0;
    m_Closed.assign(m_Height, std::vector<bool>(m_Width, false));
}

std::vector<glm::vec2> Pathfinder::FindPath(Node *startNode, Node *targetNode) {
    std::vector<glm::vec2> path;
    if (startNode == nullptr || targetNode == nullptr) {
        return path;
    }

    ResetState();

    auto pq = std::priority_queue<Node *, std::vector<Node *>, NodeCompare>{};

    startNode->gCost = 0;
    startNode->hCost = Manhattan(startNode, targetNode);
    startNode->parent = nullptr;
    pq.push(startNode);

    while (!pq.empty()) {
        Node *current = pq.top();
        pq.pop();

        m_Closed[current->y][current->x] = true;

        if (current == targetNode) {
            // 回推路徑
            Node *crawl = targetNode;
            while (crawl != nullptr) {
                path.emplace_back(static_cast<float>(crawl->x) + 0.5F,
                                  static_cast<float>(crawl->y) + 0.5F);
                crawl = crawl->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (Node *neighbor : GetNeighbors(current)) {
            if (neighbor->type == TileType::Blocked || m_Closed[neighbor->y][neighbor->x]) {
                continue;
            }

            const int tentativeG = current->gCost + 1; // 四向步距 = 1
            if (tentativeG < neighbor->gCost) {
                neighbor->parent = current;
                neighbor->gCost = tentativeG;
                neighbor->hCost = Manhattan(neighbor, targetNode);
                pq.push(neighbor);
            }
        }
    }

    // 無路徑，回傳空陣列
    return path;
}

bool Pathfinder::InBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < m_Width && y < m_Height;
}

std::vector<Node *> Pathfinder::GetNeighbors(Node *node) {
    static const int DIRS[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    std::vector<Node *> result;
    result.reserve(4);

    for (auto &d : DIRS) {
        const int nx = node->x + d[0];
        const int ny = node->y + d[1];
        if (InBounds(nx, ny)) {
            result.push_back(&m_Grid[ny][nx]);
        }
    }
    return result;
}

int Pathfinder::Manhattan(const Node *a, const Node *b) const {
    return std::abs(a->x - b->x) + std::abs(a->y - b->y);
}

void Pathfinder::ResetState() {
    for (int y = 0; y < m_Height; ++y) {
        std::fill(m_Closed[y].begin(), m_Closed[y].end(), false);
        for (int x = 0; x < m_Width; ++x) {
            Node &n = m_Grid[y][x];
            n.gCost = std::numeric_limits<int>::max();
            n.hCost = 0;
            n.parent = nullptr;
        }
    }
}

