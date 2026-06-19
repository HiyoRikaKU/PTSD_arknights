#include "Arknights/Scenes/ReturnLoadingScene.hpp"
#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Image.hpp"
#include "config.hpp"

namespace Arknights {

void ReturnLoadingScene::init() {
    LOG_DEBUG("Initializing ReturnLoadingScene");
    m_LoadingImage = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/loadingScene/LoadingScene_1.jpg"),
        1
    );
    m_LoadingImage->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoadingImage->GetScaledSize();
    m_Root.AddChild(m_LoadingImage);

    m_Initialized = true;
}

void ReturnLoadingScene::update(float deltaTime) {
    if (deltaTime > 50.0f) {
        deltaTime = 50.0f;
    }
    m_Timer += deltaTime;
    if (m_Timer >= DURATION_MS) {
        Core::SceneManager::getInstance().replaceScene(std::make_shared<LobbyScene>());
    }
}

void ReturnLoadingScene::cleanup() {
    LOG_DEBUG("Cleaning up ReturnLoadingScene");
    m_LoadingImage.reset();
}

void ReturnLoadingScene::onEnter() {
    LOG_DEBUG("Entering ReturnLoadingScene");
}

void ReturnLoadingScene::onExit() {
    LOG_DEBUG("Exiting ReturnLoadingScene");
}

} // namespace Arknights
