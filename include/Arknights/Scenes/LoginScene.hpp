#ifndef ARKNIGHTS_LOGINSCENE_HPP
#define ARKNIGHTS_LOGINSCENE_HPP

#include <memory>
#include <string>
#include <vector>

#include "Arknights/Scenes/Scene.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/BGM.hpp"

namespace Arknights {

/**
 * @brief Login scene with splash/login/loading flow
 * 
 * Flow:
 * splash1 -> splash2 -> splash3 -> LoginScene1~3 -> login page
 * -> click anywhere -> main_loading -> avg_0_3 -> LobbyScene
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
        SPLASH2_HOLD,
        SPLASH3_HOLD,
        LOGIN_FAST_CYCLE,
        LOGIN_IDLE_WAIT_CLICK,
        MAIN_LOADING,
        AVG_LOADING
    };

    void createFlowUI();
    std::shared_ptr<Util::GameObject> createFullScreenImage(const std::string& path, float zIndex);
    void hideAllImages();
    void enterPhase(Phase nextPhase);
    void transitToLobby();

private:
    std::shared_ptr<Util::GameObject> m_Splash1;
    std::shared_ptr<Util::GameObject> m_Splash2;
    std::shared_ptr<Util::GameObject> m_Splash3;
    std::shared_ptr<Util::GameObject> m_Login1;
    std::shared_ptr<Util::GameObject> m_Login2;
    std::shared_ptr<Util::GameObject> m_Login3;
    std::shared_ptr<Util::GameObject> m_LoginPage;
    std::shared_ptr<Util::GameObject> m_MainLoading;
    std::shared_ptr<Util::GameObject> m_AvgLoading;
    std::vector<std::shared_ptr<Util::GameObject>> m_AllImages;

    // BGM
    std::unique_ptr<Util::BGM> m_LoginBGM;

    Phase m_Phase = Phase::SPLASH1_HOLD;
    float m_PhaseTimerMs = 0.0f;
    bool m_BgmStarted = false;
    bool m_TransitionQueued = false;

    static constexpr float SPLASH_HOLD_MS = 700.0f;
    static constexpr float LOGIN_FAST_TOTAL_MS = 1500.0f;
    static constexpr float LOGIN_FAST_FRAME_MS = 250.0f;
    static constexpr float MAIN_LOADING_MS = 900.0f;
    static constexpr float AVG_LOADING_MS = 900.0f;
};

} // namespace Arknights

#endif // ARKNIGHTS_LOGINSCENE_HPP
