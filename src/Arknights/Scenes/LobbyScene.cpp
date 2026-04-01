#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace Arknights {

LobbyScene::LobbyScene() {
}

void LobbyScene::init() {
    LOG_DEBUG("Initializing LobbyScene");
    
    // Create main lobby UI elements
    createBackground();
    createUserInfo();
    createResourceDisplay();
    createMainButtons();
    createTimeDisplay();
    
    // Load lobby BGM
    m_LobbyBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/battle/battle.mp3");
    
    m_Initialized = true;
}

void LobbyScene::createBackground() {
    // Main background - use UI_HOME_FRONT_BKG for authentic Arknights look
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_HOME_FRONT_BKG.png"),
        0
    );
    m_Background->m_Transform.scale = glm::vec2(1280.0f, 720.0f) / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);
    // Character art (use Chen from arknights-ui or Amiya from existing resources)
    try {
        // Try to use Chen character art first
        m_CharacterArt = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/Haze_Skin_1.png"),
            1
        );
        m_CharacterArt->m_Transform.translation = {-200, -50};
        m_CharacterArt->m_Transform.scale = glm::vec2(0.6f);
        m_Root.AddChild(m_CharacterArt);
    } catch (...) {
        // Fallback to Amiya
        try {
            m_CharacterArt = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/char_002_amiya/Idle_01.png"),
                1
            );
            m_CharacterArt->m_Transform.translation = {-300, -100};
            m_CharacterArt->m_Transform.scale = glm::vec2(0.8f);
            m_Root.AddChild(m_CharacterArt);
        } catch (...) {
            LOG_WARN("Could not load character art");
        }
    }
}

void LobbyScene::createUserInfo() {
    // User Level (top-left)
    m_UserLevel = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        36,
        "LV 45",
        Util::Color(255, 255, 255)
    );
    auto levelObj = std::make_shared<Util::GameObject>(m_UserLevel, 10);
    levelObj->m_Transform.translation = {-550, 300};
    m_Root.AddChild(levelObj);
    
    // User Name
    m_UserName = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        24,
        "Doctor",
        Util::Color(255, 255, 255)
    );
    auto nameObj = std::make_shared<Util::GameObject>(m_UserName, 10);
    nameObj->m_Transform.translation = {-550, 250};
    m_Root.AddChild(nameObj);
    
    // User ID
    m_UserID = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        18,
        "ID: 12345678",
        Util::Color(200, 200, 200)
    );
    auto idObj = std::make_shared<Util::GameObject>(m_UserID, 10);
    idObj->m_Transform.translation = {-550, 220};
    m_Root.AddChild(idObj);
    
    // Dialog text (bottom-left)
    m_DialogText = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        "博士，準備好作戰了嗎？",
        Util::Color(255, 255, 255)
    );
    auto dialogObj = std::make_shared<Util::GameObject>(m_DialogText, 10);
    dialogObj->m_Transform.translation = {-450, -280};
    m_Root.AddChild(dialogObj);
}

void LobbyScene::createResourceDisplay() {
    // Money display (top-right area)
    m_Money = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        "💰 1000",
        Util::Color(255, 255, 255)
    );
    auto moneyObj = std::make_shared<Util::GameObject>(m_Money, 10);
    moneyObj->m_Transform.translation = {350, 310};
    m_Root.AddChild(moneyObj);
    
    // Jasper
    m_Jasper = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        "💎 1000",
        Util::Color(255, 255, 255)
    );
    auto jasperObj = std::make_shared<Util::GameObject>(m_Jasper, 10);
    jasperObj->m_Transform.translation = {470, 310};
    m_Root.AddChild(jasperObj);
    
    // Stone
    m_Stone = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        "🔷 1000",
        Util::Color(255, 255, 255)
    );
    auto stoneObj = std::make_shared<Util::GameObject>(m_Stone, 10);
    stoneObj->m_Transform.translation = {590, 310};
    m_Root.AddChild(stoneObj);
    
    // Sanity display
    m_SanityDisplay = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        24,
        "理智 125/125",
        Util::Color(100, 200, 255)
    );
    auto sanityObj = std::make_shared<Util::GameObject>(m_SanityDisplay, 10);
    sanityObj->m_Transform.translation = {450, 260};
    m_Root.AddChild(sanityObj);
    
    // Current stage info
    m_CurrentStage = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        18,
        "當前: 0-1 行動準備",
        Util::Color(255, 255, 255)
    );
    auto stageObj = std::make_shared<Util::GameObject>(m_CurrentStage, 10);
    stageObj->m_Transform.translation = {450, 220};
    m_Root.AddChild(stageObj);
}

void LobbyScene::createMainButtons() {
    // Stage/Combat Button (作戰)
    m_StageButton = std::make_shared<UI::Button>(
        "作戰",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        28,
        glm::vec2(450, 140),
        glm::vec2(180, 60),
        15
    );
    m_StageButton->setOnClick([this]() { onStageButtonClicked(); });
    m_Buttons.push_back(m_StageButton);
    m_Root.AddChild(m_StageButton);
    
    // Team Button (編隊)
    m_TeamButton = std::make_shared<UI::Button>(
        "編隊",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        24,
        glm::vec2(350, 60),
        glm::vec2(150, 50),
        15
    );
    m_TeamButton->setOnClick([this]() { onTeamButtonClicked(); });
    m_Buttons.push_back(m_TeamButton);
    m_Root.AddChild(m_TeamButton);
    
    // Operator Button (幹員)
    m_OperatorButton = std::make_shared<UI::Button>(
        "幹員",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        24,
        glm::vec2(550, 60),
        glm::vec2(150, 50),
        15
    );
    m_OperatorButton->setOnClick([this]() { onOperatorButtonClicked(); });
    m_Buttons.push_back(m_OperatorButton);
    m_Root.AddChild(m_OperatorButton);
    
    // Additional buttons (placeholders)
    auto taskButton = std::make_shared<UI::Button>(
        "任務",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        glm::vec2(350, -40),
        glm::vec2(130, 45),
        15
    );
    m_Buttons.push_back(taskButton);
    m_Root.AddChild(taskButton);
    
    auto baseButton = std::make_shared<UI::Button>(
        "基建",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        glm::vec2(500, -40),
        glm::vec2(130, 45),
        15
    );
    m_Buttons.push_back(baseButton);
    m_Root.AddChild(baseButton);
}

void LobbyScene::createTimeDisplay() {
    m_TimeDisplay = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        16,
        "2024/03/31 14:20",
        Util::Color(255, 255, 255)
    );
    auto timeObj = std::make_shared<Util::GameObject>(m_TimeDisplay, 10);
    timeObj->m_Transform.translation = {500, 340};
    m_Root.AddChild(timeObj);
}

void LobbyScene::update(float deltaTime) {
    // Update time display every second
    m_TimeCounter += deltaTime;
    if (m_TimeCounter >= 1000.0f) {
        m_TimeCounter = 0.0f;
        
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::stringstream ss;
        ss << std::put_time(localTime, "%Y/%m/%d %H:%M");
        
        if (m_TimeDisplay) {
            m_TimeDisplay->SetText(ss.str());
        }
    }
    
    // Update buttons
    for (auto& button : m_Buttons) {
        button->update(deltaTime);
    }
}

void LobbyScene::cleanup() {
    LOG_DEBUG("Cleaning up LobbyScene");
    m_Buttons.clear();
    m_LobbyBGM.reset();
}

void LobbyScene::onEnter() {
    LOG_DEBUG("Entering LobbyScene");
    if (m_LobbyBGM) {
        m_LobbyBGM->Play();
    }
}

void LobbyScene::onExit() {
    LOG_DEBUG("Exiting LobbyScene");
    if (m_LobbyBGM) {
        m_LobbyBGM->FadeOut(500);
    }
}

void LobbyScene::onStageButtonClicked() {
    LOG_INFO("Stage button clicked - Starting game...");
    
    // Push GameScene
    auto gameScene = std::make_shared<GameScene>();
    Core::SceneManager::getInstance().pushScene(gameScene);
}

void LobbyScene::onOperatorButtonClicked() {
    LOG_INFO("Operator button clicked");
    // Future: Show operator management screen
}

void LobbyScene::onTeamButtonClicked() {
    LOG_INFO("Team button clicked");
    // Future: Show team formation screen
}

} // namespace Arknights
