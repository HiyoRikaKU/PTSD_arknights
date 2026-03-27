#ifndef ARKNIGHTS_ENEMYPOOL_HPP
#define ARKNIGHTS_ENEMYPOOL_HPP

#include <vector>
#include <string>
#include <memory>

#include "Arknights/Enemy.hpp"

namespace Arknights {

class EnemyPool {
public:
    EnemyPool(std::size_t size, const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& waypoints);

    Enemy* getEnemy();
    void returnEnemy(Enemy* enemy);

    const std::vector<std::shared_ptr<Enemy>>& getRenderHandles() const { return m_Pool; }

private:
    std::vector<std::shared_ptr<Enemy>> m_Pool;
    std::vector<Enemy*> m_Available;
};

} // namespace Arknights

#endif
