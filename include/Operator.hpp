#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include <string>
#include <vector>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"

class Operator : public Util::GameObject {
public:
    enum class State {
        IDLE,
        ATTACK,
        SKILL,
        DEAD
    };

    Operator(const std::vector<std::string>& idleAnimationPaths, float hp, float attack);

    virtual void Update(float deltaTime);

    void SetState(State state) { m_State = state; }
    State GetState() const { return m_State; }

    float GetHp() const { return m_Hp; }
    float GetAttack() const { return m_Attack; }

    // Position on the map (world coordinates)
    void SetPosition(const glm::vec2& position) { m_Transform.translation = position; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }

protected:
    void Init(const std::vector<std::string>& idleAnimationPaths);

    float m_Hp;
    float m_Attack;
    State m_State = State::IDLE;

    std::shared_ptr<Util::Animation> m_IdleAnimation;
    // We can add AttackAnimation, SkillAnimation later
};

#endif
