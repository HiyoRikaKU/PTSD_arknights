#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include <iomanip>
#include <sstream>

namespace Arknights {

GameScene::GameScene() {
}

void GameScene::init() {
    LOG_DEBUG("Initializing GameScene");
    
    initOperation();
    initEnemies();
    initOperators();
    initWaveManager();
    initUI();
    
    // Battle BGM
    m_BattleBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/battle/battle.mp3");
    
    m_Initialized = true;
}

void GameScene::initOperation() {
    // Enemy path: left left, down, left left, up-left, left left, down, left left
    glm::vec2 spawnPos = {1.5f, 9.5f}; // {row, col}
    std::vector<glm::vec2> steps = {
        {-1,  0}, {-1,  0}, // left left
        { 0, -1},           // down
        {-1,  0}, {-1,  0}, // left left
        {-1,  1},           // up-left
        {-1,  0}, {-1,  0}, // left left
        { 0, -1},           // down
        {-1,  0}, {-1,  0}  // left left (disappear)
    };

    m_Waypoints.clear();
    m_Waypoints.push_back(spawnPos);
    glm::vec2 currentPos = spawnPos;
    for (const auto& s : steps) {
        currentPos.x -= s.y; // +y is Up -> Row decreases
        currentPos.y += s.x; // +x is Right -> Col increases
        m_Waypoints.push_back(currentPos);
    }
    
    m_CurrentOperation = std::make_unique<Operation>(
        std::string(RESOURCE_DIR) + "/map/operation1.jpg", 
        m_Waypoints
    );
    m_CurrentOperation->getMap()->SetVisible(true);
    m_Root.AddChild(m_CurrentOperation->getMap());
}

void GameScene::initEnemies() {
    // Preload Gopro animations
    m_EnemyAnimationPathsGopro.clear();
    for (int i = 1; i <= 25; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Move_Loop_" 
           << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsGopro.push_back(ss.str());
    }

    // Preload Bigbo animations
    m_EnemyAnimationPathsBigbo.clear();
    for (int i = 1; i <= 31; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1001_bigbo/Move_Loop_" 
           << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsBigbo.push_back(ss.str());
    }

    // Create enemy pool
    m_EnemyPool = std::make_unique<EnemyPool>(
        ENEMY_POOL_SIZE, 
        m_EnemyAnimationPathsGopro, 
        m_Waypoints
    );

    // Register all pooled enemies to the renderer
    for (const auto &handle : m_EnemyPool->getRenderHandles()) {
        m_Root.AddChild(handle);
    }
}

void GameScene::initOperators() {
    // Initialize Amiya
    std::vector<std::string> amiyaIdlePaths;
    for (int i = 1; i <= 31; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/char_002_amiya/Idle_" 
           << std::setfill('0') << std::setw(2) << i << ".png";
        amiyaIdlePaths.push_back(ss.str());
    }
    
    auto amiya = std::make_shared<Operator>(amiyaIdlePaths, 1000.0f, 100.0f);
    amiya->setPosition({0.0f, 120.0f});
    amiya->m_Transform.scale = glm::vec2(-0.3f, 0.3f);
    amiya->SetVisible(true);
    m_Operators.push_back(amiya);
    m_Root.AddChild(amiya);
}

void GameScene::initWaveManager() {
    m_WaveManager = std::make_unique<WaveManager>();
    m_WaveManager->addSpawnEvent(2000.0f, "gopro", 100.0f, 0.001f);
    m_WaveManager->addSpawnEvent(4000.0f, "gopro", 100.0f, 0.001f);
    m_WaveManager->addSpawnEvent(6000.0f, "bigbo", 300.0f, 0.0005f);
    m_WaveManager->addSpawnEvent(8000.0f, "bigbo", 300.0f, 0.0005f);
    m_WaveManager->addSpawnEvent(10000.0f, "gopro", 150.0f, 0.001f);
}

void GameScene::initUI() {
    m_StageText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(
            std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 
            48, 
            "Operation Level: 0-1",
            Util::Color(255, 255, 255)
        ),
        20
    );
    m_StageText->m_Transform.translation = {0, 320};
    m_Root.AddChild(m_StageText);
}

void GameScene::spawnEnemy(const SpawnEvent& event) {
    if (!m_EnemyPool || !m_CurrentOperation) {
        return;
    }

    Enemy *enemy = m_EnemyPool->getEnemy();
    if (enemy == nullptr) {
        LOG_WARN("Enemy pool exhausted; spawn skipped");
        return;
    }

    // Set animation based on type
    if (event.enemyType == "gopro") {
        enemy->setAnimation(m_EnemyAnimationPathsGopro);
    } else if (event.enemyType == "bigbo") {
        enemy->setAnimation(m_EnemyAnimationPathsBigbo);
    }

    const auto& waypoints = m_CurrentOperation->getWaypoints();
    enemy->spawn(waypoints, event.hp, event.speed, m_CurrentOperation->getHomography());
    m_ActiveEnemies.push_back(enemy);
    LOG_DEBUG("Enemy spawned: type={}, hp={}, speed={}", event.enemyType, event.hp, event.speed);
}

void GameScene::update(float deltaTime) {
    if (m_IsGameOver) {
        return;
    }
    
    updateWaveSystem(deltaTime);
    updateEnemies(deltaTime);
    updateOperators(deltaTime);
    handleOperatorDrag(deltaTime);
    
    checkVictoryCondition();
    checkDefeatCondition();
}

void GameScene::updateWaveSystem(float deltaTime) {
    m_WaveTimer += deltaTime;

    SpawnEvent event;
    while (m_WaveManager->shouldSpawn(m_WaveTimer, event)) {
        spawnEnemy(event);
    }
}

void GameScene::updateEnemies(float deltaTime) {
    for (std::size_t i = 0; i < m_ActiveEnemies.size();) {
        Enemy *enemy = m_ActiveEnemies[i];
        enemy->update(deltaTime);

        // Check if enemy reached the end
        if (!enemy->isActive() || enemy->reachedEnd()) {
            if (enemy->reachedEnd()) {
                m_BaseHP--;
                LOG_DEBUG("Enemy reached base! Base HP: {}", m_BaseHP);
            }
            
            m_EnemyPool->returnEnemy(enemy);
            m_ActiveEnemies[i] = m_ActiveEnemies.back();
            m_ActiveEnemies.pop_back();
            continue;
        }
        ++i;
    }
}

void GameScene::updateOperators(float deltaTime) {
    // Reset blocking states
    for (auto& op : m_Operators) {
        op->clearBlockedEnemies();
    }
    for (auto& enemy : m_ActiveEnemies) {
        enemy->setBlocked(false);
    }

    // Update operators and check for blocking
    for (auto& op : m_Operators) {
        op->update(deltaTime);
        for (auto& enemy : m_ActiveEnemies) {
            if (!enemy->isBlocked() && op->canBlockMore()) {
                if (glm::distance(op->getPosition(), enemy->getPosition()) < BLOCK_DISTANCE) {
                    enemy->setBlocked(true);
                    op->blockEnemy(enemy);
                }
            }
        }
    }
}

void GameScene::handleOperatorDrag(float /*deltaTime*/) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        if (!m_DraggedOperator) {
            for (auto& op : m_Operators) {
                if (glm::distance(mousePos, op->getPosition()) < 50.0f) {
                    m_DraggedOperator = op;
                    break;
                }
            }
        }
    }
    
    if (m_DraggedOperator) {
        m_DraggedOperator->setPosition(mousePos);
        
        if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB)) {
            // Snap to grid
            int r, c;
            if (m_CurrentOperation->getTileIndices(mousePos, r, c)) {
                Operation::TileType type = m_CurrentOperation->getTileType(r, c);
                // Check terrain legality
                if (type == Operation::TileType::GROUND || 
                    type == Operation::TileType::HIGH_GROUND || 
                    type == Operation::TileType::SPAWN) {
                    
                    glm::vec2 snapPos = m_CurrentOperation->getTileCenterPos(r, c);
                    m_DraggedOperator->setPosition(snapPos);
                    LOG_DEBUG("Operator dropped at tile ({}, {}) type: {}", r, c, static_cast<int>(type));
                } else {
                    LOG_DEBUG("Illegal terrain at ({}, {}) type: {}", r, c, static_cast<int>(type));
                }
            }
            m_DraggedOperator = nullptr;
        }
    }
}

void GameScene::checkVictoryCondition() {
    // Victory: all waves spawned and all enemies defeated
    if (m_WaveManager && m_ActiveEnemies.empty()) {
        SpawnEvent dummyEvent;
        if (!m_WaveManager->shouldSpawn(m_WaveTimer + 10000.0f, dummyEvent)) {
            // No more enemies to spawn
            m_IsVictory = true;
            m_IsGameOver = true;
            LOG_INFO("VICTORY! All enemies defeated!");
            
            // TODO: Show victory screen
        }
    }
}

void GameScene::checkDefeatCondition() {
    if (m_BaseHP <= 0) {
        m_IsVictory = false;
        m_IsGameOver = true;
        LOG_INFO("DEFEAT! Base destroyed!");
        
        // TODO: Show defeat screen
    }
}

void GameScene::cleanup() {
    LOG_DEBUG("Cleaning up GameScene");
    m_ActiveEnemies.clear();
    m_Operators.clear();
    m_BattleBGM.reset();
}

void GameScene::onEnter() {
    LOG_DEBUG("Entering GameScene");
    if (m_BattleBGM) {
        m_BattleBGM->Play();
    }
}

void GameScene::onExit() {
    LOG_DEBUG("Exiting GameScene");
    if (m_BattleBGM) {
        m_BattleBGM->FadeOut(500);
    }
}

} // namespace Arknights
