#ifndef ENEMY_POOL_HPP
#define ENEMY_POOL_HPP

#include <memory>
#include <queue>
#include <vector>

#include "Enemy.hpp"

/**
 * @brief A fixed-size enemy object pool.
 *
 * - Pre-allocates all Enemy instances upfront inside a contiguous vector of
 *   std::unique_ptr to maximize cache locality.
 * - Uses a std::queue of raw pointers as a free list for O(1) spawn/return.
 * - No dynamic allocations happen during gameplay (spawn/update/return).
 */
class EnemyPool {
public:
    EnemyPool(std::size_t poolSize, const std::vector<std::string> &animationPaths,
              const std::vector<glm::vec2> &waypoints);

    /**
     * @brief Get an inactive enemy from the pool.
     * @return Enemy* or nullptr if the pool is exhausted.
     */
    Enemy *GetEnemy();

    /**
     * @brief Return an enemy to the pool (marks it inactive).
     */
    void ReturnEnemy(Enemy *enemy);

    /**
     * @brief Update every active enemy (O(n) over pool, no allocations).
     */
    void UpdateAllActive(float dt);

    /**
     * @brief Render handles (non-owning shared_ptr) for registering with the renderer.
     */
    const std::vector<std::shared_ptr<Enemy>> &GetRenderHandles() const {
        return m_RenderHandles;
    }

    std::size_t FreeCount() const { return m_Free.size(); }
    std::size_t Capacity() const { return m_Pool.size(); }

private:
    std::vector<std::unique_ptr<Enemy>> m_Pool;             // Owns enemies contiguously
    std::queue<Enemy *> m_Free;                             // Free list
    std::vector<std::shared_ptr<Enemy>> m_RenderHandles;    // Non-owning views for renderer
};

#endif

