#include "Arknights/Scenes/StageSelectScene.hpp"

#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/LoadingScene.hpp"
#include "Arknights/Scenes/ZoneScene.hpp"
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
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/selection_0-4_1.jpg"),
        0
    );
    m_SelectBackground->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_SelectBackground->GetScaledSize();
    m_Root.AddChild(m_SelectBackground);

    m_ConfirmBackground = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/selection_0-4_2.jpg"),
        1
    );
    m_ConfirmBackground->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_ConfirmBackground->GetScaledSize();
    m_ConfirmBackground->SetVisible(false);
    m_Root.AddChild(m_ConfirmBackground);

    const auto& stages = Map::StageRepository::getAllStages();
    auto getStageDisplayName = [&stages](const std::string& stageId) {
        for (const auto& stage : stages) {
            if (stage.id == stageId) {
                return stage.id;
            }
        }
        return stageId;
    };

    // Keep stage select explicit on temp_0-2_1.jpg page.
    auto addStageButton = [this, &getStageDisplayName](const std::string& stageId, const glm::vec2& position) {
        auto stageButton = std::make_shared<UI::Button>(
            getStageDisplayName(stageId),
            std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
            30,
            position,
            glm::vec2(360, 80),
            15
        );
        stageButton->setOnClick([this, stageId]() { onStageClicked(stageId); });
        m_StageButtons.push_back(stageButton);
        m_Buttons.push_back(stageButton);
        m_Root.AddChild(stageButton);
    };

    addStageButton("0-2", glm::vec2(-600.0f, -80.0f));
    addStageButton("0-3", glm::vec2(70.0f, 60.0f));
    addStageButton("0-4", glm::vec2(700.0f, 170.0f));

    m_SelectedStageId = "0-2";

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

    m_BackNavigation = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_component/back.png"),
        15
    );
    m_BackNavigation->m_Transform.scale =
        glm::vec2(1600.0f, 900.0f) / m_BackNavigation->GetScaledSize();
    m_Root.AddChild(m_BackNavigation);

    const std::string hitImagePath = std::string(RESOURCE_DIR) + "/UI/UI_component/back.png";

    m_BackButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 16.0f);
    m_BackButton->setAlpha(0.0f);
    m_BackButton->setHitBox(glm::vec2(-695.0f, 395.0f), glm::vec2(150.0f, 90.0f));
    m_BackButton->setHoverScale(1.0f);
    m_BackButton->setClickScale(1.0f);
    m_BackButton->setOnClick([this]() { onBackClicked(); });
    m_Buttons.push_back(m_BackButton);
    m_Root.AddChild(m_BackButton);

    m_HomeButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 16.0f);
    m_HomeButton->setAlpha(0.0f);
    m_HomeButton->setHitBox(glm::vec2(-410.0f, 395.0f), glm::vec2(300.0f, 90.0f));
    m_HomeButton->setHoverScale(1.0f);
    m_HomeButton->setClickScale(1.0f);
    m_HomeButton->setOnClick([this]() { onHomeClicked(); });
    m_Buttons.push_back(m_HomeButton);
    m_Root.AddChild(m_HomeButton);

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
        auto zoneScene = std::make_shared<ZoneScene>();
        Core::SceneManager::getInstance().replaceScene(zoneScene);
        return;
    }

    if (m_RequestHome) {
        m_RequestHome = false;
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
    m_BackNavigation.reset();
    m_BackButton.reset();
    m_HomeButton.reset();
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

void StageSelectScene::onHomeClicked() {
    m_RequestHome = true;
}

} // namespace Arknights
