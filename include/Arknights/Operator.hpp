#ifndef ARKNIGHTS_OPERATOR_HPP
#define ARKNIGHTS_OPERATOR_HPP

#include <string>
#include <vector>
#include <memory>

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Arknights/HealthBar.hpp"

namespace Arknights {

class Operator : public Util::GameObject {
public:
    enum class State {
        IDLE,
        ATTACK,
        SKILL,
        DEAD
    };

    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    enum class Type {
        AMIYA,
        CHEN,
        ANGELINA,
        RED,
        EYJAFJALLA,
        TEXAS,
        UMIRIN
    };

    Operator(Type type, float hp, float attack);
    virtual ~Operator() = default;

    virtual void update(float deltaTime);

    void setState(State state) { m_State = state; }
    State getState() const { return m_State; }

    void setDirection(Direction direction);
    Direction getDirection() const { return m_Direction; }

    float getHp() const { return m_Hp; }
    float getAttack() const { return m_Attack; }
    int getDeploymentCost() const { return m_DeploymentCost; }

    bool isAlive() const { return m_State != State::DEAD; }

    Type getType() const { return m_Type; }

    void takeDamage(float damage) {
        m_Hp -= damage;
        if (m_Hp <= 0) {
            m_Hp = 0;
            setState(State::DEAD);
        }
        updateHealthBar();
    }

    bool canAttack() const { return m_AttackTimer <= 0.0f && m_State == State::IDLE; }
    void resetAttackTimer() { m_AttackTimer = m_AttackInterval; }

    void playAttackAnimation();

    bool hasDamagePending() const { return m_DamageTimer > 0.0f || m_CurrentDamageIndex < m_DamageDelays.size(); }
    
    bool consumeDamageTrigger() {
        if (m_DamageReady) {
            m_DamageReady = false;
            return true;
        }
        return false;
    }

    std::size_t getAttackCount() const { return m_AttackCount; }

    // Position on the map (world coordinates)
    void setPosition(const glm::vec2& position) { m_Transform.translation = position; }
    glm::vec2 getPosition() const { return m_Transform.translation; }

    void setGridPosition(const glm::vec2& gridPosition) { m_GridPosition = gridPosition; }
    glm::vec2 getGridPosition() const { return m_GridPosition; }

    bool isInAttackRange(const glm::vec2& enemyGridPos) const;

    std::size_t getBlockCount() const { return m_BlockCount; }
    bool canBlockMore() const { return m_BlockedEnemies.size() < m_BlockCount; }
    void blockEnemy(class Enemy* enemy) { m_BlockedEnemies.push_back(enemy); }
    void clearBlockedEnemies() { m_BlockedEnemies.clear(); }
    void reset();

    void SetVisible(bool visible) {
        Util::GameObject::SetVisible(visible);
        updateHealthBar();
    }

protected:
    void init(const std::vector<std::string>& idleAnimationPaths, const std::vector<std::string>& attackAnimationPaths);
    void updateHealthBar();

protected:
    float m_Hp;
    float m_MaxHp;
    float m_Attack;
    int m_DeploymentCost = 10;
    State m_State = State::IDLE;

    float m_AttackInterval = 1.0f; // Seconds
    float m_AttackTimer = 0.0f;

    std::vector<float> m_DamageDelays = {0.5f}; // Seconds after animation starts to apply damage
    std::size_t m_CurrentDamageIndex = 0;
    float m_DamageTimer = 0.0f;
    bool m_DamageReady = false;

    std::size_t m_BlockCount = 1;
    std::vector<class Enemy*> m_BlockedEnemies;

    std::shared_ptr<Util::Animation> m_IdleAnimation;
    std::shared_ptr<Util::Animation> m_AttackAnimation;

    std::size_t m_AttackCount = 1;
    int m_AnimationInterval = 30;

    Type m_Type;
    Direction m_Direction = Direction::LEFT;
    glm::vec2 m_GridPosition = {0.0f, 0.0f};

    std::shared_ptr<HealthBar> m_HealthBar;
    
    float m_BaseScale = 0.3f;
    float m_VisualOffset = 0.0f;
};

class Amiya : public Operator {
public:
    Amiya();
};

class Chen : public Operator {
public:
    Chen();
};

class Angelina : public Operator {
public:
    Angelina();
};

class Red : public Operator {
public:
    Red();
};

class Eyjafjalla : public Operator {
public:
    Eyjafjalla();
};

class Texas : public Operator {
public:
    Texas();
};

class Umirin : public Operator {
public:
    Umirin();
};

} // namespace Arknights

#endif
