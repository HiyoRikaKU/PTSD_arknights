#ifndef ARKNIGHTS_LOGINSCENE_HPP
#define ARKNIGHTS_LOGINSCENE_HPP

#include <memory>
#include <string>
#include <vector>

#include "Arknights/Scenes/Scene.hpp"
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/BGM.hpp"

namespace Arknights {

/**
 * @brief Login scene with splash/login/loading flow
 * 
 * Flow:
 * splash1 -> splash2 (fade) -> splash3 (fade) -> LoginScene1~3 (no loop)
 * -> login_animation frame sequence -> LobbyScene
 */
class LoginScene : public Scene {
public:
    LoginScene();
    ~LoginScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;
    
    void onEnter() override;
    void onExit() override;

private:
    enum class Phase {
        SPLASH1_HOLD,
        SPLASH12_FADE,
        SPLASH2_HOLD,
        SPLASH23_FADE,
        SPLASH3_HOLD,
        LOGIN1_SHOW,
        LOGIN2_SHOW,
        LOGIN3_SHOW,
        LOGIN_ANIMATION
    };

    void createFlowUI();
    std::shared_ptr<Util::GameObject> createFullScreenImage(const std::string& path, float zIndex);
    void hideAllImages();
    void releaseImage(std::shared_ptr<Util::GameObject>& image);
    void updateCrossfade(const std::shared_ptr<Util::GameObject>& fromImage,
                         const std::shared_ptr<Util::GameObject>& toImage,
                         float durationMs);
    void preloadStageResources();
    void enterPhase(Phase nextPhase);
    void transitToLobby();

private:
    std::shared_ptr<Util::GameObject> m_Splash1;
    std::shared_ptr<Util::GameObject> m_Splash2;
    std::shared_ptr<Util::GameObject> m_Splash3;
    std::shared_ptr<Util::GameObject> m_Login1;
    std::shared_ptr<Util::GameObject> m_Login2;
    std::shared_ptr<Util::GameObject> m_Login3;
    std::shared_ptr<Util::Animation> m_LoginAnimation;
    std::shared_ptr<Util::GameObject> m_LoginAnimationObject;
    std::vector<std::shared_ptr<Util::GameObject>> m_AllImages;

    // BGM
    std::unique_ptr<Util::BGM> m_LoginBGM;

    Phase m_Phase = Phase::SPLASH1_HOLD;
    float m_PhaseTimerMs = 0.0f;
    bool m_BgmStarted = false;
    bool m_TransitionQueued = false;
    bool m_StagePreloaded = false;

    static constexpr float SPLASH_HOLD_MS = 2600.0f;
    static constexpr float SPLASH_FADE_MS = 40.0f;
    static constexpr float LOGIN_IMAGE_SHOW_MS = 2600.0f;
    static constexpr int LOGIN_ANIMATION_INTERVAL_MS = 67;
    static constexpr float FADE_DITHER_TICK_MS = 40.0f;
};

} // namespace Arknights

#endif // ARKNIGHTS_LOGINSCENE_HPP
