#include "Arknights/EnemyPool.hpp"

namespace Arknights {

EnemyPool::EnemyPool(std::size_t size, const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& waypoints) {
    m_Pool.reserve(size);
    m_Available.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        auto enemy = std::make_shared<Enemy>(animationPaths, waypoints);
        m_Pool.push_back(enemy);
        m_Available.push_back(enemy.get());
    }
}

Enemy* EnemyPool::getEnemy() {
    if (m_Available.empty()) {
        return nullptr;
    }
    Enemy* enemy = m_Available.back();
    m_Available.pop_back();
    return enemy;
}

void EnemyPool::returnEnemy(Enemy* enemy) {
    enemy->despawn();
    m_Available.push_back(enemy);
}

} // namespace Arknights
