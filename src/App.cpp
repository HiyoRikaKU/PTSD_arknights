#include "App.hpp"

#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Animation.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"

void App::Start() {
    LOG_TRACE("Start");

    // Login Page
    m_LoginPage = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/login_page.jpg"),
        10 // High Z-index to cover everything
    );
    // Scale login page to 1600x900
    m_LoginPage->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoginPage->GetScaledSize();
    m_Root.AddChild(m_LoginPage);

    // Login BGM
    m_LoginBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/login.mp3");
    m_LoginBGM->Play();

    // Login SFX
    // Note: Resources/login.mp3 exists, but login.wav might be missing. Using lobby.mp3 as a fallback if needed,
    // but keeping login.wav for now to avoid breaking other things if it's supposed to be there.
    m_LoginSFX = std::make_unique<Util::SFX>(std::string(RESOURCE_DIR) + "/SFX/login.wav");

    // Battle BGM
    m_BattleBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/battle/battle.mp3");

    // Map
    m_Map = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/map/operation1.jpg"),
        0
    );
    // Scale map to 1600x900
    m_Map->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_Map->GetScaledSize();
    m_Map->SetVisible(false);
    m_Root.AddChild(m_Map);

    // Text demonstration
    m_Text = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/../PTSD/assets/fonts/Inter.ttf", 48, "Operation Level: 0-2", Util::Color(255, 255, 255)),
        2 // Higher than Amiya
    );
    m_Text->m_Transform.translation = {0, 400}; // Top center
    m_Text->SetVisible(false);
    m_Root.AddChild(m_Text);

    // Build a simple grid for pathfinding (all walkable except spawn/goal markers).
    const int gridW = 10;
    const int gridH = 5;
    m_Grid.assign(gridH, std::vector<Node>(gridW));
    for (int y = 0; y < gridH; ++y) {
        for (int x = 0; x < gridW; ++x) {
            m_Grid[y][x].x = x;
            m_Grid[y][x].y = y;
            m_Grid[y][x].type = TileType::Walkable;
        }
    }
    // Place spawn on the left-middle, goal on the right-middle.
    m_SpawnNode = &m_Grid[gridH / 2][gridW - 1];
    m_SpawnNode->type = TileType::Spawn;
    m_GoalNode = &m_Grid[gridH / 2][0];
    m_GoalNode->type = TileType::Goal;

    m_Pathfinder = std::make_unique<Pathfinder>(m_Grid);

    // Preload enemy animations and pre-allocate the pool (no new/delete during gameplay).
    m_EnemyAnimationPaths.clear();
    for (int i = 1; i <= 25; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPaths.push_back(ss.str());
    }

    m_EnemyPool = std::make_unique<EnemyPool>(ENEMY_POOL_SIZE, m_EnemyAnimationPaths, m_Waypoints);

    // Register all pooled enemies to the renderer once; visibility toggles on spawn/despawn.
    for (const auto &handle : m_EnemyPool->GetRenderHandles()) {
        m_Root.AddChild(handle);
    }

    // Amiya Animation
    std::vector<std::string> amiyaIdle;
    for (int i = 1; i <= 31; ++i) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/operator/char_002_amiya/Idle_" << std::setfill('0') << std::setw(2) << i << ".png";
        amiyaIdle.push_back(ss.str());
    }

    m_Amiya = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Animation>(amiyaIdle, true, 50, true, 100),
        1 // Slightly higher than map
    );
    m_Amiya->SetVisible(false); // Hide Amiya initially
    m_Root.AddChild(m_Amiya);

    m_CurrentState = State::UPDATE;
}

void App::SpawnEnemy() {
    if (!m_EnemyPool) {
        return;
    }

    if (!m_Pathfinder || m_SpawnNode == nullptr || m_GoalNode == nullptr) {
        LOG_WARN("Pathfinder not initialized; spawn skipped");
        return;
    }

    std::vector<glm::vec2> tilePath = m_Pathfinder->FindPath(m_SpawnNode, m_GoalNode);
    if (tilePath.empty()) {
        LOG_WARN("No path found for enemy; spawn skipped");
        return;
    }

    std::vector<glm::vec2> worldPath;
    worldPath.reserve(tilePath.size());
    for (const auto &p : tilePath) {
        glm::vec2 mapOffset(550.0f, 230.0f); //圖片偏移
        worldPath.emplace_back(p.x * TILE_SIZE - mapOffset.x, p.y * TILE_SIZE - mapOffset.y);
    }

    Enemy *enemy = m_EnemyPool->GetEnemy();
    if (enemy == nullptr) {
        LOG_WARN("Enemy pool exhausted; spawn skipped");
        return;
    }

    const glm::vec2 start = worldPath.front();
    enemy->Spawn(start, worldPath, 100.0F, 0.2F);
    m_ActiveEnemies.push_back(enemy);
    LOG_DEBUG("Enemy spawned from pool and following preset path");
}

void App::Update() {
    
    if (!m_IsLoggedIn) {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) || 
            Util::Input::IsKeyDown(Util::Keycode::RETURN) ||
            Util::Input::IsKeyDown(Util::Keycode::KP_ENTER)) {
            m_IsLoggedIn = true;
            m_LoginPage->SetVisible(false);
            
            // Map shows after login
            m_Map->SetVisible(true);
            
            // Show text after login
            m_Text->SetVisible(true);
            
            // Amiya remains hidden as requested
            m_Amiya->SetVisible(false); 

            // Music transition
            m_LoginBGM->FadeOut(500);
            m_BattleBGM->Play();
            
            // Note: m_LoginSFX might fail if file doesn't exist
            try {
                m_LoginSFX->Play();
            } catch (...) {}
            
            LOG_DEBUG("Login successful!");
        }
    } else {
        // Enemy logic after login
        float deltaTime = Util::Time::GetDeltaTimeMs();
        m_SpawnTimer += deltaTime;

        if (m_SpawnTimer >= m_SpawnInterval) {
            SpawnEnemy();
            m_SpawnTimer = 0.0f; // Reset timer properly
        }

        for (std::size_t i = 0; i < m_ActiveEnemies.size();) {
            Enemy *enemy = m_ActiveEnemies[i];
            enemy->Update(deltaTime);

            if (!enemy->IsActive() || enemy->ReachedEnd()) {
                m_EnemyPool->ReturnEnemy(enemy);
                m_ActiveEnemies[i] = m_ActiveEnemies.back();
                m_ActiveEnemies.pop_back();
                LOG_DEBUG("Enemy returned to pool");
                continue;
            }
            ++i;
        }
    }

    m_Root.Update();
    
    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
