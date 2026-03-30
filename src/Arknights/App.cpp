#include "Arknights/App.hpp"

#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

namespace Arknights {

void App::start() {
    LOG_TRACE("Start");

    // Login Page
    m_LoginPage = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/login_page.jpg"),
        10 // High Z-index to cover everything
    );
    // Scale login page to 1600x900
    m_LoginPage->m_Transform.scale = glm::vec2(1000.0f, 300.0f) / m_LoginPage->GetScaledSize();
    m_Root.AddChild(m_LoginPage);

    // Login BGM
    m_LoginBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/login.mp3");
    m_LoginBGM->Play();

    // Login SFX
    m_LoginSFX = std::make_unique<Util::SFX>(std::string(RESOURCE_DIR) + "/SFX/login.wav");

    // Battle BGM
    m_BattleBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/battle/battle.mp3");

    // Enemy Trace: left left, down, left, up-left, left left, down, left left
    // Relative format: {x, y} where +x is right, +y is up
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

    std::vector<glm::vec2> waypoints;
    waypoints.push_back(spawnPos);
    glm::vec2 currentPos = spawnPos;
    for (const auto& s : steps) {
        currentPos.x -= s.y; // +y is Up -> Row decreases
        currentPos.y += s.x; // +x is Right -> Col increases
        waypoints.push_back(currentPos);
    }
    
    m_CurrentOperation = std::make_unique<Operation>(std::string(RESOURCE_DIR) + "/map/operation1.jpg", waypoints);
    m_Root.AddChild(m_CurrentOperation->getMap());

    // Text demonstration
    m_Text = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 48, "Operation Level: 0-2", Util::Color(255, 255, 255)),
        2 // Higher than map and enemies
    );
    m_Text->m_Transform.translation = {0, 400}; // Top center
    m_Text->SetVisible(false);
    m_Root.AddChild(m_Text);

    // Preload enemy animations
    m_EnemyAnimationPathsGopro.clear();
    for (int i = 1; i <= 25; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsGopro.push_back(ss.str());
    }

    m_EnemyAnimationPathsBigbo.clear();
    for (int i = 1; i <= 31; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1001_bigbo/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsBigbo.push_back(ss.str());
    }

    m_EnemyPool = std::make_unique<EnemyPool>(ENEMY_POOL_SIZE, m_EnemyAnimationPathsGopro, waypoints);

    // Register all pooled enemies to the renderer once; visibility toggles on spawn/despawn.
    for (const auto &handle : m_EnemyPool->getRenderHandles()) {
        m_Root.AddChild(handle);
    }

    // Initialize operators (Amiya)
    std::vector<std::string> amiyaIdlePaths;
    for (int i = 1; i <= 31; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/char_002_amiya/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        amiyaIdlePaths.push_back(ss.str());
    }
    auto amiya = std::make_shared<Operator>(amiyaIdlePaths, 1000.0f, 100.0f);
    amiya->setPosition({0.0f, 120.0f}); // On the path
    amiya->m_Transform.scale = glm::vec2(-0.3f, 0.3f); // Match enemy scale and face left
    amiya->SetVisible(false);
    m_Operators.push_back(amiya);
    m_Root.AddChild(amiya);

    // Initialize WaveManager with timeline (Speed in grid units per ms)
    m_WaveManager = std::make_unique<WaveManager>();
    m_WaveManager->addSpawnEvent(2000.0f, "gopro", 100.0f, 0.001f);
    m_WaveManager->addSpawnEvent(4000.0f, "gopro", 100.0f, 0.001f);
    m_WaveManager->addSpawnEvent(6000.0f, "bigbo", 300.0f, 0.0005f);
    m_WaveManager->addSpawnEvent(8000.0f, "bigbo", 300.0f, 0.0005f);
    m_WaveManager->addSpawnEvent(10000.0f, "gopro", 150.0f, 0.001f);

    m_CurrentState = State::UPDATE;
}

void App::spawnEnemy(const SpawnEvent& event) {
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

void App::update() {
    
    if (!m_IsLoggedIn) {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) || 
            Util::Input::IsKeyDown(Util::Keycode::RETURN) ||
            Util::Input::IsKeyDown(Util::Keycode::KP_ENTER)) {
            m_IsLoggedIn = true;
            m_LoginPage->SetVisible(false);
            
            // Map shows after login
            if (m_CurrentOperation) {
                m_CurrentOperation->getMap()->SetVisible(true);
            }
            
            // Show text after login
            m_Text->SetVisible(true);
            
            // Show operators after login
            for (auto& op : m_Operators) {
                op->SetVisible(true);
            }

            // Music transition
            m_LoginBGM->FadeOut(500);
            m_BattleBGM->Play();
            
            // Note: m_LoginSFX might fail if file doesn't exist
            if (m_LoginSFX) {
                try {
                    m_LoginSFX->Play();
                } catch (...) {}
            }
            
            LOG_DEBUG("Login successful!");
        }
    } else {
        // Timeline-based enemy logic after login
        float deltaTime = Util::Time::GetDeltaTimeMs();
        m_WaveTimer += deltaTime;

        SpawnEvent event;
        while (m_WaveManager->shouldSpawn(m_WaveTimer, event)) {
            spawnEnemy(event);
        }

        // Update enemies
        for (std::size_t i = 0; i < m_ActiveEnemies.size();) {
            Enemy *enemy = m_ActiveEnemies[i];
            enemy->update(deltaTime);

            if (!enemy->isActive() || enemy->reachedEnd()) {
                m_EnemyPool->returnEnemy(enemy);
                m_ActiveEnemies[i] = m_ActiveEnemies.back();
                m_ActiveEnemies.pop_back();
                continue;
            }
            ++i;
        }

        // Reset blocking states and re-evaluate
        for (auto& op : m_Operators) {
            op->clearBlockedEnemies();
        }
        for (auto& enemy : m_ActiveEnemies) {
            enemy->setBlocked(false);
        }

        // Update operators and check for blocking
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            for (auto& op : m_Operators) {
                if (glm::distance(mousePos, op->getPosition()) < 50.0f) {
                    m_DraggedOperator = op;
                    break;
                }
            }
        }
        if (m_DraggedOperator) {
            m_DraggedOperator->setPosition(mousePos);
            if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB)) {
                // Implement onDrop
                int r, c;
                if (m_CurrentOperation->getTileIndices(mousePos, r, c)) {
                    Operation::TileType type = m_CurrentOperation->getTileType(r, c);
                    // Check terrain legality: ground, high ground, and spawn (for testing) are allowed
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

        constexpr float BLOCK_DISTANCE = 50.0f;
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

    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
        LOG_DEBUG("Mouse Position: {}, {}", Util::Input::GetCursorPosition().x, Util::Input::GetCursorPosition().y);
    }

    m_Root.Update();
    
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::end() { 
    LOG_TRACE("End");
}

} // namespace Arknights
