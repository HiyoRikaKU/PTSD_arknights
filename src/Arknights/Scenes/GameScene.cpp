#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Arknights/Map/StageFactory.hpp"
#include "Arknights/Scenes/ReturnLoadingScene.hpp"
#include "Arknights/Scenes/StageSelectScene.hpp"
#include "config.hpp"

#include <iomanip>
#include <sstream>
#include <utility>
#include <algorithm>

namespace Arknights {

namespace {
constexpr glm::vec2 kScreenSize{1600.0f, 900.0f};

const char* operatorName(Operator::Type type) {
    switch (type) {
        case Operator::Type::AMIYA: return "Amiya";
        case Operator::Type::CHEN: return "Chen";
        case Operator::Type::ANGELINA: return "Angelina";
        case Operator::Type::RED: return "Red";
        case Operator::Type::EYJAFJALLA: return "Eyjafjalla";
        case Operator::Type::TEXAS: return "Texas";
        case Operator::Type::UMIRIN: return "Umirin";
        default: return "Unknown";
    }
}

const char* operatorStateName(Operator::State state) {
    switch (state) {
        case Operator::State::IDLE: return "IDLE";
        case Operator::State::ATTACK: return "ATTACK";
        case Operator::State::SKILL: return "SKILL";
        case Operator::State::DYING: return "DYING";
        case Operator::State::DEAD: return "DEAD";
        default: return "UNKNOWN";
    }
}
} // namespace


GameScene::GameScene(std::string stageId)
    : m_StageId(std::move(stageId)) {
}

void GameScene::init() {
    LOG_DEBUG("Initializing GameScene");
    
    // 1. Initialize Operation
    m_CurrentOperation = Map::StageFactory::createOperationByStageId(m_StageId);
    m_CurrentOperation->getMap()->SetVisible(true);
    m_Root.AddChild(m_CurrentOperation->getMap());
    m_TotalEnemies = static_cast<int>(m_CurrentOperation->getWaveManager().getTotalSpawnCount());
    m_BaseHP = MAX_ESCAPED_ENEMIES;

    // 2. Initialize Enemy Animations
    m_EnemyAnimationPathsGopro.clear();
    m_EnemyAttackPathsGopro.clear();
    m_EnemyDiePathsGopro.clear();

    for (int i = 1; i <= 25; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsGopro.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAttackPathsGopro.push_back(ss.str());
    }
    for (int i = 1; i <= 21; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyDiePathsGopro.push_back(ss.str());
    }

    m_EnemyAnimationPathsBigbo.clear();
    m_EnemyAttackPathsBigbo.clear();
    m_EnemyDiePathsBigbo.clear();
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1001_bigbo/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsBigbo.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1001_bigbo/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAttackPathsBigbo.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1001_bigbo/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyDiePathsBigbo.push_back(ss.str());
    }

    m_EnemyAnimationPathsTrslim.clear();
    m_EnemyAttackPathsTrslim.clear();
    m_EnemyDiePathsTrslim.clear();
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_10001_trslim/Move_A_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsTrslim.push_back(ss.str());
    }
    for (int i = 1; i <= 33; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_10001_trslim/Attack_A_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAttackPathsTrslim.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_10001_trslim/Die_A_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyDiePathsTrslim.push_back(ss.str());
    }

    m_EnemyAnimationPathsYokai.clear();
    m_EnemyAttackPathsYokai.clear();
    m_EnemyDiePathsYokai.clear();
    for (int i = 1; i <= 59; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1005_yokai/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsYokai.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1005_yokai/Attack_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAttackPathsYokai.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1005_yokai/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyDiePathsYokai.push_back(ss.str());
    }

    // 3. Initialize Enemy Pool
    m_EnemyPool = std::make_unique<EnemyPool>(ENEMY_POOL_SIZE, m_EnemyAnimationPathsGopro, m_CurrentOperation->getWaypoints());
    for (const auto &handle : m_EnemyPool->getRenderHandles()) {
        m_Root.AddChild(handle);
    }

    // 4. Initialize Operators
    m_Operators.clear();
    auto amiya = std::make_shared<Amiya>();
    amiya->SetVisible(false);
    m_Operators.push_back(amiya);
    m_Root.AddChild(amiya);

    auto chen = std::make_shared<Chen>();
    chen->SetVisible(false);
    m_Operators.push_back(chen);
    m_Root.AddChild(chen);

    auto angelina = std::make_shared<Angelina>();
    angelina->SetVisible(false);
    m_Operators.push_back(angelina);
    m_Root.AddChild(angelina);

    auto red = std::make_shared<Red>();
    red->SetVisible(false);
    m_Operators.push_back(red);
    m_Root.AddChild(red);

    auto eyja = std::make_shared<Eyjafjalla>();
    eyja->SetVisible(false);
    m_Operators.push_back(eyja);
    m_Root.AddChild(eyja);

    auto texas = std::make_shared<Texas>();
    texas->SetVisible(false);
    m_Operators.push_back(texas);
    m_Root.AddChild(texas);

    auto umirin = std::make_shared<Umirin>();
    umirin->SetVisible(false);
    m_Operators.push_back(umirin);
    m_Root.AddChild(umirin);

    // 5. Initialize Icons
    m_AmiyaIcon = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Amiya_icon.png"), 5);
    m_AmiyaIcon->m_Transform.translation = {700, -350};
    m_AmiyaIcon->m_Transform.scale = {0.7f, 0.7f};
    m_Root.AddChild(m_AmiyaIcon);

    m_ChenIcon = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Chen_icon.png"), 5);
    m_ChenIcon->m_Transform.translation = {580, -350};
    m_ChenIcon->m_Transform.scale = {0.7f, 0.7f};
    m_Root.AddChild(m_ChenIcon);

    m_AngelinaIcon = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Angelina_icon.png"), 5);
    m_AngelinaIcon->m_Transform.translation = {460, -350};
    m_AngelinaIcon->m_Transform.scale = {0.7f, 0.7f};
    m_Root.AddChild(m_AngelinaIcon);

    m_RedIcon = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Projekt_Red_icon.png"), 5);
    m_RedIcon->m_Transform.translation = {340, -350};
    m_RedIcon->m_Transform.scale = {0.7f, 0.7f};
    m_Root.AddChild(m_RedIcon);

    m_EyjafjallaIcon = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Eyjafjalla_icon.png"), 5);
    m_EyjafjallaIcon->m_Transform.translation = {220, -350};
    m_EyjafjallaIcon->m_Transform.scale = {0.7f, 0.7f};
    m_Root.AddChild(m_EyjafjallaIcon);

    m_TexasIcon = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/texas.png"), 5);
    m_TexasIcon->m_Transform.translation = {100, -350};
    m_TexasIcon->m_Transform.scale = {0.7f, 0.7f};
    m_Root.AddChild(m_TexasIcon);

    m_UmirinIcon = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Yahata_Umiri_icon.png"), 5);
    m_UmirinIcon->m_Transform.translation = {-20, -350};
    m_UmirinIcon->m_Transform.scale = {0.7f, 0.7f};
    m_Root.AddChild(m_UmirinIcon);

    // 6. Initialize UI Text
    m_EnemyCountText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 36, "0 / 4", Util::Color(255, 255, 255)), 2);
    m_EnemyCountText->m_Transform.translation = {0, 400};
    m_Root.AddChild(m_EnemyCountText);

    m_GameOverText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 100, " ", Util::Color(255, 0, 0)), 10);
    m_GameOverText->m_Transform.translation = {0, 0};
    m_GameOverText->SetVisible(false);
    m_Root.AddChild(m_GameOverText);

    m_RestartText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "Press R to Restart, ESC to Exit", Util::Color(200, 200, 200)), 10);
    m_RestartText->m_Transform.translation = {0, -100};
    m_RestartText->SetVisible(false);
    m_Root.AddChild(m_RestartText);

    // ── Mission Complete：黑色橫條 ───────────────────────────────────
    m_MissionCompletedBar = std::make_shared<ExGameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/mission_completed_bar.png"),7   
    );
    m_MissionCompletedBar->m_Transform.translation = {BAR_OFFSCREEN_X, BAR_TARGET_Y};
    m_MissionCompletedBar->m_Transform.scale       = {1.0f, 1.0f};
    m_MissionCompletedBar->SetVisible(false);
    m_Root.AddChild(m_MissionCompletedBar);

    m_MissionCompletedImage = std::make_shared<ExGameObject>(
        std::make_shared<Util::Image>(
            std::string(RESOURCE_DIR) + "/UI/game_condition/mission_completed.png"),
        8   
    );
    m_MissionCompletedImage->m_Transform.translation = {LOGO_OFFSCREEN_X, LOGO_TARGET_Y};
    m_MissionCompletedImage->m_Transform.scale       = {0.9f, 0.9f};
    m_MissionCompletedImage->SetVisible(false);
    m_Root.AddChild(m_MissionCompletedImage);
    // ────────────────────────────────────────────────────────────────

    std::string winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin.jpg";
    if (m_StageId == "0-2") {
        winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin/0_2_star3.png";
    } else if (m_StageId == "0-3") {
        winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin/0_3_star3.png";
    } else if (m_StageId == "0-4") {
        winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin/0_4_star3.png";
    }

    m_YourWinImage = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(winImagePath), 11);
    m_YourWinImage->m_Transform.translation = {0.0f, 0.0f};
    m_YourWinImage->m_Transform.scale = {1.0f, 1.0f};
    m_YourWinImage->SetVisible(false);
    m_Root.AddChild(m_YourWinImage);

    m_MissionFailedImage = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/mission_failed.png"), 11);
    m_MissionFailedImage->m_Transform.translation = {0.0f, 0.0f};
    m_MissionFailedImage->m_Transform.scale = {0.9f, 0.9f};
    m_MissionFailedImage->SetVisible(false);
    m_Root.AddChild(m_MissionFailedImage);

    m_DPText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 40, "COST: 10", Util::Color(255, 255, 0)), 2);
    m_DPText->m_Transform.translation = {620, -250};
    m_Root.AddChild(m_DPText);

    // Cost labels
    m_AmiyaCostText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "10", Util::Color(255, 255, 255)), 6);
    m_AmiyaCostText->m_Transform.translation = {m_AmiyaIcon->m_Transform.translation.x, m_AmiyaIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_AmiyaCostText);

    m_ChenCostText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "18", Util::Color(255, 255, 255)), 6);
    m_ChenCostText->m_Transform.translation = {m_ChenIcon->m_Transform.translation.x, m_ChenIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_ChenCostText);

    m_AngelinaCostText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "14", Util::Color(255, 255, 255)), 6);
    m_AngelinaCostText->m_Transform.translation = {m_AngelinaIcon->m_Transform.translation.x, m_AngelinaIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_AngelinaCostText);

    m_RedCostText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "6", Util::Color(255, 255, 255)), 6);
    m_RedCostText->m_Transform.translation = {m_RedIcon->m_Transform.translation.x, m_RedIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_RedCostText);

    m_EyjafjallaCostText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "20", Util::Color(255, 255, 255)), 6);
    m_EyjafjallaCostText->m_Transform.translation = {m_EyjafjallaIcon->m_Transform.translation.x, m_EyjafjallaIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_EyjafjallaCostText);

    m_TexasCostText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "12", Util::Color(255, 255, 255)), 6);
    m_TexasCostText->m_Transform.translation = {m_TexasIcon->m_Transform.translation.x, m_TexasIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_TexasCostText);

    m_UmirinCostText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "10", Util::Color(255, 255, 255)), 6);
    m_UmirinCostText->m_Transform.translation = {m_UmirinIcon->m_Transform.translation.x, m_UmirinIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_UmirinCostText);

    // Operator property panel
    m_OperatorPanelTitleText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 28, "Operator: -", Util::Color(255, 255, 255)), 9);
    m_OperatorPanelTitleText->m_Transform.translation = {580, 260};
    m_Root.AddChild(m_OperatorPanelTitleText);

    m_OperatorPanelLine1Text = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 22, "HP: - / -", Util::Color(220, 220, 220)), 9);
    m_OperatorPanelLine1Text->m_Transform.translation = {580, 225};
    m_Root.AddChild(m_OperatorPanelLine1Text);

    m_OperatorPanelLine2Text = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 22, "ATK: -  COST: -", Util::Color(220, 220, 220)), 9);
    m_OperatorPanelLine2Text->m_Transform.translation = {580, 190};
    m_Root.AddChild(m_OperatorPanelLine2Text);

    m_OperatorPanelLine3Text = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 22, "INTERVAL: -s  BLOCK: -", Util::Color(220, 220, 220)), 9);
    m_OperatorPanelLine3Text->m_Transform.translation = {580, 155};
    m_Root.AddChild(m_OperatorPanelLine3Text);

    m_OperatorPanelLine4Text = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 22, "STATE: -", Util::Color(220, 220, 220)), 9);
    m_OperatorPanelLine4Text->m_Transform.translation = {580, 120};
    m_Root.AddChild(m_OperatorPanelLine4Text);

    m_GameUIOverlay = std::make_shared<ExGameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_component/gameUI.png"),
        10
    );
    m_GameUIOverlay->m_Transform.scale = kScreenSize / m_GameUIOverlay->GetScaledSize();
    m_Root.AddChild(m_GameUIOverlay);

    m_SettingsOverlayImage = std::make_shared<ExGameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_component/scene.png"),
        12
    );
    m_SettingsOverlayImage->m_Transform.scale = kScreenSize / m_SettingsOverlayImage->GetScaledSize();
    m_SettingsOverlayImage->SetVisible(false);
    m_Root.AddChild(m_SettingsOverlayImage);

    m_ExitOverlayImage = std::make_shared<ExGameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_component/exit.png"),
        14
    );
    m_ExitOverlayImage->m_Transform.scale = kScreenSize / m_ExitOverlayImage->GetScaledSize();
    m_ExitOverlayImage->SetVisible(false);
    m_Root.AddChild(m_ExitOverlayImage);

    m_PauseOverlayImage = std::make_shared<ExGameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_component/pause.png"),
        12
    );
    m_PauseOverlayImage->m_Transform.scale = kScreenSize / m_PauseOverlayImage->GetScaledSize();
    m_PauseOverlayImage->SetVisible(false);
    m_Root.AddChild(m_PauseOverlayImage);

    m_PauseOverlayText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 1, " ", Util::Color(255, 255, 255)), 12);
    m_PauseOverlayText->SetVisible(false);
    m_Root.AddChild(m_PauseOverlayText);

    m_PauseHintText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 1, " ", Util::Color(220, 220, 220)), 12);
    m_PauseHintText->SetVisible(false);
    m_Root.AddChild(m_PauseHintText);

    m_HubOverlayText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 70, "SETTINGS", Util::Color(255, 255, 255)), 13);
    m_HubOverlayText->m_Transform.translation = {0, 160};
    m_HubOverlayText->SetVisible(false);
    m_Root.AddChild(m_HubOverlayText);

    m_CheatModeText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(
        std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 24, "CHEAT MODE", Util::Color(255, 80, 80)), 11);
    m_CheatModeText->m_Transform.translation = {-650, 420};
    m_CheatModeText->SetVisible(false);
    m_Root.AddChild(m_CheatModeText);

    const std::string hitImagePath = std::string(RESOURCE_DIR) + "/UI/UI_component/gameUI.png";

    m_SettingsButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 15.0f);
    m_SettingsButton->setAlpha(0.0f);
    m_SettingsButton->setHitBox(glm::vec2(-700.0f, 382.0f), glm::vec2(180.0f, 130.0f));
    m_SettingsButton->setHoverScale(1.0f);
    m_SettingsButton->setClickScale(1.0f);
    m_SettingsButton->setOnClick([this]() { playClickSFX(); toggleHub(); });
    m_Root.AddChild(m_SettingsButton);

    m_PauseButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 15.0f);
    m_PauseButton->setAlpha(0.0f);
    m_PauseButton->setHitBox(glm::vec2(695.0f, 382.0f), glm::vec2(145.0f, 125.0f));
    m_PauseButton->setHoverScale(1.0f);
    m_PauseButton->setClickScale(1.0f);
    m_PauseButton->setOnClick([this]() { playClickSFX(); togglePause(); });
    m_Root.AddChild(m_PauseButton);

    m_SpeedButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 15.0f);
    m_SpeedButton->setAlpha(0.0f);
    m_SpeedButton->setHitBox(glm::vec2(565.0f, 382.0f), glm::vec2(130.0f, 125.0f));
    m_SpeedButton->setHoverScale(1.0f);
    m_SpeedButton->setClickScale(1.0f);
    m_SpeedButton->setOnClick([this]() {
        playClickSFX();
        setGameSpeed(m_GameSpeedMultiplier < 1.5f ? 2.0f : 1.0f);
    });
    m_Root.AddChild(m_SpeedButton);

    m_ResumeButton = std::make_shared<UI::Button>("Resume", std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, glm::vec2(0, -40), glm::vec2(220, 60), 13);
    m_ResumeButton->setOnClick([this]() {
        m_IsPaused = false;
    });
    m_ResumeButton->SetVisible(false);
    m_Root.AddChild(m_ResumeButton);

    m_RetryButton = std::make_shared<UI::Button>("Retry", std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, glm::vec2(0, -120), glm::vec2(220, 60), 13);
    m_RetryButton->setOnClick([this]() {
        reset();
    });
    m_RetryButton->SetVisible(false);
    m_Root.AddChild(m_RetryButton);

    m_ExitButton = std::make_shared<UI::Button>("退出行動", std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, glm::vec2(0, -170), glm::vec2(320, 70), 13);
    m_ExitButton->setOnClick([this]() { playClickSFX(); openExitConfirm(); });
    m_ExitButton->SetVisible(false);
    m_Root.AddChild(m_ExitButton);

    m_NormalModeButton = std::make_shared<UI::Button>("Normal Mode", std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, glm::vec2(-170, 10), glm::vec2(280, 70), 13);
    m_NormalModeButton->setOnClick([this]() {
        playClickSFX();
        setCheatMode(false);
    });
    m_NormalModeButton->SetVisible(false);
    m_Root.AddChild(m_NormalModeButton);

    m_CheatModeButton = std::make_shared<UI::Button>("Cheat Mode", std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, glm::vec2(170, 10), glm::vec2(280, 70), 13);
    m_CheatModeButton->setOnClick([this]() {
        playClickSFX();
        setCheatMode(true);
    });
    m_CheatModeButton->SetVisible(false);
    m_Root.AddChild(m_CheatModeButton);

    m_ExitCancelButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 16.0f);
    m_ExitCancelButton->setAlpha(0.0f);
    m_ExitCancelButton->setHitBox(glm::vec2(-400.0f, -185.0f), glm::vec2(800.0f, 100.0f));
    m_ExitCancelButton->setHoverScale(1.0f);
    m_ExitCancelButton->setClickScale(1.0f);
    m_ExitCancelButton->setOnClick([this]() { playClickSFX(); closeExitConfirm(); });
    m_ExitCancelButton->SetVisible(false);
    m_Root.AddChild(m_ExitCancelButton);

    m_ExitConfirmButton = std::make_shared<UI::Button>(hitImagePath, glm::vec2(0.0f), 16.0f);
    m_ExitConfirmButton->setAlpha(0.0f);
    m_ExitConfirmButton->setHitBox(glm::vec2(400.0f, -185.0f), glm::vec2(800.0f, 100.0f));
    m_ExitConfirmButton->setHoverScale(1.0f);
    m_ExitConfirmButton->setClickScale(1.0f);
    m_ExitConfirmButton->setOnClick([this]() { playClickSFX(); returnToStageSelect(); });
    m_ExitConfirmButton->SetVisible(false);
    m_Root.AddChild(m_ExitConfirmButton);

    // 7. Battle BGM
    m_BattleBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/battle/battle.mp3");
    m_ClickSFX = std::make_unique<Util::SFX>(std::string(RESOURCE_DIR) + "/SFX/click.mp3");
    updateHudText();
    if (m_OperatorPanelTitleText && m_OperatorPanelLine1Text &&
        m_OperatorPanelLine2Text && m_OperatorPanelLine3Text &&
        m_OperatorPanelLine4Text) {
        updateOperatorPanel(Util::Input::GetCursorPosition());
    }

    setCheatMode(false);
    m_Initialized = true;
}


void GameScene::spawnEnemy(const SpawnEvent& event) {
    if (!m_EnemyPool || !m_CurrentOperation) return;

    Enemy *enemy = m_EnemyPool->getEnemy();
    if (enemy == nullptr) return;

    if (event.enemyType == "gopro") {
        enemy->setAnimation(m_EnemyAnimationPathsGopro);
        enemy->setAttackAnimation(m_EnemyAttackPathsGopro);
        enemy->setDieAnimation(m_EnemyDiePathsGopro);
        enemy->setAttack(30.0f);
        enemy->setAttackInterval(1000.0f);
    } else if (event.enemyType == "bigbo") {
        enemy->setAnimation(m_EnemyAnimationPathsBigbo);
        enemy->setAttackAnimation(m_EnemyAttackPathsBigbo);
        enemy->setDieAnimation(m_EnemyDiePathsBigbo);
        enemy->setAttack(50.0f);
        enemy->setAttackInterval(1500.0f);
    } else if (event.enemyType == "trslim") {
        enemy->setAnimation(m_EnemyAnimationPathsTrslim);
        enemy->setAttackAnimation(m_EnemyAttackPathsTrslim);
        enemy->setDieAnimation(m_EnemyDiePathsTrslim);
        enemy->setAttack(35.0f);
        enemy->setAttackInterval(800.0f);
    } else if (event.enemyType == "yokai") {
        enemy->setAnimation(m_EnemyAnimationPathsYokai);
        enemy->setAttackAnimation(m_EnemyAttackPathsYokai);
        enemy->setDieAnimation(m_EnemyDiePathsYokai);
        enemy->setAttack(45.0f);
        enemy->setAttackInterval(900.0f);
    }

    std::vector<glm::vec2> spawnPath = m_CurrentOperation->getWaypoints();
    if (event.enemyType == "yokai") {
        if (m_StageId == "0-3" && spawnPath.size() >= 4) {
            constexpr float kRowOffset = -1.0f; // Spawn one tile higher than regular enemies.
            const glm::vec2 patrolA = spawnPath[2] + glm::vec2(kRowOffset, 0.0f);
            const glm::vec2 patrolB = spawnPath[3] + glm::vec2(kRowOffset, 0.0f);
            spawnPath = {spawnPath[0] + glm::vec2(kRowOffset, 0.0f), patrolA, patrolB};
            for (int i = 0; i < 24; ++i) {
                spawnPath.push_back((i % 2 == 0) ? patrolA : patrolB);
            }
        } else if (m_StageId == "0-4") {
            const glm::vec2 spawnPos = {1.5f, 0.5f};
            const glm::vec2 patrolA = {1.5f, 4.5f};
            const glm::vec2 patrolB = {1.5f, 5.5f};
            spawnPath = {spawnPos, patrolA, patrolB};
            for (int i = 0; i < 24; ++i) {
                spawnPath.push_back((i % 2 == 0) ? patrolA : patrolB);
            }
        }
    }

    enemy->spawn(spawnPath, event.hp, event.speed, m_CurrentOperation->getHomography());
    m_ActiveEnemies.push_back(enemy);
}

void GameScene::update(float deltaTime) {
    // Cap deltaTime to prevent huge jumps after loading or lag spikes
    if (deltaTime > 100.0f) {
        deltaTime = 16.666f;
    }

    updateButtons(deltaTime);
    if (!m_IsGameOver && Util::Input::IsKeyPressed(Util::Keycode::ESCAPE)) {
        if (m_IsExitConfirmOpen) {
            closeExitConfirm();
        } else if (m_IsHubOpen || m_IsPaused) {
            m_IsHubOpen = false;
            m_IsPaused = false;
        }
    }
    if (!m_IsGameOver && !m_IsHubOpen && !m_IsExitConfirmOpen &&
        Util::Input::IsKeyPressed(Util::Keycode::P)) {
        playClickSFX();
        togglePause();
    }
    if (!m_IsGameOver && !m_IsHubOpen && !m_IsExitConfirmOpen &&
        Util::Input::IsKeyPressed(Util::Keycode::NUM_2)) {
        playClickSFX();
        setGameSpeed(m_GameSpeedMultiplier < 1.5f ? 2.0f : 1.0f);
    }

    if (m_IsGameOver) {
        if (m_ResultPhase == ResultPhase::VICTORY_SLIDE) {
            if (m_MissionAnimState == MissionAnimState::DONE) {
                m_ResultPhase = ResultPhase::VICTORY_SHOW_WIN;
                m_MissionCompletedImage->SetVisible(false);
                if (m_MissionCompletedBar) m_MissionCompletedBar->SetVisible(false);
                m_YourWinImage->SetVisible(true);
            }
        } else if (m_ResultPhase == ResultPhase::VICTORY_WAIT_BEFORE_WIN) {
            m_ResultTimer += deltaTime;
            if (m_ResultTimer >= 2500.0f) {
                m_ResultPhase = ResultPhase::VICTORY_SHOW_WIN;
                m_MissionCompletedImage->SetVisible(false);
                m_YourWinImage->SetVisible(true);
            }
        } else if (m_ResultPhase == ResultPhase::VICTORY_SHOW_WIN || m_ResultPhase == ResultPhase::FAILURE_SHOW) {
            if (isAnyReturnInput()) {
                Core::SceneManager::getInstance().replaceScene(std::make_shared<ReturnLoadingScene>());
            }
        }
        UpdateMissionAnimation();
        updateHudText();
        updateOperatorPanel(Util::Input::GetCursorPosition());
        return;
    }

    updateOperatorPanel(Util::Input::GetCursorPosition());
    if (m_IsPaused || m_IsHubOpen || m_IsExitConfirmOpen) {
        updateHudText();
        return;
    }

    const float gameplayDeltaTime = deltaTime * m_GameSpeedMultiplier;
    m_WaveTimer += gameplayDeltaTime;

    // DP Generation
    m_DPAccumulator += gameplayDeltaTime;
    if (m_DPAccumulator >= 1000.0f) {
        m_CurrentDP += 1.0f;
        m_DPAccumulator -= 1000.0f;
        auto dpTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_DPText->GetDrawable());
        dpTextDrawable->SetText("COST: " + std::to_string(static_cast<int>(m_CurrentDP)));
    }

    SpawnEvent event;
    while (m_CurrentOperation->getWaveManager().shouldSpawn(m_WaveTimer, event)) {
        spawnEnemy(event);
    }

    // Update Enemies
    for (std::size_t i = 0; i < m_ActiveEnemies.size();) {
        Enemy *enemy = m_ActiveEnemies[i];
        enemy->update(gameplayDeltaTime);

        if (!enemy->isActive()) {
            if (enemy->reachedEnd()) {
                if (!m_IsCheatMode) {
                    m_EscapedEnemies++;
                    m_BaseHP = std::max(0, MAX_ESCAPED_ENEMIES - m_EscapedEnemies);
                }
            } else {
                m_KilledEnemies++;
            }
            m_EnemyPool->returnEnemy(enemy);
            m_ActiveEnemies[i] = m_ActiveEnemies.back();
            m_ActiveEnemies.pop_back();
            continue;
        }
        ++i;
    }

    if (m_EscapedEnemies >= MAX_ESCAPED_ENEMIES) {
        beginFailureSequence();
    } else if (m_CurrentOperation->getWaveManager().isAllSpawned() && m_ActiveEnemies.empty()) {
        beginVictorySequence();
    }

    // Reset blocking
    for (auto& op : m_Operators) op->clearBlockedEnemies();
    for (auto& enemy : m_ActiveEnemies) {
        enemy->setBlocked(false);
        enemy->setTargetOperator(nullptr);
    }

    handleOperatorDrag(deltaTime);

    // Update operators and combat
    for (auto& op : m_Operators) {
        if (!op->GetVisible()) continue;
        op->update(gameplayDeltaTime);
        if (!op->isAlive()) continue;

        if (op->canAttack()) {
            Enemy* target = nullptr;
            for (auto& enemy : m_ActiveEnemies) {
                if (enemy->isAlive() && op->isInAttackRange(enemy->getGridPosition())) {
                    target = enemy;
                    break;
                }
            }
            if (target) {
                op->resetAttackTimer();
                op->playAttackAnimation();
            }
        }

        if (op->consumeDamageTrigger()) {
            Enemy* target = nullptr;
            for (auto& enemy : m_ActiveEnemies) {
                if (enemy->isAlive() && enemy->getTargetOperator() == op.get() && op->isInAttackRange(enemy->getGridPosition())) {
                    target = enemy;
                    break;
                }
            }
            if (!target) {
                for (auto& enemy : m_ActiveEnemies) {
                    if (enemy->isAlive() && op->isInAttackRange(enemy->getGridPosition())) {
                        target = enemy;
                        break;
                    }
                }
            }
            if (target) {
                for (std::size_t k = 0; k < op->getAttackCount(); ++k) {
                    target->takeDamage(op->getAttack());
                }
            }
        }

        for (auto& enemy : m_ActiveEnemies) {
            if (!enemy->isAlive()) continue;
            if (!enemy->isBlocked() && op->canBlockMore()) {
                glm::vec2 opGridPos = op->getGridPosition();
                glm::vec2 enemyGridPos = enemy->getGridPosition();
                if (std::floor(opGridPos.x) == std::floor(enemyGridPos.x) &&
                    std::floor(opGridPos.y) == std::floor(enemyGridPos.y)) {
                    enemy->setBlocked(true);
                    enemy->setTargetOperator(op.get());
                    op->blockEnemy(enemy);
                }
            }
        }
    }

    // Sync visibility
    m_AmiyaCostText->m_Transform.translation = {m_AmiyaIcon->m_Transform.translation.x, m_AmiyaIcon->m_Transform.translation.y + 67.0f};
    m_ChenCostText->m_Transform.translation = {m_ChenIcon->m_Transform.translation.x, m_ChenIcon->m_Transform.translation.y + 67.0f};
    m_AngelinaCostText->m_Transform.translation = {m_AngelinaIcon->m_Transform.translation.x, m_AngelinaIcon->m_Transform.translation.y + 67.0f};
    m_RedCostText->m_Transform.translation = {m_RedIcon->m_Transform.translation.x, m_RedIcon->m_Transform.translation.y + 67.0f};
    m_EyjafjallaCostText->m_Transform.translation = {m_EyjafjallaIcon->m_Transform.translation.x, m_EyjafjallaIcon->m_Transform.translation.y + 67.0f};
    m_TexasCostText->m_Transform.translation = {m_TexasIcon->m_Transform.translation.x, m_TexasIcon->m_Transform.translation.y + 67.0f};
    m_UmirinCostText->m_Transform.translation = {m_UmirinIcon->m_Transform.translation.x, m_UmirinIcon->m_Transform.translation.y + 67.0f};

    m_AmiyaCostText->SetVisible(m_AmiyaIcon->GetVisible());
    m_ChenCostText->SetVisible(m_ChenIcon->GetVisible());
    m_AngelinaCostText->SetVisible(m_AngelinaIcon->GetVisible());
    m_RedCostText->SetVisible(m_RedIcon->GetVisible());
    m_EyjafjallaCostText->SetVisible(m_EyjafjallaIcon->GetVisible());
    m_TexasCostText->SetVisible(m_TexasIcon->GetVisible());
    m_UmirinCostText->SetVisible(m_UmirinIcon->GetVisible());
    updateOperatorPanel(Util::Input::GetCursorPosition());
    updateHudText();
}

void GameScene::updateHudText() {
    auto enemyCountDrawable = std::dynamic_pointer_cast<Util::Text>(m_EnemyCountText->GetDrawable());
    enemyCountDrawable->SetText("ESCAPED: " + std::to_string(m_EscapedEnemies) + " / " + std::to_string(MAX_ESCAPED_ENEMIES));
}

std::shared_ptr<Operator> GameScene::getHoveredOperator(const glm::vec2& mousePos) const {
    if (m_AmiyaIcon && m_AmiyaIcon->GetVisible() && glm::distance(mousePos, m_AmiyaIcon->m_Transform.translation) < 60.0f && m_Operators.size() >= 1) return m_Operators[0];
    if (m_ChenIcon && m_ChenIcon->GetVisible() && glm::distance(mousePos, m_ChenIcon->m_Transform.translation) < 60.0f && m_Operators.size() >= 2) return m_Operators[1];
    if (m_AngelinaIcon && m_AngelinaIcon->GetVisible() && glm::distance(mousePos, m_AngelinaIcon->m_Transform.translation) < 60.0f && m_Operators.size() >= 3) return m_Operators[2];
    if (m_RedIcon && m_RedIcon->GetVisible() && glm::distance(mousePos, m_RedIcon->m_Transform.translation) < 60.0f && m_Operators.size() >= 4) return m_Operators[3];
    if (m_EyjafjallaIcon && m_EyjafjallaIcon->GetVisible() && glm::distance(mousePos, m_EyjafjallaIcon->m_Transform.translation) < 60.0f && m_Operators.size() >= 5) return m_Operators[4];
    if (m_TexasIcon && m_TexasIcon->GetVisible() && glm::distance(mousePos, m_TexasIcon->m_Transform.translation) < 60.0f && m_Operators.size() >= 6) return m_Operators[5];
    if (m_UmirinIcon && m_UmirinIcon->GetVisible() && glm::distance(mousePos, m_UmirinIcon->m_Transform.translation) < 60.0f && m_Operators.size() >= 7) return m_Operators[6];

    for (const auto& op : m_Operators) {
        if (op->GetVisible() && glm::distance(mousePos, op->getPosition()) < 55.0f) {
            return op;
        }
    }
    return nullptr;
}

void GameScene::updateOperatorPanel(const glm::vec2& mousePos) {
    if (!m_OperatorPanelTitleText || !m_OperatorPanelLine1Text ||
        !m_OperatorPanelLine2Text || !m_OperatorPanelLine3Text ||
        !m_OperatorPanelLine4Text) {
        return;
    }

    std::shared_ptr<Operator> selected = getHoveredOperator(mousePos);
    if (!selected && m_DraggedOperator) selected = m_DraggedOperator;
    if (!selected && m_ChoosingDirectionOperator) selected = m_ChoosingDirectionOperator;

    auto titleDrawable = std::dynamic_pointer_cast<Util::Text>(m_OperatorPanelTitleText->GetDrawable());
    auto line1Drawable = std::dynamic_pointer_cast<Util::Text>(m_OperatorPanelLine1Text->GetDrawable());
    auto line2Drawable = std::dynamic_pointer_cast<Util::Text>(m_OperatorPanelLine2Text->GetDrawable());
    auto line3Drawable = std::dynamic_pointer_cast<Util::Text>(m_OperatorPanelLine3Text->GetDrawable());
    auto line4Drawable = std::dynamic_pointer_cast<Util::Text>(m_OperatorPanelLine4Text->GetDrawable());
    if (!titleDrawable || !line1Drawable || !line2Drawable || !line3Drawable || !line4Drawable) {
        return;
    }

    if (!selected) {
        titleDrawable->SetText("Operator: -");
        line1Drawable->SetText("HP: - / -");
        line2Drawable->SetText("ATK: -  COST: -");
        line3Drawable->SetText("INTERVAL: -s  BLOCK: -");
        line4Drawable->SetText("STATE: Hover icon/operator");
        return;
    }

    std::ostringstream hpStream;
    hpStream << "HP: " << static_cast<int>(selected->getHp()) << " / " << static_cast<int>(selected->getMaxHp());
    std::ostringstream statStream;
    statStream << "ATK: " << static_cast<int>(selected->getAttack()) << "  COST: " << selected->getDeploymentCost();
    std::ostringstream intervalStream;
    intervalStream << std::fixed << std::setprecision(2) << selected->getAttackInterval();

    titleDrawable->SetText(std::string("Operator: ") + operatorName(selected->getType()));
    line1Drawable->SetText(hpStream.str());
    line2Drawable->SetText(statStream.str());
    line3Drawable->SetText("INTERVAL: " + intervalStream.str() + "s  BLOCK: " + std::to_string(selected->getBlockCount()));
    line4Drawable->SetText(std::string("STATE: ") + operatorStateName(selected->getState()));
}

void GameScene::updateButtons(float deltaTime) {
    const bool showBaseHud = !m_IsGameOver && !m_IsHubOpen && !m_IsExitConfirmOpen;
    const bool showPauseMenu = showBaseHud && m_IsPaused;
    const bool showSettingsMenu = !m_IsGameOver && m_IsHubOpen && !m_IsExitConfirmOpen;
    const bool showExitConfirm = !m_IsGameOver && m_IsExitConfirmOpen;

    if (m_GameUIOverlay) m_GameUIOverlay->SetVisible(!m_IsGameOver);
    if (m_SettingsOverlayImage) m_SettingsOverlayImage->SetVisible(showSettingsMenu);
    if (m_ExitOverlayImage) m_ExitOverlayImage->SetVisible(showExitConfirm);
    if (m_PauseOverlayImage) m_PauseOverlayImage->SetVisible(showPauseMenu);
    if (m_HubOverlayText) m_HubOverlayText->SetVisible(showSettingsMenu);

    if (m_SettingsButton) {
        m_SettingsButton->SetVisible(showBaseHud);
        if (showBaseHud) m_SettingsButton->update(deltaTime);
    }
    if (m_PauseButton) {
        m_PauseButton->SetVisible(showBaseHud);
        if (showBaseHud) m_PauseButton->update(deltaTime);
    }
    if (m_SpeedButton) {
        m_SpeedButton->SetVisible(showBaseHud && !m_IsPaused);
        if (showBaseHud && !m_IsPaused) m_SpeedButton->update(deltaTime);
    }
    if (m_NormalModeButton) {
        m_NormalModeButton->SetVisible(showSettingsMenu);
        if (showSettingsMenu) m_NormalModeButton->update(deltaTime);
    }
    if (m_CheatModeButton) {
        m_CheatModeButton->SetVisible(showSettingsMenu);
        if (showSettingsMenu) m_CheatModeButton->update(deltaTime);
    }

    if (m_PauseOverlayText) m_PauseOverlayText->SetVisible(showPauseMenu);
    if (m_PauseHintText) m_PauseHintText->SetVisible(showPauseMenu);
    if (m_ResumeButton) {
        m_ResumeButton->SetVisible(false);
    }
    if (m_RetryButton) {
        m_RetryButton->SetVisible(false);
    }
    if (m_ExitButton) {
        m_ExitButton->SetVisible(showSettingsMenu);
        if (showSettingsMenu) m_ExitButton->update(deltaTime);
    }
    if (m_ExitCancelButton) {
        m_ExitCancelButton->SetVisible(showExitConfirm);
        if (showExitConfirm) m_ExitCancelButton->update(deltaTime);
    }
    if (m_ExitConfirmButton) {
        m_ExitConfirmButton->SetVisible(showExitConfirm);
        if (showExitConfirm) m_ExitConfirmButton->update(deltaTime);
    }
}

void GameScene::toggleHub() {
    if (m_IsGameOver) return;
    m_IsHubOpen = !m_IsHubOpen;
    m_IsExitConfirmOpen = false;
    if (m_IsHubOpen) {
        m_IsPaused = false;
        m_DraggedOperator = nullptr;
        m_DraggedIcon = nullptr;
        m_ChoosingDirectionOperator = nullptr;
        m_ChoosingDirectionIcon = nullptr;
    }
}

void GameScene::togglePause() {
    if (m_IsGameOver) return;
    if (m_IsHubOpen || m_IsExitConfirmOpen) return;
    m_IsPaused = !m_IsPaused;
    if (m_IsPaused) {
        m_DraggedOperator = nullptr;
        m_DraggedIcon = nullptr;
        m_ChoosingDirectionOperator = nullptr;
        m_ChoosingDirectionIcon = nullptr;
    }
    updateHudText();
}

void GameScene::openExitConfirm() {
    if (m_IsGameOver) return;
    m_IsExitConfirmOpen = true;
    m_IsHubOpen = false;
    m_IsPaused = false;
    m_DraggedOperator = nullptr;
    m_DraggedIcon = nullptr;
    m_ChoosingDirectionOperator = nullptr;
    m_ChoosingDirectionIcon = nullptr;
}

void GameScene::closeExitConfirm() {
    m_IsExitConfirmOpen = false;
    m_IsHubOpen = false;
    m_IsPaused = false;
}

void GameScene::returnToStageSelect() {
    Core::SceneManager::getInstance().replaceScene(std::make_shared<StageSelectScene>());
}

void GameScene::playClickSFX() {
    if (m_ClickSFX) {
        m_ClickSFX->Play();
    }
}

void GameScene::setGameSpeed(float speedMultiplier) {
    m_GameSpeedMultiplier = speedMultiplier >= 1.5f ? 2.0f : 1.0f;
    updateHudText();
}

void GameScene::setCheatMode(bool enabled) {
    m_IsCheatMode = enabled;
    if (m_CheatModeText) {
        m_CheatModeText->SetVisible(enabled);
    }
    if (m_NormalModeButton) {
        m_NormalModeButton->setTextColor(enabled ? Util::Color(150, 150, 150) : Util::Color(80, 255, 80));
    }
    if (m_CheatModeButton) {
        m_CheatModeButton->setTextColor(enabled ? Util::Color(255, 80, 80) : Util::Color(150, 150, 150));
    }
}

void GameScene::handleOperatorDrag(float /*deltaTime*/) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();

    if (m_ChoosingDirectionOperator) {
        glm::vec2 opPos = m_ChoosingDirectionOperator->getPosition();
        glm::vec2 delta = mousePos - opPos;
        if (glm::length(delta) > 20.0f) {
            if (std::abs(delta.x) > std::abs(delta.y)) {
                if (delta.x > 0) m_ChoosingDirectionOperator->setDirection(Operator::Direction::RIGHT);
                else m_ChoosingDirectionOperator->setDirection(Operator::Direction::LEFT);
            } else {
                if (delta.y > 0) m_ChoosingDirectionOperator->setDirection(Operator::Direction::UP);
                else m_ChoosingDirectionOperator->setDirection(Operator::Direction::DOWN);
            }
        }
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            m_ChoosingDirectionOperator = nullptr;
            m_ChoosingDirectionIcon = nullptr;
        }
    } else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        if (m_AmiyaIcon->GetVisible() && glm::distance(mousePos, m_AmiyaIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 1 && (m_IsCheatMode || m_CurrentDP >= m_Operators[0]->getDeploymentCost())) {
                m_DraggedOperator = m_Operators[0]; m_DraggedIcon = m_AmiyaIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_ChenIcon->GetVisible() && glm::distance(mousePos, m_ChenIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 2 && (m_IsCheatMode || m_CurrentDP >= m_Operators[1]->getDeploymentCost())) {
                m_DraggedOperator = m_Operators[1]; m_DraggedIcon = m_ChenIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_AngelinaIcon->GetVisible() && glm::distance(mousePos, m_AngelinaIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 3 && (m_IsCheatMode || m_CurrentDP >= m_Operators[2]->getDeploymentCost())) {
                m_DraggedOperator = m_Operators[2]; m_DraggedIcon = m_AngelinaIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_RedIcon->GetVisible() && glm::distance(mousePos, m_RedIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 4 && (m_IsCheatMode || m_CurrentDP >= m_Operators[3]->getDeploymentCost())) {
                m_DraggedOperator = m_Operators[3]; m_DraggedIcon = m_RedIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_EyjafjallaIcon->GetVisible() && glm::distance(mousePos, m_EyjafjallaIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 5 && (m_IsCheatMode || m_CurrentDP >= m_Operators[4]->getDeploymentCost())) {
                m_DraggedOperator = m_Operators[4]; m_DraggedIcon = m_EyjafjallaIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_TexasIcon->GetVisible() && glm::distance(mousePos, m_TexasIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 6 && (m_IsCheatMode || m_CurrentDP >= m_Operators[5]->getDeploymentCost())) {
                m_DraggedOperator = m_Operators[5]; m_DraggedIcon = m_TexasIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_UmirinIcon->GetVisible() && glm::distance(mousePos, m_UmirinIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 7 && (m_IsCheatMode || m_CurrentDP >= m_Operators[6]->getDeploymentCost())) {
                m_DraggedOperator = m_Operators[6]; m_DraggedIcon = m_UmirinIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else {
            for (std::size_t i = 0; i < m_Operators.size(); ++i) {
                auto& op = m_Operators[i];
                if (op->GetVisible() && glm::distance(mousePos, op->getPosition()) < 50.0f) {
                    op->SetVisible(false);
                    if (i == 0) m_AmiyaIcon->SetVisible(true);
                    else if (i == 1) m_ChenIcon->SetVisible(true);
                    else if (i == 2) m_AngelinaIcon->SetVisible(true);
                    else if (i == 3) m_RedIcon->SetVisible(true);
                    else if (i == 4) m_EyjafjallaIcon->SetVisible(true);
                    else if (i == 5) m_TexasIcon->SetVisible(true);
                    else if (i == 6) m_UmirinIcon->SetVisible(true);
                    break;
                }
            }
        }
    }

    if (m_DraggedOperator) {
        m_DraggedOperator->setPosition(mousePos);
        if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB)) {
            int r, c; bool dropped = false;
            if (m_CurrentOperation->getTileIndices(mousePos, r, c)) {
                Operation::TileType type = m_CurrentOperation->getTileType(r, c);
                bool canPlace = false;
                auto t = m_DraggedOperator->getType();
                if (t == Operator::Type::AMIYA || t == Operator::Type::ANGELINA || t == Operator::Type::EYJAFJALLA || t == Operator::Type::UMIRIN)
                    canPlace = (type == Operation::TileType::HIGH_GROUND);
                else
                    canPlace = (type == Operation::TileType::GROUND || type == Operation::TileType::SPAWN);

                if (canPlace) {
                    m_DraggedOperator->setPosition(m_CurrentOperation->getTileCenterPos(r, c));
                    m_DraggedOperator->setGridPosition({static_cast<float>(r) + 0.5f, static_cast<float>(c) + 0.5f});
                    if (!m_IsCheatMode) {
                        m_CurrentDP -= m_DraggedOperator->getDeploymentCost();
                        auto dpTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_DPText->GetDrawable());
                        dpTextDrawable->SetText("COST: " + std::to_string(static_cast<int>(m_CurrentDP)));
                    }
                    m_ChoosingDirectionOperator = m_DraggedOperator; m_ChoosingDirectionIcon = m_DraggedIcon;
                    dropped = true;
                }
            }
            if (!dropped && m_DraggedIcon) {
                m_DraggedOperator->SetVisible(false); m_DraggedIcon->SetVisible(true);
            }
            m_DraggedOperator = nullptr; m_DraggedIcon = nullptr;
        }
    }
}

void GameScene::reset() {
    m_IsGameOver = false; m_WaveTimer = 0.0f; m_EscapedEnemies = 0; m_KilledEnemies = 0;
    m_BaseHP = MAX_ESCAPED_ENEMIES;
    m_IsPaused = false;
    m_IsHubOpen = false;
    m_IsExitConfirmOpen = false;
    m_GameSpeedMultiplier = 1.0f;
    m_ResultPhase = ResultPhase::NONE;
    m_ResultTimer = 0.0f;
    m_CurrentDP = 10.0f; m_DPAccumulator = 0.0f;
    auto dpTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_DPText->GetDrawable());
    dpTextDrawable->SetText("COST: 10");
    m_GameOverText->SetVisible(false); m_RestartText->SetVisible(false);
    m_MissionCompletedImage->SetVisible(false);
    m_YourWinImage->SetVisible(false);
    m_MissionFailedImage->SetVisible(false);
    for (auto* enemy : m_ActiveEnemies) m_EnemyPool->returnEnemy(enemy);
    m_ActiveEnemies.clear();
    for (auto& op : m_Operators) { op->SetVisible(false); op->reset(); }
    m_AmiyaIcon->SetVisible(true); m_ChenIcon->SetVisible(true);
    m_AngelinaIcon->SetVisible(true); m_RedIcon->SetVisible(true);
    m_EyjafjallaIcon->SetVisible(true); m_TexasIcon->SetVisible(true);
    m_UmirinIcon->SetVisible(true);
    m_DraggedOperator = nullptr;
    m_DraggedIcon = nullptr;
    m_ChoosingDirectionOperator = nullptr;
    m_ChoosingDirectionIcon = nullptr;
    m_CurrentOperation->getWaveManager().reset();
    updateOperatorPanel(Util::Input::GetCursorPosition());
    updateHudText();
    m_BattleBGM->Play();
}

void GameScene::beginVictorySequence() {
    m_IsGameOver = true;
    m_IsVictory = true;
    m_IsPaused = false;
    m_ResultPhase = ResultPhase::VICTORY_SLIDE;
    m_ResultTimer = 0.0f;
    cleanupCharactersForResult();
    TriggerMissionComplete();
    m_YourWinImage->SetVisible(false);
    m_MissionFailedImage->SetVisible(false);
}

void GameScene::beginFailureSequence() {
    m_IsGameOver = true;
    m_IsVictory = false;
    m_IsPaused = false;
    m_ResultPhase = ResultPhase::FAILURE_SHOW;
    m_ResultTimer = 0.0f;
    cleanupCharactersForResult();
    m_MissionCompletedImage->SetVisible(false);
    m_YourWinImage->SetVisible(false);
    m_MissionFailedImage->SetVisible(true);
}

void GameScene::cleanupCharactersForResult() {
    for (auto* enemy : m_ActiveEnemies) {
        m_EnemyPool->returnEnemy(enemy);
    }
    m_ActiveEnemies.clear();

    for (auto& op : m_Operators) {
        op->SetVisible(false);
    }
}

bool GameScene::isAnyReturnInput() const {
    return Util::Input::IsKeyDown(Util::Keycode::SPACE)
        || Util::Input::IsKeyDown(Util::Keycode::RETURN)
        || Util::Input::IsKeyDown(Util::Keycode::ESCAPE)
        || Util::Input::IsKeyDown(Util::Keycode::A)
        || Util::Input::IsKeyDown(Util::Keycode::B)
        || Util::Input::IsKeyDown(Util::Keycode::C)
        || Util::Input::IsKeyDown(Util::Keycode::D)
        || Util::Input::IsKeyDown(Util::Keycode::E)
        || Util::Input::IsKeyDown(Util::Keycode::F)
        || Util::Input::IsKeyDown(Util::Keycode::G)
        || Util::Input::IsKeyDown(Util::Keycode::H)
        || Util::Input::IsKeyDown(Util::Keycode::I)
        || Util::Input::IsKeyDown(Util::Keycode::J)
        || Util::Input::IsKeyDown(Util::Keycode::K)
        || Util::Input::IsKeyDown(Util::Keycode::L)
        || Util::Input::IsKeyDown(Util::Keycode::M)
        || Util::Input::IsKeyDown(Util::Keycode::N)
        || Util::Input::IsKeyDown(Util::Keycode::O)
        || Util::Input::IsKeyDown(Util::Keycode::P)
        || Util::Input::IsKeyDown(Util::Keycode::Q)
        || Util::Input::IsKeyDown(Util::Keycode::R)
        || Util::Input::IsKeyDown(Util::Keycode::S)
        || Util::Input::IsKeyDown(Util::Keycode::T)
        || Util::Input::IsKeyDown(Util::Keycode::U)
        || Util::Input::IsKeyDown(Util::Keycode::V)
        || Util::Input::IsKeyDown(Util::Keycode::W)
        || Util::Input::IsKeyDown(Util::Keycode::X)
        || Util::Input::IsKeyDown(Util::Keycode::Y)
        || Util::Input::IsKeyDown(Util::Keycode::Z)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_0)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_1)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_2)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_3)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_4)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_5)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_6)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_7)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_8)
        || Util::Input::IsKeyDown(Util::Keycode::NUM_9)
        || Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)
        || Util::Input::IsKeyDown(Util::Keycode::MOUSE_MB)
        || Util::Input::IsKeyDown(Util::Keycode::MOUSE_RB);
}

void GameScene::cleanup() {
    LOG_DEBUG("Cleaning up GameScene");
    m_ActiveEnemies.clear();
    m_Operators.clear();
    m_BattleBGM.reset();
    m_ClickSFX.reset();
}

void GameScene::onEnter() {
    LOG_DEBUG("Entering GameScene");
    if (m_BattleBGM) m_BattleBGM->Play();
}

void GameScene::onExit() {
    LOG_DEBUG("Exiting GameScene");
    if (m_BattleBGM) m_BattleBGM->FadeOut(500);
}

void GameScene::TriggerMissionComplete() {
    if (m_MissionAnimState != MissionAnimState::IDLE &&
        m_MissionAnimState != MissionAnimState::DONE) return;

    m_MissionCompletedBar->m_Transform.translation   = {BAR_OFFSCREEN_X,  BAR_TARGET_Y};
    m_MissionCompletedImage->m_Transform.translation = {LOGO_OFFSCREEN_X, LOGO_TARGET_Y};

    m_MissionCompletedBar->SetVisible(true);
    m_MissionCompletedImage->SetVisible(true);

    m_MissionAnimTimer = 0.0f;
    m_MissionAnimState = MissionAnimState::BAR_SLIDE_IN;
}

void GameScene::UpdateMissionAnimation() {
    if (m_MissionAnimState == MissionAnimState::IDLE ||
        m_MissionAnimState == MissionAnimState::DONE) return;

    const float dt = Util::Time::GetDeltaTimeMs() / 1000.0f; 
    m_MissionAnimTimer += dt;

    auto easeOut = [](float t) -> float {
        t = std::clamp(t, 0.0f, 1.0f);
        return 1.0f - (1.0f - t) * (1.0f - t);
    };
    
    auto easeIn = [](float t) -> float {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t;
    };

    auto lerp = [](float a, float b, float t) -> float {
        return a + (b - a) * t;
    };

    switch (m_MissionAnimState) {
    case MissionAnimState::BAR_SLIDE_IN: {
        float t = m_MissionAnimTimer / ANIM_BAR_SLIDE_DURATION;
        float x = lerp(BAR_OFFSCREEN_X, BAR_TARGET_X, easeOut(t));
        m_MissionCompletedBar->m_Transform.translation = {x, BAR_TARGET_Y};

        if (t >= 1.0f) {
            m_MissionCompletedBar->m_Transform.translation = {BAR_TARGET_X, BAR_TARGET_Y};
            m_MissionAnimState = MissionAnimState::LOGO_SLIDE_IN;
            m_MissionAnimTimer = 0.0f;
        }
        break;
    }
    case MissionAnimState::LOGO_SLIDE_IN: {
        float t = m_MissionAnimTimer / ANIM_LOGO_SLIDE_DURATION;
        float x = lerp(LOGO_OFFSCREEN_X, LOGO_TARGET_X, easeOut(t));
        m_MissionCompletedImage->m_Transform.translation = {x, LOGO_TARGET_Y};

        if (t >= 1.0f) {
            m_MissionCompletedImage->m_Transform.translation = {LOGO_TARGET_X, LOGO_TARGET_Y};
            m_MissionAnimState = MissionAnimState::HOLD;
            m_MissionAnimTimer = 0.0f;
        }
        break;
    }
    case MissionAnimState::HOLD: {
        if (m_MissionAnimTimer >= ANIM_HOLD_DURATION) {
            m_MissionAnimState = MissionAnimState::FADE_OUT; // Actually sliding out right
            m_MissionAnimTimer = 0.0f;
        }
        break;
    }
    case MissionAnimState::FADE_OUT: {
        float t = m_MissionAnimTimer / ANIM_FADE_DURATION;
        float targetX = 1500.0f; // Far enough right to clear the screen
        
        float barX = lerp(BAR_TARGET_X, targetX, easeIn(t));
        m_MissionCompletedBar->m_Transform.translation = {barX, BAR_TARGET_Y};
        
        float logoX = lerp(LOGO_TARGET_X, targetX, easeIn(t));
        m_MissionCompletedImage->m_Transform.translation = {logoX, LOGO_TARGET_Y};

        if (t >= 1.0f) {
            m_MissionCompletedBar->SetVisible(false);
            m_MissionCompletedImage->SetVisible(false);
            m_MissionAnimState = MissionAnimState::DONE;
        }
        break;
    }
    default: break;
    }
}

} // namespace Arknights
