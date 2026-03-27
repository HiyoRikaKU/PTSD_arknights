#ifndef ARKNIGHTS_OPERATOR_HPP
#define ARKNIGHTS_OPERATOR_HPP

#include <string>
#include <vector>
#include <memory>

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"

namespace Arknights {

class Operator : public Util::GameObject {
public:
    enum class State {
        IDLE,
        ATTACK,
        SKILL,
        DEAD
    };

    Operator(const std::vector<std::string>& idleAnimationPaths, float hp, float attack);

    virtual void update(float deltaTime);

    void setState(State state) { m_State = state; }
    State getState() const { return m_State; }

    float getHp() const { return m_Hp; }
    float getAttack() const { return m_Attack; }

    // Position on the map (world coordinates)
    void setPosition(const glm::vec2& position) { m_Transform.translation = position; }
    glm::vec2 getPosition() const { return m_Transform.translation; }

    std::size_t getBlockCount() const { return m_BlockCount; }
    bool canBlockMore() const { return m_BlockedEnemies.size() < m_BlockCount; }
    void blockEnemy(class Enemy* enemy) { m_BlockedEnemies.push_back(enemy); }
    void clearBlockedEnemies() { m_BlockedEnemies.clear(); }

protected:
    void init(const std::vector<std::string>& idleAnimationPaths);

protected:
    float m_Hp;
    float m_Attack;
    State m_State = State::IDLE;

    std::size_t m_BlockCount = 1;
    std::vector<class Enemy*> m_BlockedEnemies;

    std::shared_ptr<Util::Animation> m_IdleAnimation;
    // We can add AttackAnimation, SkillAnimation later
};

} // namespace Arknights

#endif
