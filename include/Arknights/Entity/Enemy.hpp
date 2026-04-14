#ifndef ARKNIGHTS_ENEMY_HPP
#define ARKNIGHTS_ENEMY_HPP

#include <queue>
#include <vector>
#include <string>
#include <memory>

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Arknights/UI/HealthBar.hpp"

namespace Arknights {

class Enemy : public Util::GameObject {
public:
    enum class State {
        ALIVE,
        DYING,
        DEAD
    };

    Enemy(const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& gridWaypoints);
    Enemy(const std::vector<std::string>& animationPaths, const std::string& pathString);

    void spawn(const std::vector<glm::vec2>& gridPath, float hp, float speed, const glm::mat3& homography);
    void despawn();

    bool isActive() const { return m_IsActive; }
    bool isAlive() const { return m_State == State::ALIVE; }

    void update(float deltaTime);
    bool reachedEnd() const { return m_ReachedEnd; }
    void setSpeed(float speed) { m_Speed = speed; }
    void setAttack(float attack) { m_Attack = attack; }
    void setAttackInterval(float interval) { m_AttackInterval = interval; }

    void setAnimation(const std::vector<std::string>& animationPaths);
    void setDieAnimation(const std::vector<std::string>& dieAnimationPaths);

    bool isBlocked() const { return m_IsBlocked; }
    void setBlocked(bool blocked) { m_IsBlocked = blocked; }
    void setTargetOperator(class Operator* op) { m_TargetOperator = op; }
    class Operator* getTargetOperator() const { return m_TargetOperator; }

    glm::vec2 getPosition() const { return GetTransform().translation; }
    glm::vec2 getGridPosition() const { return m_CurrentGridPos; }

    void takeDamage(float damage) {
        if (m_State != State::ALIVE) return;
        m_Hp -= damage;
        if (m_Hp <= 0) {
            m_Hp = 0;
            startDeath();
        }
        updateHealthBar();
    }

private:
    void init(const std::vector<std::string>& animationPaths);
    void updateHealthBar();
    void startDeath();
    static std::vector<glm::vec2> parsePathString(const std::string& pathString);

private:
    std::vector<glm::vec2> m_GridWaypoints;
    glm::vec2 m_CurrentGridPos;
    glm::mat3 m_Homography;

    std::size_t m_CurrentWaypointIndex = 0;
    float m_Speed = 0.0002f; // Grid units per ms
    float m_Hp = 100.0F;
    float m_MaxHp = 100.0F;
    float m_Attack = 10.0f;
    float m_AttackInterval = 1000.0f; // ms
    float m_AttackTimer = 0.0f;
    class Operator* m_TargetOperator = nullptr;

    bool m_IsActive = false;
    bool m_ReachedEnd = false;
    bool m_IsBlocked = false;

    State m_State = State::DEAD;

    float m_BaseScale = 0.3f;
    float m_HealthBarOffset = 50.0f;

    std::shared_ptr<Util::Animation> m_MoveAnimation;
    std::shared_ptr<Util::Animation> m_DieAnimation;

    std::shared_ptr<HealthBar> m_HealthBar;
};

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
