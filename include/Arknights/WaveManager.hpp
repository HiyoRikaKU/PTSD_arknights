#ifndef ARKNIGHTS_WAVEMANAGER_HPP
#define ARKNIGHTS_WAVEMANAGER_HPP

#include <vector>
#include <string>

namespace Arknights {

struct SpawnEvent {
    float timestamp;
    std::string enemyType;
    float hp;
    float speed;
};

class WaveManager {
public:
    void addSpawnEvent(float timestamp, const std::string& enemyType, float hp, float speed) {
        m_Timeline.push_back({timestamp, enemyType, hp, speed});
    }

    bool shouldSpawn(float currentTime, SpawnEvent& outEvent) {
        if (m_CurrentEventIndex < m_Timeline.size() && currentTime >= m_Timeline[m_CurrentEventIndex].timestamp) {
            outEvent = m_Timeline[m_CurrentEventIndex];
            m_CurrentEventIndex++;
            return true;
        }
        return false;
    }

private:
    std::vector<SpawnEvent> m_Timeline;
    std::size_t m_CurrentEventIndex = 0;
};

} // namespace Arknights

#endif
