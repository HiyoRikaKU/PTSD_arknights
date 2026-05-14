#ifndef ARKNIGHTS_CORE_SCENEMANAGER_HPP
#define ARKNIGHTS_CORE_SCENEMANAGER_HPP

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "Arknights/Scenes/Scene.hpp"

namespace Arknights {
namespace Core {

class SceneManager {
public:
    static SceneManager& getInstance();

    void pushScene(std::shared_ptr<Scene> scene);
    void popScene();
    void replaceScene(std::shared_ptr<Scene> scene);
    
    void update(float deltaTime);
    void render();
    void clear();

    void setPreparedGameScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<Scene> takePreparedGameScene();
    
    Scene* getCurrentScene() const;
    bool hasScene() const { return !m_SceneStack.empty(); }

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

private:
    void cleanupScene(const std::shared_ptr<Scene>& scene);
    void flushDeferredCleanup();

private:
    std::stack<std::shared_ptr<Scene>> m_SceneStack;
    std::shared_ptr<Scene> m_PreparedGameScene = nullptr;
    std::vector<std::shared_ptr<Scene>> m_DeferredCleanupScenes;
    bool m_IsUpdating = false;
};

} // namespace Core
} // namespace Arknights

#endif
