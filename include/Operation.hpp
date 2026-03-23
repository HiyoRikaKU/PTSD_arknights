#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <string>
#include <vector>
#include <memory>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Operation {
public:
    Operation(const std::string& mapPath, const std::vector<glm::vec2>& waypoints);

    std::shared_ptr<Util::GameObject> GetMap() const { return m_Map; }
    const std::vector<glm::vec2>& GetWaypoints() const { return m_Waypoints; }

private:
    std::shared_ptr<Util::GameObject> m_Map;
    std::vector<glm::vec2> m_Waypoints;
};

#endif
