#include "Arknights/Scenes/LoginScene.hpp"
#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

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

    std::vector<std::string> loginAnimationPaths;
    loginAnimationPaths.reserve(150 - 17 + 1);
    for (int i = 17; i <= 150; i+=2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/UI/login_animation/ezgif-frame-" << std::setfill('0') << std::setw(3) << i << ".jpg";
        loginAnimationPaths.push_back(ss.str());
    }
    m_LoginAnimation = std::make_shared<Util::Animation>(
        loginAnimationPaths, false, LOGIN_ANIMATION_INTERVAL_MS, false, 0
    );
    m_LoginAnimationObject = std::make_shared<Util::GameObject>(m_LoginAnimation, 6);
    m_LoginAnimationObject->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoginAnimationObject->GetScaledSize();
    m_LoginAnimationObject->SetVisible(false);
    m_Root.AddChild(m_LoginAnimationObject);

    hideAllImages();
}

void LoginScene::hideAllImages() {
    for (const auto& image : m_AllImages) {
        if (image) image->SetVisible(false);
    }
    if (m_LoginAnimationObject) m_LoginAnimationObject->SetVisible(false);
}

void LoginScene::releaseImage(std::shared_ptr<Util::GameObject>& image) {
    if (!image) return;
    m_AllImages.erase(
        std::remove(m_AllImages.begin(), m_AllImages.end(), image),
        m_AllImages.end()
    );
    m_Root.RemoveChild(image);
    image.reset();
}

void LoginScene::updateCrossfade(const std::shared_ptr<Util::GameObject>& fromImage,
                                 const std::shared_ptr<Util::GameObject>& toImage,
                                 float durationMs) {
    if (!fromImage || !toImage) return;
    const float t = std::clamp(m_PhaseTimerMs / durationMs, 0.0f, 1.0f);
    const float fromWeight = 1.0f - t;
    const float tick = std::fmod(m_PhaseTimerMs, FADE_DITHER_TICK_MS) / FADE_DITHER_TICK_MS;
    const bool showFrom = tick < fromWeight;
    fromImage->SetVisible(showFrom);
    toImage->SetVisible(!showFrom);
}

void LoginScene::enterPhase(Phase nextPhase) {
    m_Phase = nextPhase;
    m_PhaseTimerMs = 0.0f;

    hideAllImages();

    switch (m_Phase) {
        case Phase::SPLASH1_HOLD:
            m_Splash1->SetVisible(true);
            break;
        case Phase::SPLASH12_FADE:
            m_Splash1->SetVisible(true);
            m_Splash2->SetVisible(false);
            break;
        case Phase::SPLASH2_HOLD:
            m_Splash2->SetVisible(true);
            break;
        case Phase::SPLASH23_FADE:
            m_Splash2->SetVisible(true);
            m_Splash3->SetVisible(false);
            break;
        case Phase::SPLASH3_HOLD:
            m_Splash3->SetVisible(true);
            break;
        case Phase::LOGIN1_SHOW:
            m_Login1->SetVisible(true);
            break;
        case Phase::LOGIN2_SHOW:
            m_Login2->SetVisible(true);
            break;
        case Phase::LOGIN3_SHOW:
            m_Login3->SetVisible(true);
            break;
        case Phase::LOGIN_ANIMATION:
            if (m_LoginAnimationObject) m_LoginAnimationObject->SetVisible(true);
            if (m_LoginAnimation) {
                m_LoginAnimation->SetCurrentFrame(0);
                m_LoginAnimation->Play();
            }
            if (!m_BgmStarted && m_LoginBGM) {
                m_LoginBGM->Play();
                m_BgmStarted = true;
            }
            break;
    }
}

void LoginScene::preloadStageResources() {
    if (m_StagePreloaded) return;

    LOG_INFO("Preloading stage resources during LoginScene1");
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
        m_Phase == Phase::LOGIN1_SHOW &&
        m_PhaseTimerMs >= 120.0f) {
        preloadStageResources();
    }

    switch (m_Phase) {
        case Phase::SPLASH1_HOLD:
            if (m_PhaseTimerMs >= SPLASH_HOLD_MS) enterPhase(Phase::SPLASH12_FADE);
            break;
        case Phase::SPLASH12_FADE:
            updateCrossfade(m_Splash1, m_Splash2, SPLASH_FADE_MS);
            if (m_PhaseTimerMs >= SPLASH_FADE_MS) enterPhase(Phase::SPLASH2_HOLD);
            break;
        case Phase::SPLASH2_HOLD:
            if (m_PhaseTimerMs >= SPLASH_HOLD_MS) enterPhase(Phase::SPLASH23_FADE);
            break;
        case Phase::SPLASH23_FADE:
            updateCrossfade(m_Splash2, m_Splash3, SPLASH_FADE_MS);
            if (m_PhaseTimerMs >= SPLASH_FADE_MS) enterPhase(Phase::SPLASH3_HOLD);
            break;
        case Phase::SPLASH3_HOLD:
            if (m_PhaseTimerMs >= SPLASH_HOLD_MS) enterPhase(Phase::LOGIN1_SHOW);
            break;
        case Phase::LOGIN1_SHOW:
            if (m_PhaseTimerMs >= LOGIN_IMAGE_SHOW_MS) {
                releaseImage(m_Login1);
                enterPhase(Phase::LOGIN2_SHOW);
            }
            break;
        case Phase::LOGIN2_SHOW:
            if (m_PhaseTimerMs >= LOGIN_IMAGE_SHOW_MS) {
                releaseImage(m_Login2);
                enterPhase(Phase::LOGIN3_SHOW);
            }
            break;
        case Phase::LOGIN3_SHOW:
            if (m_PhaseTimerMs >= LOGIN_IMAGE_SHOW_MS) {
                releaseImage(m_Login3);
                enterPhase(Phase::LOGIN_ANIMATION);
            }
            break;
        case Phase::LOGIN_ANIMATION:
            if (m_LoginAnimation &&
                m_LoginAnimation->GetState() == Util::Animation::State::ENDED &&
                !m_TransitionQueued) {
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
    m_LoginAnimation.reset();
    m_LoginAnimationObject.reset();
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
