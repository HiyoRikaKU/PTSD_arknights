#ifndef ARKNIGHTS_SCENE_HPP
#define ARKNIGHTS_SCENE_HPP

#include <memory>
#include "Util/Renderer.hpp"

namespace Arknights {

class Scene {
public:
    virtual ~Scene() = default;

    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() {}
    virtual void cleanup() {}

    virtual void onEnter() {}
    virtual void onExit() {}

    bool isInitialized() const { return m_Initialized; }
    
    Util::Renderer& getRoot() { return m_Root; }

protected:
    Util::Renderer m_Root;
    bool m_Initialized = false;
};

} // namespace Arknights

#endif
