#include "Arknights/Entity/Operator.hpp"
#include "Arknights/Entity/Enemy.hpp"

#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "Util/Image.hpp"

namespace Arknights {

Operator::Operator(Type type, float hp, float attack)
    : m_Hp(hp), m_MaxHp(hp), m_Attack(attack), m_Type(type) {
}

void Operator::reset() {
    m_Hp = m_MaxHp;
    m_State = State::IDLE;
    m_AttackTimer = 0.0f;
    m_CurrentDamageIndex = 0;
    m_DamageReady = false;
    m_BlockedEnemies.clear();
    SetVisible(false);
    SetDrawable(m_IdleAnimation);
    m_IdleAnimation->Play();
    updateHealthBar();
}

void Operator::init(const std::vector<std::string>& idleAnimationPaths,
                    const std::vector<std::string>& attackAnimationPaths,
                    const std::vector<std::string>& dieAnimationPaths) {
    m_IdleAnimation = std::make_shared<Util::Animation>(idleAnimationPaths, true, m_AnimationInterval, true, 100);
    m_AttackAnimation = std::make_shared<Util::Animation>(attackAnimationPaths, false, m_AnimationInterval, false, 0);
    m_DieAnimation = std::make_shared<Util::Animation>(dieAnimationPaths, false, m_AnimationInterval, false, 0);

    SetDrawable(m_IdleAnimation);
    SetZIndex(2.0f); // Higher than map (0.0) and enemies (1.5)

    // Set pivot to bottom center (0, -height/2)
    // We use the first frame of idle animation to determine the pivot
    SetPivot({0, -m_IdleAnimation->GetSize().y / 2.0f - m_VisualOffset / m_BaseScale});

    // Initialize Health Bar
    m_HealthBar = std::make_shared<HealthBar>(2.1f, "/blue.png");
    m_HealthBar->SetVisible(false);
    AddChild(m_HealthBar);
    
    // Set initial scale
    setDirection(m_Direction);
}

void Operator::update(float deltaTime) {
    if (m_State == State::DYING) {
        if (m_DieAnimation && m_DieAnimation->GetState() == Util::Animation::State::ENDED) {
            m_State = State::DEAD;
            SetVisible(false);
        }
        updateHealthBar();
        return;
    }

    if (m_AttackTimer > 0.0f) {
        m_AttackTimer -= deltaTime / 1000.0f;
    }

    if (m_State == State::ATTACK) {
        // Calculate time elapsed in animation based on current frame
        float animationTime = static_cast<float>(m_AttackAnimation->GetCurrentFrameIndex()) * m_AnimationInterval / 1000.0f;
        
        if (m_CurrentDamageIndex < m_DamageDelays.size()) {
            if (animationTime >= m_DamageDelays[m_CurrentDamageIndex]) {
                m_DamageReady = true;
                m_CurrentDamageIndex++;
            }
        }

        if (m_AttackAnimation->GetState() == Util::Animation::State::ENDED) {
            m_State = State::IDLE;
            SetDrawable(m_IdleAnimation);
        }
    }

    updateHealthBar();
}

void Operator::takeDamage(float damage) {
    if (!isAlive()) return;

    m_Hp -= damage;
    if (m_Hp <= 0.0f) {
        m_Hp = 0.0f;
        startDeathAnimation();
    }
    updateHealthBar();
}

void Operator::startDeathAnimation() {
    if (m_State == State::DYING || m_State == State::DEAD) return;
    m_State = State::DYING;
    m_DamageReady = false;
    m_CurrentDamageIndex = 0;
    m_AttackTimer = 0.0f;
    m_BlockedEnemies.clear();
    if (m_DieAnimation) {
        SetDrawable(m_DieAnimation);
        m_DieAnimation->SetCurrentFrame(0);
        m_DieAnimation->Play();
    } else {
        m_State = State::DEAD;
        SetVisible(false);
    }
}

void Operator::playAttackAnimation() {
    m_State = State::ATTACK;
    SetDrawable(m_AttackAnimation);
    m_AttackAnimation->SetCurrentFrame(0);
    m_AttackAnimation->Play();
    m_CurrentDamageIndex = 0;
    m_DamageReady = false;
    m_DamageTimer = 0.0f; 
}

void Operator::updateHealthBar() {
    if (!m_HealthBar) return;

    bool visible = m_Visible;
    m_HealthBar->SetVisible(visible);

    if (visible) {
        m_HealthBar->SetValue(m_Hp, m_MaxHp);
        // Position at the bottom of operator
        m_HealthBar->Update(m_Transform.translation + glm::vec2(0, m_VisualOffset), -10.0f);
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
    } else if (m_Type == Type::ANGELINA) {
        return itr >= -1 && itr <= 1 && itc >= -1 && itc <= 2;
    } else if (m_Type == Type::RED || m_Type == Type::TEXAS) {
        return itr == 0 && (itc == 0 || itc == 1);
    } else if (m_Type == Type::EYJAFJALLA) {
        return itr >= -1 && itr <= 1 && itc >= 0 && itc <= 2;
    } else if (m_Type == Type::UMIRIN) {
        if (itr == 0) return itc >= -1 && itc <= 3;
        return itr >= -1 && itr <= 1 && itc >= -1 && itc <= 2;
    }

    return false;
}

// Amiya Implementation
Amiya::Amiya() : Operator(Type::AMIYA, 1000.0f, 120.0f) {
    m_DeploymentCost = 10;
    m_VisualOffset = 20.0f; // Stand higher on highground
    m_AttackInterval = 1.6f;
    m_DamageDelays = {0.6f};
    m_BaseScale = 0.3f;
    m_AnimationInterval = 60; // Doubled to account for half the frames

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/amiya/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 53; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/amiya/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    std::vector<std::string> diePaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/amiya/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        diePaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths, diePaths);
}

// Chen Implementation
Chen::Chen() : Operator(Type::CHEN, 1200.0f, 80.0f) {
    m_DeploymentCost = 18;
    m_AttackInterval = 1.3f;
    m_DamageDelays = {0.4f, 0.6f}; // Chen hits twice
    m_AttackCount = 1; // Each delay trigger represents one hit
    m_BlockCount = 2;
    m_BaseScale = 0.3f;
    m_AnimationInterval = 60; // Doubled

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 199; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/chen/Idle_" << std::setfill('0') << std::setw(3) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 45; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/chen/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    std::vector<std::string> diePaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/chen/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        diePaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths, diePaths);
}

// Angelina Implementation
Angelina::Angelina() : Operator(Type::ANGELINA, 1100.0f, 100.0f) {
    m_DeploymentCost = 14;
    m_VisualOffset = 20.0f;
    m_AttackInterval = 1.9f;
    m_DamageDelays = {0.7f};
    m_BaseScale = 0.3f;
    m_AnimationInterval = 60;

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 59; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/agelina/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 41; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/agelina/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    std::vector<std::string> diePaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/agelina/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        diePaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths, diePaths);
}

// Red Implementation
Red::Red() : Operator(Type::RED, 900.0f, 150.0f) {
    m_DeploymentCost = 6;
    m_AttackInterval = 0.93f;
    m_DamageDelays = {0.3f};
    m_BaseScale = 0.3f;
    m_AnimationInterval = 60;

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 59; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/red/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 35; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/red/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    std::vector<std::string> diePaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/red/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        diePaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths, diePaths);
}

// Eyjafjalla Implementation
Eyjafjalla::Eyjafjalla() : Operator(Type::EYJAFJALLA, 1200.0f, 200.0f) {
    m_DeploymentCost = 20;
    m_VisualOffset = 20.0f;
    m_AttackInterval = 1.6f;
    m_DamageDelays = {0.6f};
    m_BaseScale = 0.3f;
    m_AnimationInterval = 60;

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 81; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/eyjafjalla/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 41; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/eyjafjalla/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    std::vector<std::string> diePaths;
    for (int i = 1; i <= 21; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/eyjafjalla/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        diePaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths, diePaths);
}

// Texas Implementation
Texas::Texas() : Operator(Type::TEXAS, 1100.0f, 100.0f) {
    m_DeploymentCost = 12;
    m_AttackInterval = 1.05f;
    m_DamageDelays = {0.4f};
    m_BlockCount = 2;
    m_BaseScale = 0.3f;
    m_AnimationInterval = 60;

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 59; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/texas/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/texas/Attack_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    std::vector<std::string> diePaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/texas/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        diePaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths, diePaths);
}

// Umirin Implementation
Umirin::Umirin() : Operator(Type::UMIRIN, 1000.0f, 120.0f) {
    m_DeploymentCost = 10;
    m_VisualOffset = 20.0f;
    m_AttackInterval = 1.6f;
    m_DamageDelays = {0.6f};
    m_BaseScale = 0.3f;
    m_AnimationInterval = 60;

    std::vector<std::string> idlePaths;
    for (int i = 1; i <= 121; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/timoris/Idle_" << std::setfill('0') << std::setw(3) << i << ".png";
        idlePaths.push_back(ss.str());
    }
    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/timoris/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        attackPaths.push_back(ss.str());
    }
    std::vector<std::string> diePaths;
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/timoris/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        diePaths.push_back(ss.str());
    }
    init(idlePaths, attackPaths, diePaths);
}

} // namespace Arknights
