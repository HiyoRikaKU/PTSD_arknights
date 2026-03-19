#include "EnemyPool.hpp"

#include <utility>

EnemyPool::EnemyPool(std::size_t poolSize,
                     const std::vector<std::string> &animationPaths,
                     const std::vector<glm::vec2> &waypoints) {
    m_Pool.reserve(poolSize);
    m_RenderHandles.reserve(poolSize);

    for (std::size_t i = 0; i < poolSize; ++i) {
        m_Pool.push_back(std::make_unique<Enemy>(animationPaths, waypoints));
        Enemy *enemyPtr = m_Pool.back().get();
        m_Free.push(enemyPtr);

        // Non-owning shared_ptr (noop deleter) so renderer can hold references
        m_RenderHandles.emplace_back(enemyPtr, [](Enemy *) {});
    }
}

Enemy *EnemyPool::GetEnemy() {
    if (m_Free.empty()) {
        return nullptr;
    }
    Enemy *enemy = m_Free.front();
    m_Free.pop();
    return enemy;
}

void EnemyPool::ReturnEnemy(Enemy *enemy) {
    if (enemy == nullptr) {
        return;
    }
    // Ensure it's marked inactive before returning to free list.
    enemy->Despawn();
    m_Free.push(enemy);
}

void EnemyPool::UpdateAllActive(float dt) {
    for (auto &enemyPtr : m_Pool) {
        if (enemyPtr->IsActive()) {
            enemyPtr->Update(dt);
        }
    }
}
