#include "Arknights/Enemy.hpp"

#include <sstream>
#include <iomanip>

#include "Util/Animation.hpp"

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
    auto animation = std::make_shared<Util::Animation>(animationPaths, true, 50, true, 100);
    SetDrawable(animation);
    SetZIndex(1.5f);
    SetVisible(false);

    // Set pivot to bottom center (0, -height/2)
    SetPivot({0, -animation->GetSize().y / 2.0f});

    // Shrink the enemy sprite so it takes less screen space and face left.
    constexpr float ENEMY_SCALE = 0.3F;
    m_Transform.scale = glm::vec2{-ENEMY_SCALE, ENEMY_SCALE};
}

void Enemy::spawn(const std::vector<glm::vec2>& gridPath, float hp, float speed, const glm::mat3& homography) {
    m_GridWaypoints = gridPath;
    m_Homography = homography;
    m_CurrentWaypointIndex = m_GridWaypoints.size() > 1 ? 1 : 0;
    m_ReachedEnd = false;
    m_IsActive = true;
    m_IsBlocked = false;
    m_Hp = hp;
    m_Speed = speed; // Grid units per ms

    if (!m_GridWaypoints.empty()) {
        m_CurrentGridPos = m_GridWaypoints[0];
        glm::vec3 p = m_Homography * glm::vec3(m_CurrentGridPos.y, m_CurrentGridPos.x, 1.0f);
        m_Transform.translation = {p.x / p.z, p.y / p.z};
    }

    SetVisible(true);
}

void Enemy::despawn() {
    m_IsActive = false;
    m_ReachedEnd = true;
    m_IsBlocked = false;
    m_Hp = 0.0F;
    m_Speed = 0.0F;
    m_Transform.rotation = 0.0f;
    SetVisible(false);
}

void Enemy::setAnimation(const std::vector<std::string>& animationPaths) {
    SetDrawable(std::make_shared<Util::Animation>(animationPaths, true, 50, true, 100));
}

void Enemy::update(float deltaTime) {
    if (!m_IsActive || m_GridWaypoints.size() < 2 || m_CurrentWaypointIndex >= m_GridWaypoints.size() || m_IsBlocked) {
        return;
    }

    glm::vec2 targetGrid = m_GridWaypoints[m_CurrentWaypointIndex];
    glm::vec2 direction = targetGrid - m_CurrentGridPos;
    float distance = glm::length(direction);

    float step = m_Speed * deltaTime;

    if (distance < step) {
        m_CurrentGridPos = targetGrid;
        m_CurrentWaypointIndex++;
        if (m_CurrentWaypointIndex >= m_GridWaypoints.size()) {
            m_ReachedEnd = true;
            m_IsActive = false; 
        }
    } else {
        m_CurrentGridPos += glm::normalize(direction) * step;
    }

    // Project current grid position to world coordinates
    glm::vec3 p = m_Homography * glm::vec3(m_CurrentGridPos.y, m_CurrentGridPos.x, 1.0f); // c=y, r=x
    m_Transform.translation = {p.x / p.z, p.y / p.z};
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

} // namespace Arknights
