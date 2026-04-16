#include "Arknights/Scenes/LoginScene.hpp"
#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

namespace Arknights {

LoginScene::LoginScene() {
}

void LoginScene::init() {
    LOG_DEBUG("Initializing LoginScene");

    createFlowUI();
    m_LoginBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/login.mp3");

    enterPhase(Phase::SPLASH1_HOLD);
    m_Initialized = true;
}

std::shared_ptr<Util::GameObject> LoginScene::createFullScreenImage(const std::string& path, float zIndex) {
    auto image = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(path),
        zIndex
    );
    image->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / image->GetScaledSize();
    image->SetVisible(false);
    m_Root.AddChild(image);
    m_AllImages.push_back(image);
    return image;
}

void LoginScene::createFlowUI() {
    m_Splash1 = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/splash_screen1.jpg", 0);
    m_Splash2 = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/splash_screen2.jpg", 1);
    m_Splash3 = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/splash_screen3.jpg", 2);
    m_Login1 = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/login_loading/LoginScene1.png", 3);
    m_Login2 = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/login_loading/LoginScene2.png", 4);
    m_Login3 = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/login_loading/LoginScene3.png", 5);
    m_LoginPage = createFullScreenImage(std::string(RESOURCE_DIR) + "/login_page.jpg", 6);
    m_MainLoading = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/loadingScene/main_loading.png", 7);
    m_AvgLoading = createFullScreenImage(std::string(RESOURCE_DIR) + "/UI/loadingScene/avg_0_3.png", 8);

    hideAllImages();
}

void LoginScene::hideAllImages() {
    for (const auto& image : m_AllImages) {
        if (image) image->SetVisible(false);
    }
}

void LoginScene::enterPhase(Phase nextPhase) {
    m_Phase = nextPhase;
    m_PhaseTimerMs = 0.0f;

    hideAllImages();

    switch (m_Phase) {
        case Phase::SPLASH1_HOLD:
            m_Splash1->SetVisible(true);
            break;
        case Phase::SPLASH2_HOLD:
            m_Splash2->SetVisible(true);
            break;
        case Phase::SPLASH3_HOLD:
            m_Splash3->SetVisible(true);
            break;
        case Phase::LOGIN_FAST_CYCLE:
            m_Login1->SetVisible(true);
            break;
        case Phase::LOGIN_IDLE_WAIT_CLICK:
            m_LoginPage->SetVisible(true);
            if (!m_BgmStarted && m_LoginBGM) {
                m_LoginBGM->Play();
                m_BgmStarted = true;
            }
            break;
        case Phase::MAIN_LOADING:
            m_MainLoading->SetVisible(true);
            break;
        case Phase::AVG_LOADING:
            m_AvgLoading->SetVisible(true);
            break;
    }
}

void LoginScene::preloadStageResources() {
    if (m_StagePreloaded) return;

    LOG_INFO("Preloading stage resources during splash");
    auto prepared = std::make_shared<GameScene>();
    prepared->init();
    Core::SceneManager::getInstance().setPreparedGameScene(prepared);
    m_StagePreloaded = true;
}

void LoginScene::update(float deltaTime) {
    if (!m_Initialized) return;

    // Stabilize splash/login pacing under occasional frame spikes.
    if (deltaTime > 50.0f) {
        deltaTime = 50.0f;
    }
    m_PhaseTimerMs += deltaTime;

    if (!m_StagePreloaded &&
        (m_Phase == Phase::SPLASH1_HOLD || m_Phase == Phase::SPLASH2_HOLD || m_Phase == Phase::SPLASH3_HOLD) &&
        m_PhaseTimerMs >= 50.0f) {
        preloadStageResources();
    }

    switch (m_Phase) {
        case Phase::SPLASH1_HOLD:
            if (m_PhaseTimerMs >= SPLASH_HOLD_MS) enterPhase(Phase::SPLASH2_HOLD);
            break;
        case Phase::SPLASH2_HOLD:
            if (m_PhaseTimerMs >= SPLASH_HOLD_MS) enterPhase(Phase::SPLASH3_HOLD);
            break;
        case Phase::SPLASH3_HOLD:
            if (m_PhaseTimerMs >= SPLASH_HOLD_MS) enterPhase(Phase::LOGIN_FAST_CYCLE);
            break;
        case Phase::LOGIN_FAST_CYCLE: {
            const int index = static_cast<int>(m_PhaseTimerMs / LOGIN_FAST_FRAME_MS) % 3;
            m_Login1->SetVisible(index == 0);
            m_Login2->SetVisible(index == 1);
            m_Login3->SetVisible(index == 2);
            if (m_PhaseTimerMs >= LOGIN_FAST_TOTAL_MS) enterPhase(Phase::LOGIN_IDLE_WAIT_CLICK);
            break;
        }
        case Phase::LOGIN_IDLE_WAIT_CLICK:
            if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) enterPhase(Phase::MAIN_LOADING);
            break;
        case Phase::MAIN_LOADING:
            if (m_PhaseTimerMs >= MAIN_LOADING_MS) enterPhase(Phase::AVG_LOADING);
            break;
        case Phase::AVG_LOADING:
            if (m_PhaseTimerMs >= AVG_LOADING_MS && !m_TransitionQueued) {
                m_TransitionQueued = true;
                transitToLobby();
            }
            break;
    }
}

void LoginScene::cleanup() {
    LOG_DEBUG("Cleaning up LoginScene");
    m_LoginBGM.reset();
    m_AllImages.clear();
    m_Splash1.reset();
    m_Splash2.reset();
    m_Splash3.reset();
    m_Login1.reset();
    m_Login2.reset();
    m_Login3.reset();
    m_LoginPage.reset();
    m_MainLoading.reset();
    m_AvgLoading.reset();
}

void LoginScene::onEnter() {
    LOG_DEBUG("Entering LoginScene");
}

void LoginScene::onExit() {
    LOG_DEBUG("Exiting LoginScene");
    if (m_LoginBGM) {
        m_LoginBGM->FadeOut(400);
    }
}

void LoginScene::transitToLobby() {
    LOG_INFO("Login flow complete - transitioning to lobby");
    auto lobbyScene = std::make_shared<LobbyScene>();
    Core::SceneManager::getInstance().replaceScene(lobbyScene);
}

} // namespace Arknights
