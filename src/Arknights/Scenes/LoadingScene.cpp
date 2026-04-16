#include "Arknights/Scenes/LoadingScene.hpp"
#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

namespace Arknights {

LoadingScene::LoadingScene() {
}

void LoadingScene::init() {
    LOG_DEBUG("Initializing LoadingScene");
    
    // Create Background using LoadingScene_1.jpeg
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/LoadingScene_1.jpeg"),
        0
    );
    
    // Fill the screen
    m_Background->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);

    // Create Text
    auto text = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        48,
        "LOADING...",
        Util::Color(255, 255, 255)
    );
    m_LoadingText = std::make_shared<Util::GameObject>(text, 10);
    // Bottom right area
    m_LoadingText->m_Transform.translation = {600, -380}; 
    m_Root.AddChild(m_LoadingText);

    m_Initialized = true;
}

void LoadingScene::update(float deltaTime) {
    m_Timer += deltaTime;
    
    // Wait for at least 1-2 frames to ensure the screen is drawn
    if (m_Timer > 50.0f && !m_HasStartedLoading) {
        m_HasStartedLoading = true;
        LOG_INFO("LoadingScreen visible, starting GameScene initialization...");
        
        auto gameScene = std::make_shared<GameScene>();
        Core::SceneManager::getInstance().replaceScene(gameScene);
    }
}

void LoadingScene::cleanup() {
    LOG_DEBUG("Cleaning up LoadingScene");
    m_Background.reset();
    m_LoadingText.reset();
}

void LoadingScene::onEnter() {
    LOG_DEBUG("Entering LoadingScene");
}

void LoadingScene::onExit() {
    LOG_DEBUG("Exiting LoadingScene");
}

} // namespace Arknights
