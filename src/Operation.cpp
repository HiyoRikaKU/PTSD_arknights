#include "Operation.hpp"

Operation::Operation(const std::string& mapPath, const std::vector<glm::vec2>& waypoints)
    : m_Waypoints(waypoints) {
    m_Map = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(mapPath),
        0
    );
    // Scale map to 1600x900 as per App.cpp requirement
    m_Map->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Map->GetScaledSize();
    m_Map->SetVisible(false);
}
