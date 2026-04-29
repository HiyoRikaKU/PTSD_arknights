#include "Arknights/Scenes/StageSelectScene.hpp"

#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/LoadingScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Arknights/Map/StageRepository.hpp"
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

    const auto& stages = Map::StageRepository::getAllStages();
    if (!stages.empty()) {
        m_SelectedStageId = stages.front().id;
    }

    constexpr float buttonStartY = -80.0f;
    constexpr float buttonStepY = -100.0f;
    constexpr float buttonX = 60.0f;
    for (std::size_t i = 0; i < stages.size(); ++i) {
        const auto& stage = stages[i];
        auto stageButton = std::make_shared<UI::Button>(
            stage.id + " " + stage.name,
            std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
            30,
            glm::vec2(buttonX, buttonStartY + buttonStepY * static_cast<float>(i)),
            glm::vec2(360, 80),
            15
        );
        stageButton->setOnClick([this, stageId = stage.id]() { onStageClicked(stageId); });
        m_StageButtons.push_back(stageButton);
        m_Buttons.push_back(stageButton);
        m_Root.AddChild(stageButton);
    }

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
        glm::vec2(-660, 350),
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
        auto loadingScene = std::make_shared<LoadingScene>(m_SelectedStageId);
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
    m_StageButtons.clear();
    m_SelectBackground.reset();
    m_ConfirmBackground.reset();
    m_StartActionButton.reset();
    m_BackButton.reset();
    m_SelectedStageId.clear();
}

void StageSelectScene::onEnter() {
    LOG_DEBUG("Entering StageSelectScene");
}

void StageSelectScene::onExit() {
    LOG_DEBUG("Exiting StageSelectScene");
}

void StageSelectScene::onStageClicked(const std::string& stageId) {
    m_SelectedStageId = stageId;
    if (m_SelectBackground) m_SelectBackground->SetVisible(false);
    if (m_ConfirmBackground) m_ConfirmBackground->SetVisible(true);
    for (const auto& button : m_StageButtons) {
        if (button) button->SetVisible(false);
    }
    if (m_StartActionButton) m_StartActionButton->SetVisible(true);
}

void StageSelectScene::onStartActionClicked() {
    if (m_SelectedStageId.empty()) {
        return;
    }
    m_RequestStartOperation = true;
}

void StageSelectScene::onBackClicked() {
    m_RequestBack = true;
}

} // namespace Arknights
