#include "Arknights/Operator.hpp"

#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "Util/Image.hpp"

namespace Arknights {

Operator::Operator(Type type, float hp, float attack)
    : m_Hp(hp), m_MaxHp(hp), m_Attack(attack), m_Type(type) {
}

void Operator::init(const std::vector<std::string>& idleAnimationPaths, const std::vector<std::string>& attackAnimationPaths) {
    m_IdleAnimation = std::make_shared<Util::Animation>(idleAnimationPaths, true, m_AnimationInterval, true, 100);
    m_AttackAnimation = std::make_shared<Util::Animation>(attackAnimationPaths, false, m_AnimationInterval, false, 0);

    SetDrawable(m_IdleAnimation);
    SetZIndex(2.0f); // Higher than map (0.0) and enemies (1.5)

    // Set pivot to bottom center (0, -height/2)
    // We use the first frame of idle animation to determine the pivot
    SetPivot({0, -m_IdleAnimation->GetSize().y / 2.0f});

    // Initialize Health Bar
    m_HealthBar = std::make_shared<HealthBar>(2.1f);
    m_HealthBar->SetVisible(false);
    AddChild(m_HealthBar);
    
    // Set initial scale
    setDirection(m_Direction);
}

void Operator::update(float deltaTime) {
    if (m_AttackTimer > 0.0f) {
        m_AttackTimer -= deltaTime / 1000.0f;
    }

    if (m_State == State::ATTACK) {
        if (m_AttackAnimation->GetState() == Util::Animation::State::ENDED) {
            m_State = State::IDLE;
            SetDrawable(m_IdleAnimation);
        }
    }

    updateHealthBar();
}

void Operator::playAttackAnimation() {
    m_State = State::ATTACK;
    SetDrawable(m_AttackAnimation);
    m_AttackAnimation->SetCurrentFrame(0);
    m_AttackAnimation->Play();
}

void Operator::updateHealthBar() {
    if (!m_HealthBar) return;

    bool visible = GetVisible();
    m_HealthBar->SetVisible(visible);

    if (visible) {
        m_HealthBar->SetValue(m_Hp, m_MaxHp);
        // Position UNDER operator (offset -10.0f)
        m_HealthBar->Update(m_Transform.translation, -10.0f);
    }
}

void Operator::setDirection(Direction direction) {
    m_Direction = direction;
    m_Transform.scale.y = m_BaseScale;
    switch (m_Direction) {
        case Direction::LEFT:
            m_Transform.scale.x = -m_BaseScale;
            break;
        case Direction::RIGHT:
            m_Transform.scale.x = m_BaseScale;
            break;
        case Direction::UP:
            m_Transform.scale.x = -m_BaseScale;
            break;
        case Direction::DOWN:
            m_Transform.scale.x = -m_BaseScale;
            break;
    }
}

bool Operator::isInAttackRange(const glm::vec2& enemyGridPos) const {
    float dr = enemyGridPos.x - m_GridPosition.x;
    float dc = enemyGridPos.y - m_GridPosition.y;

    float tr, tc;
    switch (m_Direction) {
        case Direction::RIGHT:
            tr = dr;
            tc = dc;
            break;
        case Direction::LEFT:
            tr = -dr;
            tc = -dc;
            break;
        case Direction::UP:
            tr = dc;
            tc = -dr;
            break;
        case Direction::DOWN:
            tr = -dc;
            tc = dr;
            break;
        default:
            tr = dr;
            tc = dc;
            break;
    }

    int itr = static_cast<int>(std::round(tr));
    int itc = static_cast<int>(std::round(tc));

    if (m_Type == Type::AMIYA) {
        if (itr == -1 || itr == 1) {
            return itc >= 0 && itc <= 2;
        } else if (itr == 0) {
            return itc >= 0 && itc <= 3;
        }
    } else if (m_Type == Type::CHEN) {
        return itr == 0 && (itc == 0 || itc == 1);
    }

    return false;
}

// Amiya Implementation
Amiya::Amiya() : Operator(Type::AMIYA, 1000.0f, 120.0f) {
    m_AttackInterval = 1.6f;
    m_BaseScale = 0.3f;

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 31; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/char_002_amiya/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 53; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/char_002_amiya/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths);
}

// Chen Implementation
Chen::Chen() : Operator(Type::CHEN, 1200.0f, 80.0f) {
    m_AttackInterval = 1.3f;
    m_AttackCount = 2;
    m_BlockCount = 2;
    m_BaseScale = 0.3f;

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 200; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/char_010_chen/Idle_" << std::setfill('0') << std::setw(3) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 45; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/char_010_chen/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths);
}

} // namespace Arknights
