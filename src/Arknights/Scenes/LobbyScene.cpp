#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/StageSelectScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

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
    
    m_Initialized = true;
}

void LobbyScene::createBackground() {
    // Main background - use UI_HOME_FRONT_BKG for authentic Arknights look
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_HOME_FRONT_BKG.png"),
        0
    );
    m_Background->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);
    createCharacterSlider();
}

void LobbyScene::createCharacterSlider() {
    m_CharacterArtPaths = {
        std::string(RESOURCE_DIR) + "/charactor/Yahata_Umiri_Elite.png",
        std::string(RESOURCE_DIR) + "/charactor/Haze_Skin_1.png"
    };
    m_CharacterScales = {
        {0.4f, 0.4f},   // Yahata_Umiri_Elite
        {0.6f, 0.6f}  // Haze_Skin_1
    };

    m_CharacterArts.clear();
    for (std::size_t i = 0; i < m_CharacterArtPaths.size(); ++i) {
        auto art = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(m_CharacterArtPaths[i]),
            1
        );
        const glm::vec2 scale = i < m_CharacterScales.size() ? m_CharacterScales[i] : glm::vec2(0.4f, 0.4f);
        art->m_Transform.translation = m_CharacterBasePos;
        art->m_Transform.scale = scale;
        art->SetVisible(false);
        m_Root.AddChild(art);
        m_CharacterArts.push_back(art);
    }

    if (!m_CharacterArts.empty()) {
        m_CurrentCharacterIndex = 0;
        m_CharacterArts[m_CurrentCharacterIndex]->SetVisible(true);
        m_CharacterArt = m_CharacterArts[m_CurrentCharacterIndex];
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
    levelObj->m_Transform.translation = {-710, 390};
    m_Root.AddChild(levelObj);
    
    // User Name
    m_UserName = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        24,
        "Doctor",
        Util::Color(255, 255, 255)
    );
    auto nameObj = std::make_shared<Util::GameObject>(m_UserName, 10);
    nameObj->m_Transform.translation = {-710, 340};
    m_Root.AddChild(nameObj);
    
    // User ID
    m_UserID = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        18,
        "ID: 12345678",
        Util::Color(200, 200, 200)
    );
    auto idObj = std::make_shared<Util::GameObject>(m_UserID, 10);
    idObj->m_Transform.translation = {-710, 310};
    m_Root.AddChild(idObj);
    
    // Dialog text (bottom-left)
    m_DialogText = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        "博士，準備好作戰了嗎？",
        Util::Color(255, 255, 255)
    );
    auto dialogObj = std::make_shared<Util::GameObject>(m_DialogText, 10);
    dialogObj->m_Transform.translation = {-610, -370};
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
    moneyObj->m_Transform.translation = {510, 400};
    m_Root.AddChild(moneyObj);
    
    // Jasper
    m_Jasper = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        "💎 1000",
        Util::Color(255, 255, 255)
    );
    auto jasperObj = std::make_shared<Util::GameObject>(m_Jasper, 10);
    jasperObj->m_Transform.translation = {630, 400};
    m_Root.AddChild(jasperObj);
    
    // Stone
    m_Stone = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        "🔷 1000",
        Util::Color(255, 255, 255)
    );
    auto stoneObj = std::make_shared<Util::GameObject>(m_Stone, 10);
    stoneObj->m_Transform.translation = {750, 400};
    m_Root.AddChild(stoneObj);
    
    // Sanity display
    m_SanityDisplay = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        24,
        "理智 125/125",
        Util::Color(100, 200, 255)
    );
    auto sanityObj = std::make_shared<Util::GameObject>(m_SanityDisplay, 10);
    sanityObj->m_Transform.translation = {610, 350};
    m_Root.AddChild(sanityObj);
    
    // Current stage info
    m_CurrentStage = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        18,
        "當前: 0-2 守衛",
        Util::Color(255, 255, 255)
    );
    auto stageObj = std::make_shared<Util::GameObject>(m_CurrentStage, 10);
    stageObj->m_Transform.translation = {610, 310};
    m_Root.AddChild(stageObj);
}

void LobbyScene::createMainButtons() {
    // Stage/Combat Button (作戰)
    m_StageButton = std::make_shared<UI::Button>(
        "終端",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        28,
        glm::vec2(610, 230),
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
        glm::vec2(510, 150),
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
        glm::vec2(710, 150),
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
        glm::vec2(510, 50),
        glm::vec2(130, 45),
        15
    );
    m_Buttons.push_back(taskButton);
    m_Root.AddChild(taskButton);
    
    auto baseButton = std::make_shared<UI::Button>(
        "基建",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        20,
        glm::vec2(660, 50),
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
        "2026/06/06 14:20",
        Util::Color(41, 36, 33)
    );
    auto timeObj = std::make_shared<Util::GameObject>(m_TimeDisplay, 10);
    timeObj->m_Transform.translation = {660, 430};
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
    
    if (!m_IsCharacterSliding) {
        if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
            startCharacterSlide(1);
        } else if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
            startCharacterSlide(-1);
        }
    }
    updateCharacterSlide(deltaTime);

    // Update buttons
    for (auto& button : m_Buttons) {
        button->update(deltaTime);
    }

    if (m_RequestStageSelect) {
        m_RequestStageSelect = false;
        auto stageSelectScene = std::make_shared<StageSelectScene>();
        Core::SceneManager::getInstance().replaceScene(stageSelectScene);
    }
}

void LobbyScene::startCharacterSlide(int direction) {
    if (m_CharacterArts.size() < 2 || direction == 0 || m_IsCharacterSliding) return;

    const int count = static_cast<int>(m_CharacterArts.size());
    int nextIndex = (m_CurrentCharacterIndex + direction + count) % count;
    if (nextIndex == m_CurrentCharacterIndex) return;

    m_NextCharacterIndex = nextIndex;
    m_CharacterSlideDirection = direction;
    m_CharacterSlideTimer = 0.0f;
    m_IsCharacterSliding = true;

    auto& incoming = m_CharacterArts[m_NextCharacterIndex];
    incoming->SetVisible(true);
    incoming->m_Transform.scale =
        m_NextCharacterIndex < static_cast<int>(m_CharacterScales.size())
            ? m_CharacterScales[m_NextCharacterIndex]
            : glm::vec2(0.4f, 0.4f);
    incoming->m_Transform.translation = {
        m_CharacterBasePos.x + CHARACTER_SLIDE_DISTANCE * static_cast<float>(direction),
        m_CharacterBasePos.y
    };
}

void LobbyScene::updateCharacterSlide(float deltaTime) {
    if (!m_IsCharacterSliding || m_NextCharacterIndex < 0) return;

    m_CharacterSlideTimer += deltaTime;
    const float t = glm::clamp(m_CharacterSlideTimer / CHARACTER_SLIDE_DURATION_MS, 0.0f, 1.0f);
    const float offset = CHARACTER_SLIDE_DISTANCE * t * static_cast<float>(m_CharacterSlideDirection);

    auto& current = m_CharacterArts[m_CurrentCharacterIndex];
    auto& incoming = m_CharacterArts[m_NextCharacterIndex];
    current->m_Transform.translation = {m_CharacterBasePos.x - offset, m_CharacterBasePos.y};
    incoming->m_Transform.translation = {
        m_CharacterBasePos.x + CHARACTER_SLIDE_DISTANCE * static_cast<float>(m_CharacterSlideDirection) - offset,
        m_CharacterBasePos.y
    };

    if (t >= 1.0f) {
        current->SetVisible(false);
        current->m_Transform.translation = m_CharacterBasePos;

        m_CurrentCharacterIndex = m_NextCharacterIndex;
        m_NextCharacterIndex = -1;
        m_IsCharacterSliding = false;
        m_CharacterSlideDirection = 0;
        m_CharacterArt = m_CharacterArts[m_CurrentCharacterIndex];

        m_CharacterArt->SetVisible(true);
        m_CharacterArt->m_Transform.translation = m_CharacterBasePos;
        m_CharacterArt->m_Transform.scale =
            m_CurrentCharacterIndex < static_cast<int>(m_CharacterScales.size())
                ? m_CharacterScales[m_CurrentCharacterIndex]
                : glm::vec2(0.4f, 0.4f);
    }
}

void LobbyScene::cleanup() {
    LOG_DEBUG("Cleaning up LobbyScene");
    m_Buttons.clear();
}

void LobbyScene::onEnter() {
    LOG_DEBUG("Entering LobbyScene");
}

void LobbyScene::onExit() {
    LOG_DEBUG("Exiting LobbyScene");
}

void LobbyScene::onStageButtonClicked() {
    LOG_INFO("Terminal button clicked - entering stage select");
    m_RequestStageSelect = true;
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
