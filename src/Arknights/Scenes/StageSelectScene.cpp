#include "Arknights/Scenes/StageSelectScene.hpp"

#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/LoadingScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/Logger.hpp"

namespace Arknights {

StageSelectScene::StageSelectScene() {
}

void StageSelectScene::init() {
    LOG_DEBUG("Initializing StageSelectScene");

    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/zone/zone_0.png"),
        0
    );
    m_Background->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);

    auto title = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
            40,
            "關卡",
            Util::Color(255, 255, 255)
        ),
        10
    );
    title->m_Transform.translation = {0, 330};
    m_Root.AddChild(title);

    m_Stage02Button = std::make_shared<UI::Button>(
        "0-2",
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        34,
        glm::vec2(380, 80),
        glm::vec2(180, 80),
        15
    );
    m_Stage02Button->setOnClick([this]() { onStage02Clicked(); });
    m_Buttons.push_back(m_Stage02Button);
    m_Root.AddChild(m_Stage02Button);

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

    if (m_RequestStage02) {
        m_RequestStage02 = false;
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
    m_Background.reset();
    m_Stage02Button.reset();
    m_BackButton.reset();
}

void StageSelectScene::onEnter() {
    LOG_DEBUG("Entering StageSelectScene");
}

void StageSelectScene::onExit() {
    LOG_DEBUG("Exiting StageSelectScene");
}

void StageSelectScene::onStage02Clicked() {
    m_RequestStage02 = true;
}

void StageSelectScene::onBackClicked() {
    m_RequestBack = true;
}

} // namespace Arknights
