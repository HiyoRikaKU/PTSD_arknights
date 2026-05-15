#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"

namespace Arknights {
namespace Core {

void SceneManager::cleanupScene(const std::shared_ptr<Scene>& scene) {
    if (!scene) {
        return;
    }
    if (m_IsUpdating) {
        m_DeferredCleanupScenes.push_back(scene);
        return;
    }
    scene->cleanup();
}

void SceneManager::flushDeferredCleanup() {
    for (const auto& scene : m_DeferredCleanupScenes) {
        if (scene) {
            scene->cleanup();
        }
    }
    m_DeferredCleanupScenes.clear();
}

SceneManager& SceneManager::getInstance() {
    static SceneManager instance;
    return instance;
}

void SceneManager::pushScene(std::shared_ptr<Scene> scene) {
    if (!scene) {
        LOG_ERROR("Attempted to push null scene");
        return;
    }

    // Exit current scene if exists
    if (!m_SceneStack.empty()) {
        m_SceneStack.top()->onExit();
    }

    // Push and initialize new scene
    m_SceneStack.push(scene);
    
    if (!scene->isInitialized()) {
        scene->init();
    }
    
    scene->onEnter();
    
    LOG_DEBUG("Scene pushed. Stack size: {}", m_SceneStack.size());
}

void SceneManager::popScene() {
    if (m_SceneStack.empty()) {
        LOG_WARN("Attempted to pop from empty scene stack");
        return;
    }

    // Cleanup and exit current scene
    m_SceneStack.top()->onExit();
    cleanupScene(m_SceneStack.top());
    m_SceneStack.pop();

    // Enter previous scene if exists
    if (!m_SceneStack.empty()) {
        m_SceneStack.top()->onEnter();
    }
    
    LOG_DEBUG("Scene popped. Stack size: {}", m_SceneStack.size());
}

void SceneManager::replaceScene(std::shared_ptr<Scene> scene) {
    if (!scene) {
        LOG_ERROR("Attempted to replace with null scene");
        return;
    }

    // Remove current scene without re-entering previous
    if (!m_SceneStack.empty()) {
        m_SceneStack.top()->onExit();
        cleanupScene(m_SceneStack.top());
        m_SceneStack.pop();
    }

    // Push new scene
    m_SceneStack.push(scene);
    
    if (!scene->isInitialized()) {
        scene->init();
    }
    
    scene->onEnter();
    
    LOG_DEBUG("Scene replaced. Stack size: {}", m_SceneStack.size());
}

void SceneManager::update(float deltaTime) {
    if (!m_SceneStack.empty()) {
        m_IsUpdating = true;
        m_SceneStack.top()->update(deltaTime);
        if (!m_SceneStack.empty()) {
            m_SceneStack.top()->getRoot().Update();
        }
        m_IsUpdating = false;
        flushDeferredCleanup();
    }
}

void SceneManager::render() {
    if (!m_SceneStack.empty()) {
        m_SceneStack.top()->render();
    }
}

void SceneManager::clear() {
    while (!m_SceneStack.empty()) {
        m_SceneStack.top()->onExit();
        cleanupScene(m_SceneStack.top());
        m_SceneStack.pop();
    }
    flushDeferredCleanup();
    LOG_DEBUG("SceneManager cleared");
}

void SceneManager::setPreparedGameScene(std::shared_ptr<Scene> scene) {
    m_PreparedGameScene = std::move(scene);
}

std::shared_ptr<Scene> SceneManager::takePreparedGameScene() {
    auto prepared = m_PreparedGameScene;
    m_PreparedGameScene.reset();
    return prepared;
}

Scene* SceneManager::getCurrentScene() const {
    if (m_SceneStack.empty()) {
        return nullptr;
    }
    return m_SceneStack.top().get();
}

} // namespace Core
} // namespace Arknights
