#include "Enemy.hpp"
#include "Util/Animation.hpp"
#include <sstream>
#include <iomanip>

Enemy::Enemy(const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& waypoints)
    : m_Waypoints(waypoints) {
    Init(animationPaths);
}

Enemy::Enemy(const std::vector<std::string>& animationPaths, const std::string& pathString)
    : m_Waypoints(ParsePathString(pathString)) {
    Init(animationPaths);
}

void Enemy::Init(const std::vector<std::string>& animationPaths) {
    auto animation = std::make_shared<Util::Animation>(animationPaths, true, 50, true, 100);
    SetDrawable(animation);
    SetZIndex(1.5f);
    SetVisible(false);

    // Set pivot to bottom center (0, -height/2)
    SetPivot({0, -animation->GetSize().y / 2.0f});

    // Shrink the enemy sprite so it takes less screen space and face left.
    constexpr float ENEMY_SCALE = 0.3F;
    m_Transform.scale = glm::vec2{-ENEMY_SCALE, ENEMY_SCALE};

    if (!m_Waypoints.empty()) {
        m_Transform.translation = m_Waypoints[0]; // Spawn at first waypoint
    }
}

void Enemy::Spawn(const glm::vec2 &startPos, const std::vector<glm::vec2> &path,
                  float hp, float speed) {
    m_Waypoints = path;
    m_Transform.translation = startPos;
    m_CurrentWaypointIndex = m_Waypoints.size() > 1 ? 1 : 0;
    m_ReachedEnd = false;
    m_IsActive = true;
    m_IsBlocked = false;
    m_Hp = hp;
    m_Speed = speed;
    SetVisible(true);
}

void Enemy::Despawn() {
    m_IsActive = false;
    m_ReachedEnd = true;
    m_IsBlocked = false;
    m_Hp = 0.0F;
    m_Speed = 0.0F;
    SetVisible(false);
}

void Enemy::SetAnimation(const std::vector<std::string>& animationPaths) {
    SetDrawable(std::make_shared<Util::Animation>(animationPaths, true, 50, true, 100));
}

void Enemy::Update(float deltaTime) {
    if (!m_IsActive || m_Waypoints.size() < 2 || m_CurrentWaypointIndex >= m_Waypoints.size() || m_IsBlocked) {
        return;
    }

    glm::vec2 target = m_Waypoints[m_CurrentWaypointIndex];
    glm::vec2 direction = target - m_Transform.translation;
    float distance = glm::length(direction);

    float step = m_Speed * deltaTime;

    if (distance < step) {
        m_Transform.translation = target;
        m_CurrentWaypointIndex++;
        if (m_CurrentWaypointIndex >= m_Waypoints.size()) {
            m_ReachedEnd = true;
            m_IsActive = false; // Mark inactive to allow pool return.
        }
    } else {
        m_Transform.translation += glm::normalize(direction) * step;
    }
}

std::vector<glm::vec2> Enemy::ParsePathString(const std::string& pathString) {
    std::vector<glm::vec2> result;
    std::stringstream ss(pathString);
    std::string point;
    while (std::getline(ss, point, ';')) {
        size_t comma = point.find(',');
        if (comma != std::string::npos) {
            try {
                float x = std::stof(point.substr(0, comma));
                float y = std::stof(point.substr(comma + 1));
                result.push_back({x, y});
            } catch (...) {
                // Ignore invalid points
            }
        }
    }
    return result;
}
