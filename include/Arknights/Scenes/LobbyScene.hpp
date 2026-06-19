#ifndef ARKNIGHTS_LOBBYSCENE_HPP
#define ARKNIGHTS_LOBBYSCENE_HPP

#include <memory>
#include <vector>
#include <string>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/UI/Button.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"
#include "Util/Text.hpp"

namespace Arknights {

/**
 * @brief Main lobby scene - Shows home screen with all main menu options
 * 
 * This scene is shown after login.
 * Players can access: Stage, Team, Operator, Task, Base
 */
class LobbyScene : public Scene {
public:
    LobbyScene();
    ~LobbyScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;
    
    void onEnter() override;
    void onExit() override;

private:
    void createBackground();
    void createCharacterSlider();
    void createLobbyUI();
    void createTerminalButton();
    void startCharacterSlide(int direction);
    void updateCharacterSlide(float deltaTime);
    void updateTerminalButton(float deltaTime);
    void updateCharacterVoiceButton(float deltaTime);
    void onTerminalClicked();
    void onHazeClicked();
    bool isPointInHazeArt(const glm::vec2& point) const;

private:
    // Background
    std::shared_ptr<Util::GameObject> m_Background;

    // Character art (unchanged)
    std::shared_ptr<Util::GameObject> m_CharacterArt;
    std::vector<std::shared_ptr<Util::GameObject>> m_CharacterArts;
    std::vector<std::string> m_CharacterArtPaths;
    std::vector<glm::vec2> m_CharacterScales;

    // Static lobby UI overlay (lobbyUI.png)
    std::shared_ptr<Util::GameObject> m_LobbyUI;

    // Terminal button (terminal.png) — only interactive element
    std::shared_ptr<UI::Button> m_TerminalButton;

    std::unique_ptr<Util::BGM> m_LobbyBGM;
    std::unique_ptr<Util::SFX> m_DuctorSFX;
    std::unique_ptr<Util::SFX> m_HazeVoiceSFX;
    std::unique_ptr<Util::SFX> m_HazeVoice2SFX;
    bool m_WasMousePressedOnHaze = false;

    // Fade-out state for terminal button
    enum class FadeState { IDLE, WAITING };
    FadeState m_FadeState = FadeState::IDLE;
    float m_FadeTimer = 0.0f;
    static constexpr float FADE_DURATION_MS = 350.0f;
    // Scale-based fade: we shrink terminal button to 0 to simulate fade
    glm::vec2 m_TerminalOriginalScale = {1.0f, 1.0f};
    // State
    bool m_RequestStageSelect = false;
    int m_CurrentCharacterIndex = 0;
    int m_NextCharacterIndex = -1;
    int m_CharacterSlideDirection = 0;
    float m_CharacterSlideTimer = 0.0f;
    bool m_IsCharacterSliding = false;
    const glm::vec2 m_CharacterBasePos = {-300.0f, -50.0f};
    static constexpr float CHARACTER_SLIDE_DURATION_MS = 320.0f;
    static constexpr float CHARACTER_SLIDE_DISTANCE = 520.0f;
};

} // namespace Arknights

#endif
