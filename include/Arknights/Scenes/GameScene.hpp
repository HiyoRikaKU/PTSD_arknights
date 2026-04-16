#ifndef ARKNIGHTS_GAMESCENE_HPP
#define ARKNIGHTS_GAMESCENE_HPP

#include <memory>
#include <vector>
#include <string>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/Entity/Enemy.hpp"
#include "Arknights/Map/Operation.hpp"
#include "Arknights/Entity/Operator.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include "Util/Image.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"

namespace Arknights {

class GameScene : public Scene {
public:
    GameScene();
    ~GameScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;
    
    void onEnter() override;
    void onExit() override;
    
    void reset();

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

private:
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
    
    // Stats
    float m_WaveTimer = 0.0f;
    int m_EscapedEnemies = 0;
    int m_KilledEnemies = 0;
    int m_TotalEnemies = 0;
    bool m_IsGameOver = false;
    bool m_IsVictory = false;
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
    std::shared_ptr<ExGameObject> m_MissionCompletedImage;
    std::shared_ptr<ExGameObject> m_YourWinImage;
    std::shared_ptr<ExGameObject> m_MissionFailedImage;

    // Audio
    std::unique_ptr<Util::BGM> m_BattleBGM;
    
    // Constants
    static constexpr std::size_t ENEMY_POOL_SIZE = 64;
    static constexpr int MAX_ESCAPED_ENEMIES = 4;
};

} // namespace Arknights

#endif
