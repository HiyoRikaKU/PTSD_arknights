#include "Arknights/Scenes/ZoneScene.hpp"

#include "Arknights/Core/SceneManager.hpp"
#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/StageSelectScene.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

namespace Arknights {

namespace {
constexpr glm::vec2 kScreenSize{1600.0f, 900.0f};
constexpr glm::vec2 kTopBackCenter{-695.0f, 395.0f};
constexpr glm::vec2 kTopBackSize{150.0f, 90.0f};
constexpr glm::vec2 kTopHomeCenter{-410.0f, 395.0f};
constexpr glm::vec2 kTopHomeSize{300.0f, 90.0f};
constexpr glm::vec2 kEvilTimeCenter{230.0f, 185.0f};
constexpr glm::vec2 kEvilTimeSize{540.0f, 250.0f};
}

ZoneScene::ZoneScene() {
}

void ZoneScene::init() {
    LOG_DEBUG("Initializing ZoneScene");

    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/zone/next.png"),
        0
    );
    m_Background->m_Transform.scale = kScreenSize / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);

    const std::string hitImagePath = std::string(RESOURCE_DIR) + "/UI/UI_component/back.png";

    m_BackButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 5.0f);
    m_BackButton->setAlpha(0.0f);
    m_BackButton->setHitBox(kTopBackCenter, kTopBackSize);
    m_BackButton->setHoverScale(1.0f);
    m_BackButton->setClickScale(1.0f);
    m_BackButton->setOnClick([this]() { onBackClicked(); });
    m_Buttons.push_back(m_BackButton);
    m_Root.AddChild(m_BackButton);

    m_HomeButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 5.0f);
    m_HomeButton->setAlpha(0.0f);
    m_HomeButton->setHitBox(kTopHomeCenter, kTopHomeSize);
    m_HomeButton->setHoverScale(1.0f);
    m_HomeButton->setClickScale(1.0f);
    m_HomeButton->setOnClick([this]() { onHomeClicked(); });
    m_Buttons.push_back(m_HomeButton);
    m_Root.AddChild(m_HomeButton);

    m_EvilTimeButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 5.0f);
    m_EvilTimeButton->setAlpha(0.0f);
    m_EvilTimeButton->setHitBox(kEvilTimeCenter, kEvilTimeSize);
    m_EvilTimeButton->setHoverScale(1.0f);
    m_EvilTimeButton->setClickScale(1.0f);
    m_EvilTimeButton->setOnClick([this]() { onEvilTimeClicked(); });
    m_Buttons.push_back(m_EvilTimeButton);
    m_Root.AddChild(m_EvilTimeButton);

    m_Initialized = true;
}

void ZoneScene::update(float deltaTime) {
    for (auto& button : m_Buttons) {
        button->update(deltaTime);
    }

    if (m_RequestStageSelect) {
        m_RequestStageSelect = false;
        Core::SceneManager::getInstance().replaceScene(std::make_shared<StageSelectScene>());
        return;
    }

    if (m_RequestLobby) {
        m_RequestLobby = false;
        Core::SceneManager::getInstance().replaceScene(std::make_shared<LobbyScene>());
    }
}

void ZoneScene::cleanup() {
    LOG_DEBUG("Cleaning up ZoneScene");
    m_Buttons.clear();
    m_Background.reset();
    m_BackButton.reset();
    m_HomeButton.reset();
    m_EvilTimeButton.reset();
}

void ZoneScene::onEnter() {
    LOG_DEBUG("Entering ZoneScene");
}

void ZoneScene::onExit() {
    LOG_DEBUG("Exiting ZoneScene");
}

void ZoneScene::onBackClicked() {
    m_RequestLobby = true;
}

void ZoneScene::onHomeClicked() {
    m_RequestLobby = true;
}

void ZoneScene::onEvilTimeClicked() {
    m_RequestStageSelect = true;
}

} // namespace Arknights
