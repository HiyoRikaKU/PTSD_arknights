#ifndef ARKNIGHTS_ENEMY_HPP
#define ARKNIGHTS_ENEMY_HPP

#include <queue>
#include <vector>
#include <string>

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"

namespace Arknights {

class Enemy : public Util::GameObject {
public:
    Enemy(const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& gridWaypoints);
    Enemy(const std::vector<std::string>& animationPaths, const std::string& pathString);

    void spawn(const std::vector<glm::vec2>& gridPath, float hp, float speed, const glm::mat3& homography);
    void despawn();

    bool isActive() const { return m_IsActive; }

    void update(float deltaTime);
    bool reachedEnd() const { return m_ReachedEnd; }
    void setSpeed(float speed) { m_Speed = speed; }
    void setAnimation(const std::vector<std::string>& animationPaths);

    bool isBlocked() const { return m_IsBlocked; }
    void setBlocked(bool blocked) { m_IsBlocked = blocked; }
    glm::vec2 getPosition() const { return m_Transform.translation; }

private:
    void init(const std::vector<std::string>& animationPaths);
    static std::vector<glm::vec2> parsePathString(const std::string& pathString);

private:
    std::vector<glm::vec2> m_GridWaypoints;
    glm::vec2 m_CurrentGridPos;
    glm::mat3 m_Homography;

    std::size_t m_CurrentWaypointIndex = 0;
    float m_Speed = 0.0002f; // Grid units per ms
    float m_Hp = 100.0F;
    bool m_IsActive = false;
    bool m_ReachedEnd = false;
    bool m_IsBlocked = false;
};

} // namespace Arknights

#endif
