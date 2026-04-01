#ifndef ARKNIGHTS_LOBBYSCENE_HPP
#define ARKNIGHTS_LOBBYSCENE_HPP

#include <memory>
#include <vector>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/UI/Button.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/BGM.hpp"

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
    void createUserInfo();
    void createResourceDisplay();
    void createMainButtons();
    void createTimeDisplay();
    
    void onStageButtonClicked();
    void onOperatorButtonClicked();
    void onTeamButtonClicked();

private:
    // Background
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_CharacterArt;
    
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
    
    // BGM
    std::unique_ptr<Util::BGM> m_LobbyBGM;
    
    // State
    float m_TimeCounter = 0.0f;
};

} // namespace Arknights

#endif
