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

    // Operation
    const std::vector<glm::vec2> waypoints = {
        {570.0f, 120.0f}, // Spawn at red
        {337, 120.0f},
        {337, 20.0f},
        {70, 20.0f},
        {-70, 120.0f},
        {-340, 120.0f},
        {-360, 0.0f},
        {-622, -11.0f},
        {-570.0f, 120.0f} // Disappear at blue
    };
    m_CurrentOperation = std::make_unique<Operation>(std::string(RESOURCE_DIR) + "/map/operation1.jpg", waypoints);
    m_Root.AddChild(m_CurrentOperation->GetMap());

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
    for (const auto &handle : m_EnemyPool->GetRenderHandles()) {
        m_Root.AddChild(handle);
    }

    // Initialize WaveManager with timeline
    m_WaveManager = std::make_unique<WaveManager>();
    m_WaveManager->AddSpawnEvent(2000.0f, "gopro", 100.0f, 0.2f);
    m_WaveManager->AddSpawnEvent(4000.0f, "gopro", 100.0f, 0.2f);
    m_WaveManager->AddSpawnEvent(6000.0f, "bigbo", 300.0f, 0.1f);
    m_WaveManager->AddSpawnEvent(8000.0f, "bigbo", 300.0f, 0.1f);
    m_WaveManager->AddSpawnEvent(10000.0f, "gopro", 150.0f, 0.3f);

    m_CurrentState = State::UPDATE;
}

void App::SpawnEnemy(const SpawnEvent& event) {
    if (!m_EnemyPool || !m_CurrentOperation) {
        return;
    }

    Enemy *enemy = m_EnemyPool->GetEnemy();
    if (enemy == nullptr) {
        LOG_WARN("Enemy pool exhausted; spawn skipped");
        return;
    }

    // Set animation based on type
    if (event.enemyType == "gopro") {
        enemy->SetAnimation(m_EnemyAnimationPathsGopro);
    } else if (event.enemyType == "bigbo") {
        enemy->SetAnimation(m_EnemyAnimationPathsBigbo);
    }

    const auto& waypoints = m_CurrentOperation->GetWaypoints();
    const glm::vec2 start = waypoints.front();
    enemy->Spawn(start, waypoints, event.hp, event.speed);
    m_ActiveEnemies.push_back(enemy);
    LOG_DEBUG("Enemy spawned: type={}, hp={}, speed={}", event.enemyType, event.hp, event.speed);
}

void App::Update() {
    
    if (!m_IsLoggedIn) {
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) || 
            Util::Input::IsKeyDown(Util::Keycode::RETURN) ||
            Util::Input::IsKeyDown(Util::Keycode::KP_ENTER)) {
            m_IsLoggedIn = true;
            m_LoginPage->SetVisible(false);
            
            // Map shows after login
            if (m_CurrentOperation) {
                m_CurrentOperation->GetMap()->SetVisible(true);
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
            try {
                m_LoginSFX->Play();
            } catch (...) {}
            
            LOG_DEBUG("Login successful!");
        }
    } else {
        // Timeline-based enemy logic after login
        float deltaTime = Util::Time::GetDeltaTimeMs();
        m_WaveTimer += deltaTime;

        SpawnEvent event;
        while (m_WaveManager->ShouldSpawn(m_WaveTimer, event)) {
            SpawnEnemy(event);
        }

        // Update enemies
        for (std::size_t i = 0; i < m_ActiveEnemies.size();) {
            Enemy *enemy = m_ActiveEnemies[i];
            enemy->Update(deltaTime);

            if (!enemy->IsActive() || enemy->ReachedEnd()) {
                m_EnemyPool->ReturnEnemy(enemy);
                m_ActiveEnemies[i] = m_ActiveEnemies.back();
                m_ActiveEnemies.pop_back();
                continue;
            }
            ++i;
        }

        // Update operators
        for (auto& op : m_Operators) {
            op->Update(deltaTime);
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
        LOG_DEBUG("Mouse Position: {}, {}", Util::Input::GetCursorPosition().x, Util::Input::GetCursorPosition().y);
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
