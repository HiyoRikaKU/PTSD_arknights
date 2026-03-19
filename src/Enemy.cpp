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
    SetDrawable(std::make_shared<Util::Animation>(animationPaths, true, 50, true, 100));
    SetZIndex(1.5f);
    SetVisible(true);

    // Shrink the enemy sprite so it takes less screen space.
    constexpr float ENEMY_SCALE = 0.5F;
    // Negative X to flip horizontally so the sprite faces left.
    m_Transform.scale = glm::vec2{-ENEMY_SCALE, ENEMY_SCALE};

    if (!m_Waypoints.empty()) {
        m_Transform.translation = m_Waypoints[0]; // Spawn at first waypoint
    }
}

void Enemy::Update(float deltaTime) {
    if (m_ReachedEnd || m_Waypoints.size() < 2 || m_CurrentWaypointIndex >= m_Waypoints.size()) {
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
