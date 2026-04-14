#ifndef ARKNIGHTS_CORE_SCENEMANAGER_HPP
#define ARKNIGHTS_CORE_SCENEMANAGER_HPP

#include <memory>
#include <stack>
#include <string>

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
    
    Scene* getCurrentScene() const;
    bool hasScene() const { return !m_SceneStack.empty(); }

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

private:
    std::stack<std::shared_ptr<Scene>> m_SceneStack;
};

} // namespace Core
} // namespace Arknights

#endif
