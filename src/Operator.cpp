#include "Operator.hpp"

Operator::Operator(const std::vector<std::string>& idleAnimationPaths, float hp, float attack)
    : m_Hp(hp), m_Attack(attack) {
    Init(idleAnimationPaths);
}

void Operator::Init(const std::vector<std::string>& idleAnimationPaths) {
    m_IdleAnimation = std::make_shared<Util::Animation>(idleAnimationPaths, true, 50, true, 100);
    SetDrawable(m_IdleAnimation);
    SetZIndex(2.0f); // Higher than map (0.0) and enemies (1.5)

    // Set pivot to bottom center (0, -height/2)
    SetPivot({0, -m_IdleAnimation->GetSize().y / 2.0f});
}

void Operator::Update(float /*deltaTime*/) {
    // Basic update logic
}
