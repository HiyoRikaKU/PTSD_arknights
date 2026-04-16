#include "Arknights/Scenes/LoadingScene.hpp"
#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
namespace Arknights {

LoadingScene::LoadingScene(std::string stageId)
    : m_StageId(std::move(stageId)) {
}

void LoadingScene::init() {
    LOG_DEBUG("Initializing LoadingScene");

    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/blackSquare.jpg"),
        0
    );
    m_Background->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);

    m_LoadingImageA = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/operation/UI_0-2_1.jpg"),
        1
    );
    m_LoadingImageA->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoadingImageA->GetScaledSize();
    m_Root.AddChild(m_LoadingImageA);

    m_LoadingImageB = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/operation/UI_0-2_2.jpg"),
        2
    );
    m_LoadingImageB->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoadingImageB->GetScaledSize();
    m_LoadingImageB->SetVisible(false);
    m_Root.AddChild(m_LoadingImageB);

    const std::string loadingLabel = "Loading " + m_StageId + "...";
    auto text = std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf",
        38,
        loadingLabel,
        Util::Color(255, 255, 255)
    );
    m_LoadingText = std::make_shared<Util::GameObject>(text, 20);
    m_LoadingText->m_Transform.translation = {560, -390};
    m_Root.AddChild(m_LoadingText);

    m_Initialized = true;
}

void LoadingScene::update(float deltaTime) {
    if (deltaTime > 50.0f) {
        deltaTime = 50.0f;
    }
    m_Timer += deltaTime;
    updateLoadingVisual();

    // Ensure both loading images are shown at least once before heavy init starts.
    if (!m_HasStartedLoading && m_Timer >= IMAGE_DURATION_MS * 2.0f) {
        startLoadingGameScene();
    }

    if (m_AssetsLoaded && !m_HasTransitioned && m_Timer >= IMAGE_DURATION_MS * 2.0f) {
        m_HasTransitioned = true;
        Core::SceneManager::getInstance().replaceScene(m_PreparedGameScene);
    }
}

void LoadingScene::cleanup() {
    LOG_DEBUG("Cleaning up LoadingScene");
    m_Background.reset();
    m_LoadingImageA.reset();
    m_LoadingImageB.reset();
    m_LoadingText.reset();
    m_PreparedGameScene.reset();
}

void LoadingScene::onEnter() {
    LOG_DEBUG("Entering LoadingScene");
}

void LoadingScene::onExit() {
    LOG_DEBUG("Exiting LoadingScene");
}

void LoadingScene::startLoadingGameScene() {
    m_HasStartedLoading = true;
    LOG_INFO("LoadingScene: preparing stage {}", m_StageId);

    m_PreparedGameScene = std::make_shared<GameScene>();
    m_PreparedGameScene->init();
    m_AssetsLoaded = true;
}

void LoadingScene::updateLoadingVisual() {
    if (!m_LoadingImageA || !m_LoadingImageB) return;

    const bool showA = m_Timer < IMAGE_DURATION_MS;

    m_LoadingImageA->SetVisible(showA);
    m_LoadingImageB->SetVisible(!showA);
}

} // namespace Arknights
