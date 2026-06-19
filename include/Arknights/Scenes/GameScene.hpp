#ifndef ARKNIGHTS_GAMESCENE_HPP
#define ARKNIGHTS_GAMESCENE_HPP

#include <memory>
#include <vector>
#include <string>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/Entity/Enemy.hpp"
#include "Arknights/Map/Operation.hpp"
#include "Arknights/Entity/Operator.hpp"
#include "Arknights/UI/Button.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"

namespace Arknights {

class GameScene : public Scene {
public:
    explicit GameScene(std::string stageId = "0-2");
    ~GameScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;
    
    void onEnter() override;
    void onExit() override;
    
    void reset();
    const std::string& getStageId() const { return m_StageId; }

private:
    void initOperation();
    void initEnemies();
    void initOperators();
    void initUI();

    void spawnEnemy(const SpawnEvent& event);
    void handleOperatorDrag(float deltaTime);
    void beginVictorySequence();
    void beginFailureSequence();
    void cleanupCharactersForResult();
    bool isAnyReturnInput() const;
    void updateHudText();
    void updateDPText();
    void updateCostLabels();
    void updateOperatorPanel(const glm::vec2& mousePos);
    std::shared_ptr<Operator> getHoveredOperator(const glm::vec2& mousePos) const;
    void updateButtons(float deltaTime);
    void togglePause();
    void toggleHub();
    void openExitConfirm();
    void closeExitConfirm();
    void returnToStageSelect();
    void playClickSFX();
    void setGameSpeed(float speedMultiplier);
    void setCheatMode(bool enabled);
    
    void TriggerMissionComplete();
    void UpdateMissionAnimation();

private:
    std::string m_StageId;

    // Operation/Map
    std::unique_ptr<Operation> m_CurrentOperation;
    
    // Enemy system
    std::unique_ptr<EnemyPool> m_EnemyPool;
    std::vector<Enemy*> m_ActiveEnemies;
    std::vector<std::string> m_EnemyAnimationPathsGopro;
    std::vector<std::string> m_EnemyAttackPathsGopro;
    std::vector<std::string> m_EnemyDiePathsGopro;
    std::vector<std::string> m_EnemyAnimationPathsBigbo;
    std::vector<std::string> m_EnemyAttackPathsBigbo;
    std::vector<std::string> m_EnemyDiePathsBigbo;
    std::vector<std::string> m_EnemyAnimationPathsTrslim;
    std::vector<std::string> m_EnemyAttackPathsTrslim;
    std::vector<std::string> m_EnemyDiePathsTrslim;
    std::vector<std::string> m_EnemyAnimationPathsYokai;
    std::vector<std::string> m_EnemyAttackPathsYokai;
    std::vector<std::string> m_EnemyDiePathsYokai;
    
    // Stats
    float m_WaveTimer = 0.0f;
    int m_SpawnedEnemies = 0;
    int m_KilledEnemies = 0;
    int m_TotalEnemies = 0;
    int m_BaseHP = 6;
    bool m_IsGameOver = false;
    bool m_IsVictory = false;
    bool m_IsPaused = false;
    bool m_IsHubOpen = false;
    bool m_IsExitConfirmOpen = false;
    bool m_IsCheatMode = false;
    float m_GameSpeedMultiplier = 1.0f;
    enum class ResultPhase {
        NONE,
        VICTORY_SLIDE,
        VICTORY_WAIT_BEFORE_WIN,
        VICTORY_SHOW_WIN,
        FAILURE_SHOW
    };
    ResultPhase m_ResultPhase = ResultPhase::NONE;
    float m_ResultTimer = 0.0f;

    // DP System
    float m_CurrentDP = 10.0f;
    float m_DPAccumulator = 0.0f;
    int m_DisplayedDP = -1;

    // Operator system
    std::vector<std::shared_ptr<Operator>> m_Operators;
    std::shared_ptr<Operator> m_DraggedOperator = nullptr;
    std::shared_ptr<ExGameObject> m_DraggedIcon = nullptr;
    std::shared_ptr<Operator> m_ChoosingDirectionOperator = nullptr;
    std::shared_ptr<ExGameObject> m_ChoosingDirectionIcon = nullptr;
    
    // UI GameObjects
    std::shared_ptr<ExGameObject> m_AmiyaIcon;
    std::shared_ptr<ExGameObject> m_ChenIcon;
    std::shared_ptr<ExGameObject> m_AngelinaIcon;
    std::shared_ptr<ExGameObject> m_RedIcon;
    std::shared_ptr<ExGameObject> m_EyjafjallaIcon;
    std::shared_ptr<ExGameObject> m_TexasIcon;
    std::shared_ptr<ExGameObject> m_UmirinIcon;

    std::shared_ptr<ExGameObject> m_EnemyCountText;
    std::shared_ptr<ExGameObject> m_BaseHPText;
    std::shared_ptr<ExGameObject> m_GameOverText;
    std::shared_ptr<ExGameObject> m_RestartText;
    std::shared_ptr<ExGameObject> m_DPText;

    std::shared_ptr<ExGameObject> m_AmiyaCostText;
    std::shared_ptr<ExGameObject> m_ChenCostText;
    std::shared_ptr<ExGameObject> m_AngelinaCostText;
    std::shared_ptr<ExGameObject> m_RedCostText;
    std::shared_ptr<ExGameObject> m_EyjafjallaCostText;
    std::shared_ptr<ExGameObject> m_TexasCostText;
    std::shared_ptr<ExGameObject> m_UmirinCostText;
    
    enum class MissionAnimState {
        IDLE,
        BAR_SLIDE_IN,
        LOGO_SLIDE_IN,
        HOLD,
        FADE_OUT,
        DONE
    };

    MissionAnimState m_MissionAnimState = MissionAnimState::IDLE;
    float            m_MissionAnimTimer = 0.0f;

    static constexpr float ANIM_BAR_SLIDE_DURATION  = 0.20f;
    static constexpr float ANIM_LOGO_SLIDE_DURATION = 0.33f;
    static constexpr float ANIM_HOLD_DURATION       = 0.90f;
    static constexpr float ANIM_FADE_DURATION       = 0.20f;

    static constexpr float BAR_OFFSCREEN_X   =  1100.0f;
    static constexpr float BAR_TARGET_X      =     0.0f;
    static constexpr float BAR_TARGET_Y      =    23.0f;
    static constexpr float LOGO_OFFSCREEN_X  = -1100.0f;
    static constexpr float LOGO_TARGET_X     =    -6.5f;
    static constexpr float LOGO_TARGET_Y     =    54.0f;

    std::shared_ptr<ExGameObject> m_MissionCompletedBar;
    std::shared_ptr<ExGameObject> m_MissionCompletedImage;
    // ────────────────────────────────────────────────────────────────

    std::shared_ptr<ExGameObject> m_YourWinImage;
    std::shared_ptr<ExGameObject> m_MissionFailedImage;
    std::shared_ptr<ExGameObject> m_OperatorPanelTitleText;
    std::shared_ptr<ExGameObject> m_OperatorPanelLine1Text;
    std::shared_ptr<ExGameObject> m_OperatorPanelLine2Text;
    std::shared_ptr<ExGameObject> m_OperatorPanelLine3Text;
    std::shared_ptr<ExGameObject> m_OperatorPanelLine4Text;
    std::shared_ptr<ExGameObject> m_GameUIOverlay;
    std::shared_ptr<ExGameObject> m_SettingsOverlayImage;
    std::shared_ptr<ExGameObject> m_ExitOverlayImage;
    std::shared_ptr<ExGameObject> m_PauseOverlayImage;
    std::shared_ptr<ExGameObject> m_PauseOverlayText;
    std::shared_ptr<ExGameObject> m_PauseHintText;
    std::shared_ptr<ExGameObject> m_HubOverlayText;
    std::shared_ptr<ExGameObject> m_CheatModeText;

    std::shared_ptr<UI::Button> m_PauseButton;
    std::shared_ptr<UI::Button> m_SpeedButton;
    std::shared_ptr<UI::Button> m_ResumeButton;
    std::shared_ptr<UI::Button> m_RetryButton;
    std::shared_ptr<UI::Button> m_ExitButton;
    std::shared_ptr<UI::Button> m_NormalModeButton;
    std::shared_ptr<UI::Button> m_CheatModeButton;
    std::shared_ptr<UI::Button> m_SettingsButton;
    std::shared_ptr<UI::Button> m_ExitCancelButton;
    std::shared_ptr<UI::Button> m_ExitConfirmButton;

    // Speed indicator text
    std::shared_ptr<ExGameObject> m_SpeedText;

    // Audio
    std::unique_ptr<Util::BGM> m_BattleBGM;
    std::unique_ptr<Util::SFX> m_ClickSFX;
    
    // Constants
    static constexpr std::size_t ENEMY_POOL_SIZE = 64;
    static constexpr int MAX_BASE_HP = 6;
};

} // namespace Arknights

#endif
