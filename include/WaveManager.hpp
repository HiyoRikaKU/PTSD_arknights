#ifndef WAVEMANAGER_HPP
#define WAVEMANAGER_HPP

#include <vector>
#include <string>
#include <queue>
#include <glm/vec2.hpp>

struct SpawnEvent {
    float spawnTimeMs;
    std::string enemyType; // e.g., "gopro", "bigbo"
    float hp;
    float speed;
};

class WaveManager {
public:
    WaveManager() = default;

    void AddSpawnEvent(float timeMs, const std::string& type, float hp, float speed) {
        m_Timeline.push({timeMs, type, hp, speed});
    }

    bool ShouldSpawn(float currentTimeMs, SpawnEvent& outEvent) {
        if (!m_Timeline.empty() && currentTimeMs >= m_Timeline.front().spawnTimeMs) {
            outEvent = m_Timeline.front();
            m_Timeline.pop();
            return true;
        }
        return false;
    }

    bool IsFinished() const {
        return m_Timeline.empty();
    }

private:
    // A queue of events sorted by spawnTimeMs (assuming added in order)
    std::queue<SpawnEvent> m_Timeline;
};

#endif
