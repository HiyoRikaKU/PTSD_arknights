#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include <queue>
#include <vector>
#include <string>

class Enemy : public Util::GameObject {
public:
    // Lifecycle helpers for object-pool usage.
    void Spawn(const glm::vec2 &startPos, const std::vector<glm::vec2> &path,
               float hp, float speed);
    void Despawn();

    bool IsActive() const { return m_IsActive; }

    /**
     * @brief Construct an Enemy with a vector of waypoints.
     * @param animationPaths Paths to animation frames.
     * @param waypoints Vector of glm::vec2 points defining the movement path.
     */
    Enemy(const std::vector<std::string>& animationPaths, const std::vector<glm::vec2>& waypoints);

    /**
     * @brief Construct an Enemy with a path string.
     * @param animationPaths Paths to animation frames.
     * @param pathString String format: "x1,y1; x2,y2; x3,y3"
     */
    Enemy(const std::vector<std::string>& animationPaths, const std::string& pathString);

    void Update(float deltaTime);
    bool ReachedEnd() const { return m_ReachedEnd; }
    void SetSpeed(float speed) { m_Speed = speed; }
    void SetAnimation(const std::vector<std::string>& animationPaths);

    bool IsBlocked() const { return m_IsBlocked; }
    void SetBlocked(bool blocked) { m_IsBlocked = blocked; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }

private:
    void Init(const std::vector<std::string>& animationPaths);
    static std::vector<glm::vec2> ParsePathString(const std::string& pathString);

    std::vector<glm::vec2> m_Waypoints;
    size_t m_CurrentWaypointIndex = 0;
    float m_Speed = 0.2f; // Pixels per ms
    float m_Hp = 100.0F;
    bool m_IsActive = false;
    bool m_ReachedEnd = false;
    bool m_IsBlocked = false;
};

#endif
