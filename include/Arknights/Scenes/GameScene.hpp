#ifndef ARKNIGHTS_GAMESCENE_HPP
#define ARKNIGHTS_GAMESCENE_HPP

#include <memory>
#include <vector>
#include <string>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/Enemy.hpp"
#include "Arknights/EnemyPool.hpp"
#include "Arknights/Operation.hpp"
#include "Arknights/WaveManager.hpp"
#include "Arknights/Operator.hpp"
#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
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

private:
    void initOperation();
    void initEnemies();
    void initOperators();
    void initWaveManager();
    void initUI();
    
    void spawnEnemy(const SpawnEvent& event);
    void updateEnemies(float deltaTime);
    void updateOperators(float deltaTime);
    void updateWaveSystem(float deltaTime);
    void handleOperatorDrag(float deltaTime);
    
    void checkVictoryCondition();
    void checkDefeatCondition();

private:
    // Operation/Map
    std::unique_ptr<Operation> m_CurrentOperation;
    std::vector<glm::vec2> m_Waypoints;
    
    // Enemy system
    std::unique_ptr<EnemyPool> m_EnemyPool;
    std::vector<Enemy*> m_ActiveEnemies;
    std::vector<std::string> m_EnemyAnimationPathsGopro;
    std::vector<std::string> m_EnemyAnimationPathsBigbo;
    
    // Wave system
    std::unique_ptr<WaveManager> m_WaveManager;
    float m_WaveTimer = 0.0f;
    
    // Operator system
    std::vector<std::shared_ptr<Operator>> m_Operators;
    std::shared_ptr<Operator> m_DraggedOperator = nullptr;
    
    // UI
    std::shared_ptr<Util::GameObject> m_StageText;
    
    // Audio
    std::unique_ptr<Util::BGM> m_BattleBGM;
    
    // Game state
    bool m_IsGameOver = false;
    bool m_IsVictory = false;
    int m_BaseHP = 10;
    
    // Constants
    static constexpr std::size_t ENEMY_POOL_SIZE = 32;
    static constexpr float TILE_SIZE = 100.0F;
    static constexpr float BLOCK_DISTANCE = 50.0f;
};

} // namespace Arknights

#endif
