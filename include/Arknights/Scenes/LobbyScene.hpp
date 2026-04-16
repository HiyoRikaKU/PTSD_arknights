#ifndef ARKNIGHTS_LOBBYSCENE_HPP
#define ARKNIGHTS_LOBBYSCENE_HPP

#include <memory>
#include <vector>
#include <string>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/UI/Button.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
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
    void createUserInfo();
    void createResourceDisplay();
    void createMainButtons();
    void createTimeDisplay();
    void startCharacterSlide(int direction);
    void updateCharacterSlide(float deltaTime);
    
    void onStageButtonClicked();
    void onOperatorButtonClicked();
    void onTeamButtonClicked();

private:
    // Background
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_CharacterArt;
    std::vector<std::shared_ptr<Util::GameObject>> m_CharacterArts;
    std::vector<std::string> m_CharacterArtPaths;
    
    // User Info (Left side)
    std::shared_ptr<Util::GameObject> m_UserLevelBg;
    std::shared_ptr<Util::Text> m_UserLevel;
    std::shared_ptr<Util::Text> m_UserName;
    std::shared_ptr<Util::Text> m_UserID;
    std::shared_ptr<Util::Text> m_DialogText;
    
    // Resource Display (Top right)
    std::shared_ptr<Util::Text> m_TimeDisplay;
    std::shared_ptr<Util::Text> m_Money;
    std::shared_ptr<Util::Text> m_Jasper;
    std::shared_ptr<Util::Text> m_Stone;
    
    // Sanity Display
    std::shared_ptr<Util::Text> m_SanityDisplay;
    std::shared_ptr<Util::Text> m_CurrentStage;
    
    // Main Buttons
    std::vector<std::shared_ptr<UI::Button>> m_Buttons;
    std::shared_ptr<UI::Button> m_StageButton;
    std::shared_ptr<UI::Button> m_TeamButton;
    std::shared_ptr<UI::Button> m_OperatorButton;
    
    // State
    float m_TimeCounter = 0.0f;
    bool m_RequestStageSelect = false;
    int m_CurrentCharacterIndex = 0;
    int m_NextCharacterIndex = -1;
    int m_CharacterSlideDirection = 0;
    float m_CharacterSlideTimer = 0.0f;
    bool m_IsCharacterSliding = false;
    const glm::vec2 m_CharacterBasePos = {-300.0f, -50.0f};
    std::vector<glm::vec2> m_CharacterScales;
    static constexpr float CHARACTER_SLIDE_DURATION_MS = 320.0f;
    static constexpr float CHARACTER_SLIDE_DISTANCE = 520.0f;
};

} // namespace Arknights

#endif
