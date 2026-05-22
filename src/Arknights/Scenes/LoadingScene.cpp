#include "Arknights/Scenes/LoadingScene.hpp"
#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Image.hpp"
namespace Arknights {

LoadingScene::LoadingScene(std::string stageId)
    : m_StageId(std::move(stageId)) {
}

void LoadingScene::init() {
    LOG_DEBUG("Initializing LoadingScene");
    const std::string operationImagePrefix =
        (m_StageId == "0-4") ? "UI_0_4_" :
        (m_StageId == "0-3") ? "UI_0-3_" : 
        (m_StageId == "0-2") ? "UI_0-2_" : "";

    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/blackSquare.jpg"),
        0
    );
    m_Background->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);

    m_LoadingImageA = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/operation/" + operationImagePrefix + "1.jpg"),
        1
    );
    m_LoadingImageA->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoadingImageA->GetScaledSize();
    m_Root.AddChild(m_LoadingImageA);

    m_LoadingImageB = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/operation/" + operationImagePrefix + "2.jpg"),
        2
    );
    m_LoadingImageB->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoadingImageB->GetScaledSize();
    m_LoadingImageB->SetVisible(false);
    m_Root.AddChild(m_LoadingImageB);

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

    auto prepared = Core::SceneManager::getInstance().takePreparedGameScene();
    if (prepared) {
        auto preparedGameScene = std::dynamic_pointer_cast<GameScene>(prepared);
        if (preparedGameScene && preparedGameScene->getStageId() == m_StageId) {
            m_PreparedGameScene = std::move(preparedGameScene);
        }
    }

    if (!m_PreparedGameScene) {
        m_PreparedGameScene = std::make_shared<GameScene>(m_StageId);
        m_PreparedGameScene->init();
    }
    m_AssetsLoaded = true;
}

void LoadingScene::updateLoadingVisual() {
    if (!m_LoadingImageA || !m_LoadingImageB) return;

    const bool showA = m_Timer < IMAGE_DURATION_MS;

    m_LoadingImageA->SetVisible(showA);
    m_LoadingImageB->SetVisible(!showA);
}

} // namespace Arknights
