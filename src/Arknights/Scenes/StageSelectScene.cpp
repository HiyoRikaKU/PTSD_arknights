#include "Arknights/Scenes/StageSelectScene.hpp"

#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/LoadingScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

namespace Arknights {

StageSelectScene::StageSelectScene() {
}

void StageSelectScene::init() {
    LOG_DEBUG("Initializing StageSelectScene");

    m_SelectBackground = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/temp_0-2_1.jpg"),
        0
    );
    m_SelectBackground->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_SelectBackground->GetScaledSize();
    m_Root.AddChild(m_SelectBackground);

    m_ConfirmBackground = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/temp_0-2_2.jpg"),
        1
    );
    m_ConfirmBackground->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_ConfirmBackground->GetScaledSize();
    m_ConfirmBackground->SetVisible(false);
    m_Root.AddChild(m_ConfirmBackground);

    m_Stage02Button = std::make_shared<UI::Button>(
        "0-2",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        34,
        glm::vec2(60, -80),
        glm::vec2(180, 80),
        15
    );
    m_Stage02Button->setOnClick([this]() { onStage02Clicked(); });
    m_Buttons.push_back(m_Stage02Button);
    m_Root.AddChild(m_Stage02Button);

    m_StartActionButton = std::make_shared<UI::Button>(
        "開始行動",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        34,
        glm::vec2(670, -450),
        glm::vec2(250, 80),
        15
    );
    m_StartActionButton->setOnClick([this]() { onStartActionClicked(); });
    m_StartActionButton->SetVisible(false);
    m_Buttons.push_back(m_StartActionButton);
    m_Root.AddChild(m_StartActionButton);

    m_BackButton = std::make_shared<UI::Button>(
        "返回",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        24,
        glm::vec2(-660, -390),
        glm::vec2(140, 50),
        15
    );
    m_BackButton->setOnClick([this]() { onBackClicked(); });
    m_Buttons.push_back(m_BackButton);
    m_Root.AddChild(m_BackButton);

    m_Initialized = true;
}

void StageSelectScene::update(float deltaTime) {
    for (auto& button : m_Buttons) {
        button->update(deltaTime);
    }

    if (m_RequestStartOperation) {
        m_RequestStartOperation = false;
        auto loadingScene = std::make_shared<LoadingScene>("0-2");
        Core::SceneManager::getInstance().replaceScene(loadingScene);
        return;
    }

    if (m_RequestBack) {
        m_RequestBack = false;
        auto lobbyScene = std::make_shared<LobbyScene>();
        Core::SceneManager::getInstance().replaceScene(lobbyScene);
    }
}

void StageSelectScene::cleanup() {
    LOG_DEBUG("Cleaning up StageSelectScene");
    m_Buttons.clear();
    m_SelectBackground.reset();
    m_ConfirmBackground.reset();
    m_Stage02Button.reset();
    m_StartActionButton.reset();
    m_BackButton.reset();
}

void StageSelectScene::onEnter() {
    LOG_DEBUG("Entering StageSelectScene");
}

void StageSelectScene::onExit() {
    LOG_DEBUG("Exiting StageSelectScene");
}

void StageSelectScene::onStage02Clicked() {
    if (m_SelectBackground) m_SelectBackground->SetVisible(false);
    if (m_ConfirmBackground) m_ConfirmBackground->SetVisible(true);
    if (m_Stage02Button) m_Stage02Button->SetVisible(false);
    if (m_StartActionButton) m_StartActionButton->SetVisible(true);
}

void StageSelectScene::onStartActionClicked() {
    m_RequestStartOperation = true;
}

void StageSelectScene::onBackClicked() {
    m_RequestBack = true;
}

} // namespace Arknights
