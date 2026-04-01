#include "Arknights/Scenes/LoginScene.hpp"
#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"

namespace Arknights {

LoginScene::LoginScene() {
}

void LoginScene::init() {
    LOG_DEBUG("Initializing LoginScene");
    
    createLoginUI();
    
    // Load login BGM
    m_LoginBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/login.mp3");
    
    m_Initialized = true;
}

void LoginScene::createLoginUI() {
    // Login page background
    m_LoginBackground = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/login_page.jpg"),
        0
    );
    // Scale to fit screen (1280x720)
    m_LoginBackground->m_Transform.scale = glm::vec2(1280.0f, 720.0f) / m_LoginBackground->GetScaledSize();
    m_Root.AddChild(m_LoginBackground);
    
    // Create START button in the center-bottom
    m_StartButton = std::make_shared<UI::Button>(
        "START",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        48,
        glm::vec2(0, -200),
        glm::vec2(200, 60),
        10.0f
    );
    m_Root.AddChild(m_StartButton);
    
    // Set button callback to transition to lobby
    m_StartButton->setOnClick([this]() { onStartButtonClicked(); });
    
    // Optional: Add glow/pulse effect to START button
    m_StartButton->setHoverScale(1.1f);
    m_StartButton->setClickScale(0.95f);
}

void LoginScene::update(float deltaTime) {
    if (!m_Initialized) return;
    
    // Update START button
    m_StartButton->update(deltaTime);
    
    // Future: Can add animated effects, tips rotation, etc.
}

void LoginScene::cleanup() {
    LOG_DEBUG("Cleaning up LoginScene");
    m_LoginBGM.reset();
    m_StartButton.reset();
    m_LoginBackground.reset();
}

void LoginScene::onEnter() {
    LOG_DEBUG("Entering LoginScene");
    
    // Play login BGM
    if (m_LoginBGM) {
        m_LoginBGM->Play();
    }
}

void LoginScene::onExit() {
    LOG_DEBUG("Exiting LoginScene");
    
    // Fade out BGM when transitioning to lobby
    if (m_LoginBGM) {
        m_LoginBGM->FadeOut(1000); // 1 second fade
    }
}

void LoginScene::onStartButtonClicked() {
    LOG_INFO("START button clicked - Transitioning to lobby");
    
    // Create and replace with LobbyScene
    // Using replaceScene instead of pushScene because user should not return to login
    auto lobbyScene = std::make_shared<LobbyScene>();
    Core::SceneManager::getInstance().replaceScene(lobbyScene);
}

} // namespace Arknights
