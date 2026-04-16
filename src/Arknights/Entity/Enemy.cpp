#include "Arknights/Entity/Enemy.hpp"
#include "Arknights/Entity/Operator.hpp"

#include <sstream>
#include <iomanip>

#include "Util/Animation.hpp"
#include "Util/Image.hpp"

namespace Arknights {

Enemy::Enemy(const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& gridWaypoints)
    : m_GridWaypoints(gridWaypoints) {
    init(animationPaths);
}

Enemy::Enemy(const std::vector<std::string>& animationPaths, const std::string& pathString)
    : m_GridWaypoints(parsePathString(pathString)) {
    init(animationPaths);
}

void Enemy::init(const std::vector<std::string>& animationPaths) {
    m_MoveAnimation = std::make_shared<Util::Animation>(animationPaths, true, 50, true, 0);
    SetDrawable(m_MoveAnimation);
    SetVisible(false);
    SetZIndex(1.5f);
    
    // Default scale and pivot
    m_Transform.scale = {m_BaseScale, m_BaseScale};
    SetPivot({0, -m_MoveAnimation->GetSize().y / 2.0f});
}

void Enemy::spawn(const std::vector<glm::vec2>& gridPath, float hp, float speed, const glm::mat3& homography) {
    m_GridWaypoints = gridPath;
    m_Hp = hp;
    m_MaxHp = hp;
    m_Speed = speed;
    m_Homography = homography;
    m_CurrentWaypointIndex = 0;
    m_IsActive = true;
    m_ReachedEnd = false;
    m_IsBlocked = false;
    m_State = State::ALIVE;
    m_TargetOperator = nullptr;
    m_AttackTimer = 0.0f;

    if (!m_GridWaypoints.empty()) {
        m_CurrentGridPos = m_GridWaypoints[0];
        glm::vec3 p = m_Homography * glm::vec3(m_CurrentGridPos.y, m_CurrentGridPos.x, 1.0f);
        m_Transform.translation = {p.x / p.z, p.y / p.z};

        // Initial flip
        if (m_GridWaypoints.size() > 1) {
            glm::vec2 dir = m_GridWaypoints[1] - m_GridWaypoints[0];
            if (dir.y > 0) {
                m_Transform.scale.x = m_BaseScale;
            } else if (dir.y < 0) {
                m_Transform.scale.x = -m_BaseScale;
            }
        }
    }

    SetVisible(true);
    SetDrawable(m_MoveAnimation);
    m_MoveAnimation->Play();
    m_IsAttackVisualActive = false;

    if (!m_HealthBar) {
        m_HealthBar = std::make_shared<HealthBar>(1.6f);
        AddChild(m_HealthBar);
    }
    m_HealthBar->SetVisible(true);
    updateHealthBar();
}

void Enemy::despawn() {
    m_IsActive = false;
    m_IsBlocked = false;
    m_TargetOperator = nullptr;
    m_AttackTimer = 0.0f;
    m_IsAttackVisualActive = false;
    SetVisible(false);
    if (m_HealthBar) m_HealthBar->SetVisible(false);
}

void Enemy::update(float deltaTime) {
    if (!m_IsActive || m_State == State::DEAD) return;

    if (m_State == State::DYING) {
        if (m_DieAnimation && m_DieAnimation->GetState() == Util::Animation::State::ENDED) {
            m_State = State::DEAD;
            despawn();
        }
        return;
    }

    if (m_CurrentWaypointIndex + 1 < m_GridWaypoints.size()) {
        glm::vec2 target = m_GridWaypoints[m_CurrentWaypointIndex + 1];
        glm::vec2 dir = target - m_CurrentGridPos;

        // Flip sprite based on movement direction (y is column/horizontal)
        // Default is facing right
        if (dir.y > 0) {
            m_Transform.scale.x = m_BaseScale; // face right
        } else if (dir.y < 0) {
            m_Transform.scale.x = -m_BaseScale; // Flip to face left
        }
    }

    if (m_IsBlocked) {
        // Block state is rebuilt each frame by GameScene.
        // Only attack when target is still valid.

        if (!m_TargetOperator || !m_TargetOperator->isAlive() || !m_TargetOperator->GetVisible()) {
            m_IsBlocked = false;
            m_TargetOperator = nullptr;
            m_AttackTimer = 0.0f;
            return;
        }

        if (m_AttackAnimation && !m_IsAttackVisualActive) {
            SetDrawable(m_AttackAnimation);
            m_AttackAnimation->Play();
            m_IsAttackVisualActive = true;
        }

        if (m_AttackTimer > 0.0f) {
            m_AttackTimer -= deltaTime;
        } else {
            m_TargetOperator->takeDamage(m_Attack);
            m_AttackTimer = m_AttackInterval;
        }
        return;
    }

    if (m_IsAttackVisualActive && m_MoveAnimation) {
        SetDrawable(m_MoveAnimation);
        m_MoveAnimation->Play();
        m_IsAttackVisualActive = false;
    }

    if (m_CurrentWaypointIndex + 1 < m_GridWaypoints.size()) {
        glm::vec2 target = m_GridWaypoints[m_CurrentWaypointIndex + 1];
        glm::vec2 dir = target - m_CurrentGridPos;
        float dist = glm::length(dir);
        float moveDist = m_Speed * deltaTime;

        if (moveDist >= dist) {
            m_CurrentGridPos = target;
            m_CurrentWaypointIndex++;
        } else {
            m_CurrentGridPos += (dir / dist) * moveDist;
        }

        // Project to screen
        glm::vec3 p = m_Homography * glm::vec3(m_CurrentGridPos.y, m_CurrentGridPos.x, 1.0f);
        m_Transform.translation = {p.x / p.z, p.y / p.z};
    } else {
        m_ReachedEnd = true;
        despawn();
    }

    updateHealthBar();
}

void Enemy::setAnimation(const std::vector<std::string>& animationPaths) {
    m_MoveAnimation = std::make_shared<Util::Animation>(animationPaths, true, 50, true, 0);
    // Refresh pivot if size changed
    SetPivot({0, -m_MoveAnimation->GetSize().y / 2.0f});
    if (m_State == State::ALIVE && !m_IsAttackVisualActive) {
        SetDrawable(m_MoveAnimation);
    }
}

void Enemy::setAttackAnimation(const std::vector<std::string>& attackAnimationPaths) {
    m_AttackAnimation = std::make_shared<Util::Animation>(attackAnimationPaths, true, 50, true, 0);
}

void Enemy::setDieAnimation(const std::vector<std::string>& dieAnimationPaths) {
    m_DieAnimation = std::make_shared<Util::Animation>(dieAnimationPaths, false, 50, false, 0);
}

void Enemy::updateHealthBar() {
    if (m_HealthBar) {
        m_HealthBar->SetValue(m_Hp, m_MaxHp);
        // Position at the bottom of enemy
        m_HealthBar->Update(m_Transform.translation, -10.0f);
    }
}

void Enemy::startDeath() {
    m_State = State::DYING;
    m_IsAttackVisualActive = false;
    if (m_DieAnimation) {
        SetDrawable(m_DieAnimation);
        m_DieAnimation->Play();
    } else {
        m_State = State::DEAD;
        despawn();
    }
}

std::vector<glm::vec2> Enemy::parsePathString(const std::string& pathString) {
    std::vector<glm::vec2> result;
    std::stringstream ss(pathString);
    std::string point;
    while (std::getline(ss, point, ';')) {
        size_t comma = point.find(',');
        if (comma != std::string::npos) {
            try {
                float r_val = std::stof(point.substr(0, comma));
                float c_val = std::stof(point.substr(comma + 1));
                result.push_back({r_val, c_val});
            } catch (...) {}
        }
    }
    return result;
}

EnemyPool::EnemyPool(std::size_t size, const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& waypoints) {
    m_Pool.reserve(size);
    m_Available.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        auto enemy = std::make_shared<Enemy>(animationPaths, waypoints);
        m_Pool.push_back(enemy);
        m_Available.push_back(enemy.get());
    }
}

Enemy* EnemyPool::getEnemy() {
    if (m_Available.empty()) {
        return nullptr;
    }
    Enemy* enemy = m_Available.back();
    m_Available.pop_back();
    return enemy;
}

void EnemyPool::returnEnemy(Enemy* enemy) {
    enemy->despawn();
    m_Available.push_back(enemy);
}

} // namespace Arknights
