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
    m_LoginPage->m_Transform.scale = glm::vec2(1600.0f, 900.0f) / m_LoginPage->GetScaledSize();
    m_Root.AddChild(m_LoginPage);

    // Login BGM
    m_LoginBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/login.mp3");
    m_LoginBGM->Play();

    // Login SFX
    m_LoginSFX = std::make_unique<Util::SFX>(std::string(RESOURCE_DIR) + "/SFX/login.wav");

    // Battle BGM
    m_BattleBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/battle/battle.mp3");

    m_CurrentOperation = std::make_unique<Operation1>();
    m_Root.AddChild(m_CurrentOperation->getMap());

    m_EnemyAnimationPathsGopro.clear();
    m_EnemyDiePathsGopro.clear();
    for (int i = 1; i <= 25; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsGopro.push_back(ss.str());
    }
    for (int i = 1; i <= 21; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1000_gopro/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyDiePathsGopro.push_back(ss.str());
    }

    m_EnemyAnimationPathsBigbo.clear();
    m_EnemyDiePathsBigbo.clear();
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1001_bigbo/Move_Loop_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsBigbo.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_1001_bigbo/Die_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyDiePathsBigbo.push_back(ss.str());
    }

    m_EnemyAnimationPathsTrslim.clear();
    m_EnemyDiePathsTrslim.clear();
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_10001_trslim/Move_A_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyAnimationPathsTrslim.push_back(ss.str());
    }
    for (int i = 1; i <= 31; i += 2) {
        std::stringstream ss;
        ss << RESOURCE_DIR << "/charactor/enemy/enemy_10001_trslim/Die_A_" << std::setfill('0') << std::setw(2) << i << ".png";
        m_EnemyDiePathsTrslim.push_back(ss.str());
    }

    m_EnemyPool = std::make_unique<EnemyPool>(ENEMY_POOL_SIZE, m_EnemyAnimationPathsGopro, m_CurrentOperation->getWaypoints());
    // We need to set die animations for pooled enemies, but EnemyPool doesn't know about them yet.
    // Let's just set them during spawn for now or update EnemyPool.
    // Actually, App::spawnEnemy is a good place.

    // Register all pooled enemies to the renderer once; visibility toggles on spawn/despawn.
    for (const auto &handle : m_EnemyPool->getRenderHandles()) {
        m_Root.AddChild(handle);
    }

    // Initialize operators (Amiya)
    auto amiya = std::make_shared<Amiya>();
    amiya->setPosition({0.0f, 120.0f}); // Default position
    amiya->SetVisible(false); // Hidden until deployed
    m_Operators.push_back(amiya);
    m_Root.AddChild(amiya);

    // Amiya Icon at bottom right
    m_AmiyaIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Amiya_icon.png"),
        5 // Above map, below login page
    );
    m_AmiyaIcon->m_Transform.translation = {700, -350}; // Bottom right
    m_AmiyaIcon->m_Transform.scale = {0.7f, 0.7f};
    m_AmiyaIcon->SetVisible(false); // Show after login
    m_Root.AddChild(m_AmiyaIcon);

    // Initialize Chen
    auto chen = std::make_shared<Chen>();
    chen->setPosition({0.0f, 0.0f});
    chen->SetVisible(false);
    m_Operators.push_back(chen);
    m_Root.AddChild(chen);

    // Chen Icon next to Amiya
    m_ChenIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Chen_icon.png"),
        5
    );
    m_ChenIcon->m_Transform.translation = {580, -350}; // Next to Amiya
    m_ChenIcon->m_Transform.scale = {0.7f, 0.7f};
    m_ChenIcon->SetVisible(false);
    m_Root.AddChild(m_ChenIcon);

    // Angelina
    auto angelina = std::make_shared<Angelina>();
    angelina->SetVisible(false);
    m_Operators.push_back(angelina);
    m_Root.AddChild(angelina);

    m_AngelinaIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Angelina_icon.png"),
        5
    );
    m_AngelinaIcon->m_Transform.translation = {460, -350};
    m_AngelinaIcon->m_Transform.scale = {0.7f, 0.7f};
    m_AngelinaIcon->SetVisible(false);
    m_Root.AddChild(m_AngelinaIcon);

    // Red
    auto red = std::make_shared<Red>();
    red->SetVisible(false);
    m_Operators.push_back(red);
    m_Root.AddChild(red);

    m_RedIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Projekt_Red_icon.png"),
        5
    );
    m_RedIcon->m_Transform.translation = {340, -350};
    m_RedIcon->m_Transform.scale = {0.7f, 0.7f};
    m_RedIcon->SetVisible(false);
    m_Root.AddChild(m_RedIcon);

    // Eyjafjalla
    auto eyja = std::make_shared<Eyjafjalla>();
    eyja->SetVisible(false);
    m_Operators.push_back(eyja);
    m_Root.AddChild(eyja);

    m_EyjafjallaIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Eyjafjalla_icon.png"),
        5
    );
    m_EyjafjallaIcon->m_Transform.translation = {220, -350};
    m_EyjafjallaIcon->m_Transform.scale = {0.7f, 0.7f};
    m_EyjafjallaIcon->SetVisible(false);
    m_Root.AddChild(m_EyjafjallaIcon);

    // Texas
    auto texas = std::make_shared<Texas>();
    texas->SetVisible(false);
    m_Operators.push_back(texas);
    m_Root.AddChild(texas);

    m_TexasIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/texas.png"),
        5
    );
    m_TexasIcon->m_Transform.translation = {100, -350};
    m_TexasIcon->m_Transform.scale = {0.7f, 0.7f};
    m_TexasIcon->SetVisible(false);
    m_Root.AddChild(m_TexasIcon);

    // Umirin
    auto umirin = std::make_shared<Umirin>();
    umirin->SetVisible(false);
    m_Operators.push_back(umirin);
    m_Root.AddChild(umirin);

    m_UmirinIcon = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/charactor/operator/Yahata_Umiri_icon.png"),
        5
    );
    m_UmirinIcon->m_Transform.translation = {-20, -350};
    m_UmirinIcon->m_Transform.scale = {0.7f, 0.7f};
    m_UmirinIcon->SetVisible(false);
    m_Root.AddChild(m_UmirinIcon);

    // Initialize enemy counters
    m_EscapedEnemies = 0;
    m_KilledEnemies = 0;
    m_IsGameOver = false;
    if (m_CurrentOperation) {
        m_TotalEnemies = static_cast<int>(m_CurrentOperation->getWaveManager().getTotalSpawnCount());
    }

    // Enemy count text
    m_EnemyCountText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 36, std::to_string(m_EscapedEnemies) + " / " + std::to_string(MAX_ESCAPED_ENEMIES), Util::Color(255, 255, 255)),
        2
    );
    m_EnemyCountText->m_Transform.translation = {0, 400}; // Top center
    m_EnemyCountText->SetVisible(false);
    m_Root.AddChild(m_EnemyCountText);

    // Game Over text
    m_GameOverText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 100, "", Util::Color(255, 0, 0)),
        10 // Above everything
    );
    m_GameOverText->m_Transform.translation = {0, 0}; // Center
    m_GameOverText->SetVisible(false);
    m_Root.AddChild(m_GameOverText);

    // Restart instructions
    m_RestartText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "Press R to Restart, M for Main Menu, ESC to Exit", Util::Color(200, 200, 200)),
        10
    );
    m_RestartText->m_Transform.translation = {0, -100};
    m_RestartText->SetVisible(false);
    m_Root.AddChild(m_RestartText);

    // DP UI
    m_CurrentDP = 10.0f;
    m_DPText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 40, "COST: 10", Util::Color(255, 255, 0)),
        2
    );
    m_DPText->m_Transform.translation = {700, -450};
    m_DPText->SetVisible(false);
    m_Root.AddChild(m_DPText);

    // Operator Cost Labels
    m_AmiyaCostText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "10", Util::Color(255, 255, 255)),
        6 // Above icon
    );
    m_AmiyaCostText->m_Transform.translation = {700, -350}; // Center-bottom of icon
    m_AmiyaCostText->SetVisible(false);
    m_Root.AddChild(m_AmiyaCostText);

    m_ChenCostText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "18", Util::Color(255, 255, 255)),
        6
    );
    m_ChenCostText->m_Transform.translation = {580, -350}; // Center-bottom of icon
    m_ChenCostText->SetVisible(false);
    m_Root.AddChild(m_ChenCostText);

    m_AngelinaCostText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "14", Util::Color(255, 255, 255)),
        6
    );
    m_AngelinaCostText->m_Transform.translation = {460, -350};
    m_AngelinaCostText->SetVisible(false);
    m_Root.AddChild(m_AngelinaCostText);

    m_RedCostText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "6", Util::Color(255, 255, 255)),
        6
    );
    m_RedCostText->m_Transform.translation = {340, -350};
    m_RedCostText->SetVisible(false);
    m_Root.AddChild(m_RedCostText);

    m_EyjafjallaCostText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "20", Util::Color(255, 255, 255)),
        6
    );
    m_EyjafjallaCostText->m_Transform.translation = {220, -350};
    m_EyjafjallaCostText->SetVisible(false);
    m_Root.AddChild(m_EyjafjallaCostText);

    m_TexasCostText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "12", Util::Color(255, 255, 255)),
        6
    );
    m_TexasCostText->m_Transform.translation = {100, -350};
    m_TexasCostText->SetVisible(false);
    m_Root.AddChild(m_TexasCostText);

    m_UmirinCostText = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 30, "10", Util::Color(255, 255, 255)),
        6
    );
    m_UmirinCostText->m_Transform.translation = {-20, -350};
    m_UmirinCostText->SetVisible(false);
    m_Root.AddChild(m_UmirinCostText);

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
        enemy->setDieAnimation(m_EnemyDiePathsGopro);
        enemy->setAttack(10.0f);
        enemy->setAttackInterval(1000.0f);
    } else if (event.enemyType == "bigbo") {
        enemy->setAnimation(m_EnemyAnimationPathsBigbo);
        enemy->setDieAnimation(m_EnemyDiePathsBigbo);
        enemy->setAttack(30.0f);
        enemy->setAttackInterval(1500.0f);
    } else if (event.enemyType == "trslim") {
        enemy->setAnimation(m_EnemyAnimationPathsTrslim);
        enemy->setDieAnimation(m_EnemyDiePathsTrslim);
        enemy->setAttack(15.0f);
        enemy->setAttackInterval(800.0f);
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
            
            // Show operators after login (only those already on map if any, but currently all are hidden)
            /*
            for (auto& op : m_Operators) {
                // op->SetVisible(true); // Don't show them immediately if they are not deployed
            }
            */
            m_AmiyaIcon->SetVisible(true);
            m_ChenIcon->SetVisible(true);
            m_AngelinaIcon->SetVisible(true);
            m_RedIcon->SetVisible(true);
            m_EyjafjallaIcon->SetVisible(true);
            m_TexasIcon->SetVisible(true);
            m_UmirinIcon->SetVisible(true);
            m_EnemyCountText->SetVisible(true);
            m_DPText->SetVisible(true);
            m_AmiyaCostText->SetVisible(true);
            m_ChenCostText->SetVisible(true);
            m_AngelinaCostText->SetVisible(true);
            m_RedCostText->SetVisible(true);
            m_EyjafjallaCostText->SetVisible(true);
            m_TexasCostText->SetVisible(true);
            m_UmirinCostText->SetVisible(true);

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
        if (m_IsGameOver) {
            m_GameOverText->SetVisible(true);
            m_RestartText->SetVisible(true);

            if (Util::Input::IsKeyDown(Util::Keycode::R)) {
                reset();
            } else if (Util::Input::IsKeyDown(Util::Keycode::M)) {
                reset();
                m_IsLoggedIn = false;
                m_LoginPage->SetVisible(true);
                m_CurrentOperation->getMap()->SetVisible(false);
                m_AmiyaIcon->SetVisible(false);
                m_ChenIcon->SetVisible(false);
                m_EnemyCountText->SetVisible(false);
                m_DPText->SetVisible(false);
                m_AmiyaCostText->SetVisible(false);
                m_ChenCostText->SetVisible(false);
                m_BattleBGM->FadeOut(500);
                m_LoginBGM->Play();
            } else if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
                m_CurrentState = State::END;
            }
            m_Root.Update();
            return;
        }

        // Timeline-based enemy logic after login
        float deltaTime = Util::Time::GetDeltaTimeMs();
        m_WaveTimer += deltaTime;

        // DP Generation (1 point per second)
        m_DPAccumulator += deltaTime;
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

        // Update enemies
        for (std::size_t i = 0; i < m_ActiveEnemies.size();) {
            Enemy *enemy = m_ActiveEnemies[i];
            enemy->update(deltaTime);

            if (!enemy->isActive()) {
                if (enemy->reachedEnd()) {
                    m_EscapedEnemies++;
                    LOG_INFO("Enemy escaped! Escaped: {}", m_EscapedEnemies);
                } else {
                    m_KilledEnemies++;
                    LOG_INFO("Enemy killed! Killed: {}", m_KilledEnemies);
                }
                
                m_EnemyPool->returnEnemy(enemy);
                m_ActiveEnemies[i] = m_ActiveEnemies.back();
                m_ActiveEnemies.pop_back();

                // Update text
                auto enemyCountDrawable = std::dynamic_pointer_cast<Util::Text>(m_EnemyCountText->GetDrawable());
                enemyCountDrawable->SetText(std::to_string(m_EscapedEnemies) + " / " + std::to_string(MAX_ESCAPED_ENEMIES));
                
                continue;
            }
            ++i;
        }

        // Check for Win/Loss
        if (m_EscapedEnemies >= MAX_ESCAPED_ENEMIES) {
            m_IsGameOver = true;
            m_GameOverText->SetVisible(true);
            auto gameOverDrawable = std::dynamic_pointer_cast<Util::Text>(m_GameOverText->GetDrawable());
            gameOverDrawable->SetText("MISSION FAILED");
            gameOverDrawable->SetColor(Util::Color(255, 0, 0));
            LOG_INFO("Mission Failed! Too many escaped enemies.");
        } else if (m_CurrentOperation->getWaveManager().isAllSpawned() && m_ActiveEnemies.empty()) {
            m_IsGameOver = true;
            m_GameOverText->SetVisible(true);
            auto gameOverDrawable = std::dynamic_pointer_cast<Util::Text>(m_GameOverText->GetDrawable());
            gameOverDrawable->SetText("MISSION ACCOMPLISHED");
            gameOverDrawable->SetColor(Util::Color(0, 255, 0));
            LOG_INFO("Mission Accomplished! All enemies processed.");
        }

        // Reset blocking states and re-evaluate
        for (auto& op : m_Operators) {
            op->clearBlockedEnemies();
        }
        for (auto& enemy : m_ActiveEnemies) {
            enemy->setBlocked(false);
        }

        // Update operators and check for dragging
        glm::vec2 mousePos = Util::Input::GetCursorPosition();
        if (m_ChoosingDirectionOperator) {
            // In choosing direction mode, look at mouse position relative to operator
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
                LOG_DEBUG("Direction finalized");
            }
        } else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            // Check Amiya icon click
            if (m_AmiyaIcon->GetVisible() && glm::distance(mousePos, m_AmiyaIcon->m_Transform.translation) < 60.0f) {
                if (m_Operators.size() >= 1 && m_CurrentDP >= m_Operators[0]->getDeploymentCost()) {
                    m_DraggedOperator = m_Operators[0];
                    m_DraggedIcon = m_AmiyaIcon;
                    m_DraggedOperator->SetVisible(true);
                    m_DraggedIcon->SetVisible(false);
                    LOG_DEBUG("Starting drag Amiya from icon");
                }
            } 
            // Check Chen icon click
            else if (m_ChenIcon->GetVisible() && glm::distance(mousePos, m_ChenIcon->m_Transform.translation) < 60.0f) {
                if (m_Operators.size() >= 2 && m_CurrentDP >= m_Operators[1]->getDeploymentCost()) {
                    m_DraggedOperator = m_Operators[1];
                    m_DraggedIcon = m_ChenIcon;
                    m_DraggedOperator->SetVisible(true);
                    m_DraggedIcon->SetVisible(false);
                    LOG_DEBUG("Starting drag Chen from icon");
                }
            }
            // Angelina
            else if (m_AngelinaIcon->GetVisible() && glm::distance(mousePos, m_AngelinaIcon->m_Transform.translation) < 60.0f) {
                if (m_Operators.size() >= 3 && m_CurrentDP >= m_Operators[2]->getDeploymentCost()) {
                    m_DraggedOperator = m_Operators[2];
                    m_DraggedIcon = m_AngelinaIcon;
                    m_DraggedOperator->SetVisible(true);
                    m_DraggedIcon->SetVisible(false);
                }
            }
            // Red
            else if (m_RedIcon->GetVisible() && glm::distance(mousePos, m_RedIcon->m_Transform.translation) < 60.0f) {
                if (m_Operators.size() >= 4 && m_CurrentDP >= m_Operators[3]->getDeploymentCost()) {
                    m_DraggedOperator = m_Operators[3];
                    m_DraggedIcon = m_RedIcon;
                    m_DraggedOperator->SetVisible(true);
                    m_DraggedIcon->SetVisible(false);
                }
            }
            // Eyja
            else if (m_EyjafjallaIcon->GetVisible() && glm::distance(mousePos, m_EyjafjallaIcon->m_Transform.translation) < 60.0f) {
                if (m_Operators.size() >= 5 && m_CurrentDP >= m_Operators[4]->getDeploymentCost()) {
                    m_DraggedOperator = m_Operators[4];
                    m_DraggedIcon = m_EyjafjallaIcon;
                    m_DraggedOperator->SetVisible(true);
                    m_DraggedIcon->SetVisible(false);
                }
            }
            // Texas
            else if (m_TexasIcon->GetVisible() && glm::distance(mousePos, m_TexasIcon->m_Transform.translation) < 60.0f) {
                if (m_Operators.size() >= 6 && m_CurrentDP >= m_Operators[5]->getDeploymentCost()) {
                    m_DraggedOperator = m_Operators[5];
                    m_DraggedIcon = m_TexasIcon;
                    m_DraggedOperator->SetVisible(true);
                    m_DraggedIcon->SetVisible(false);
                }
            }
            // Umirin
            else if (m_UmirinIcon->GetVisible() && glm::distance(mousePos, m_UmirinIcon->m_Transform.translation) < 60.0f) {
                if (m_Operators.size() >= 7 && m_CurrentDP >= m_Operators[6]->getDeploymentCost()) {
                    m_DraggedOperator = m_Operators[6];
                    m_DraggedIcon = m_UmirinIcon;
                    m_DraggedOperator->SetVisible(true);
                    m_DraggedIcon->SetVisible(false);
                }
            }
            else {
                // Check existing operators on map for RECALL
                for (std::size_t i = 0; i < m_Operators.size(); ++i) {
                    auto& op = m_Operators[i];
                    if (op->GetVisible() && glm::distance(mousePos, op->getPosition()) < 50.0f) {
                        op->SetVisible(false);
                        // Return to icon state
                        if (i == 0) m_AmiyaIcon->SetVisible(true);
                        else if (i == 1) m_ChenIcon->SetVisible(true);
                        else if (i == 2) m_AngelinaIcon->SetVisible(true);
                        else if (i == 3) m_RedIcon->SetVisible(true);
                        else if (i == 4) m_EyjafjallaIcon->SetVisible(true);
                        else if (i == 5) m_TexasIcon->SetVisible(true);
                        else if (i == 6) m_UmirinIcon->SetVisible(true);
                        LOG_DEBUG("Operator recalled");
                        break;
                    }
                }
            }
        }
                if (m_DraggedOperator) {
            m_DraggedOperator->setPosition(mousePos);
            if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB)) {
                // Implement onDrop
                int r, c;
                bool dropped = false;
                if (m_CurrentOperation->getTileIndices(mousePos, r, c)) {
                    Operation::TileType type = m_CurrentOperation->getTileType(r, c);
                    
                    bool canPlace = false;
                    if (m_DraggedOperator->getType() == Operator::Type::AMIYA ||
                        m_DraggedOperator->getType() == Operator::Type::ANGELINA ||
                        m_DraggedOperator->getType() == Operator::Type::EYJAFJALLA ||
                        m_DraggedOperator->getType() == Operator::Type::UMIRIN) {
                        canPlace = (type == Operation::TileType::HIGH_GROUND);
                    } else if (m_DraggedOperator->getType() == Operator::Type::CHEN ||
                               m_DraggedOperator->getType() == Operator::Type::RED ||
                               m_DraggedOperator->getType() == Operator::Type::TEXAS) {
                        canPlace = (type == Operation::TileType::GROUND || type == Operation::TileType::SPAWN);
                    }

                    if (canPlace) {
                        glm::vec2 snapPos = m_CurrentOperation->getTileCenterPos(r, c);
                        m_DraggedOperator->setPosition(snapPos);
                        // Store the tile center in grid coordinates for distance-based logic
                        m_DraggedOperator->setGridPosition({static_cast<float>(r) + 0.5f, static_cast<float>(c) + 0.5f});
                        LOG_DEBUG("Operator dropped at tile ({}, {}) type: {}", r, c, static_cast<int>(type));
                        dropped = true;

                        // Deduct DP
                        m_CurrentDP -= m_DraggedOperator->getDeploymentCost();
                        auto dpTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_DPText->GetDrawable());
                        dpTextDrawable->SetText("COST: " + std::to_string(static_cast<int>(m_CurrentDP)));

                        // Enter choosing direction mode
                        m_ChoosingDirectionOperator = m_DraggedOperator;
                        m_ChoosingDirectionIcon = m_DraggedIcon;
                    } else {
                        LOG_DEBUG("Illegal terrain for this operator at ({}, {}) type: {}", r, c, static_cast<int>(type));
                    }
                }
                
                if (!dropped && m_DraggedIcon) {
                    // Return to icon state if dropped in illegal location and was dragged from icon
                    m_DraggedOperator->SetVisible(false);
                    m_DraggedIcon->SetVisible(true);
                }
                m_DraggedOperator = nullptr;
                m_DraggedIcon = nullptr;
            }
        }

        for (auto& op : m_Operators) {
            if (!op->GetVisible() || !op->isAlive()) continue;
            op->update(deltaTime);

            // 1. Check for starting a new attack
            if (op->canAttack()) {
                // Find if there's any enemy in range to start animation
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
                    LOG_DEBUG("Operator started attack animation");
                }
            }

            // 2. Check for applying damage (triggered by timer matching animation)
            if (op->consumeDamageTrigger()) {
                // Apply damage to enemies in range
                // For simplicity, we apply to the first N enemies found in range (where N is attack count)
                // or just follow the previous logic: one attack cycle damages one or more enemies based on attackCount
                
                // Arknights style: usually single target unless specified. 
                // Previous logic was: damage ONE enemy with 'attackCount' hits.
                
                Enemy* target = nullptr;
                // Prioritize blocked enemies
                for (auto& enemy : m_ActiveEnemies) {
                    if (enemy->isAlive() && enemy->getTargetOperator() == op.get() && op->isInAttackRange(enemy->getGridPosition())) {
                        target = enemy;
                        break;
                    }
                }
                // If no blocked enemy, find any in range
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
                    LOG_DEBUG("Operator damage applied to enemy");
                }
            }

            // 3. Check for blocking (only on same tile)
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
    }

    if (Util::Input::IsKeyDown(Util::Keycode::L)) {
        LOG_DEBUG("Mouse Position: {}, {}", Util::Input::GetCursorPosition().x, Util::Input::GetCursorPosition().y);
    }

    // Sync cost label visibility with icons
    m_AmiyaCostText->SetVisible(m_AmiyaIcon->GetVisible());
    m_ChenCostText->SetVisible(m_ChenIcon->GetVisible());
    m_AngelinaCostText->SetVisible(m_AngelinaIcon->GetVisible());
    m_RedCostText->SetVisible(m_RedIcon->GetVisible());
    m_EyjafjallaCostText->SetVisible(m_EyjafjallaIcon->GetVisible());
    m_TexasCostText->SetVisible(m_TexasIcon->GetVisible());
    m_UmirinCostText->SetVisible(m_UmirinIcon->GetVisible());

    m_Root.Update();
    
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::end() { 
    LOG_TRACE("End");
}

void App::reset() {
    m_IsGameOver = false;
    m_WaveTimer = 0.0f;
    m_EscapedEnemies = 0;
    m_KilledEnemies = 0;

    m_CurrentDP = 10.0f;
    m_DPAccumulator = 0.0f;
    auto dpTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_DPText->GetDrawable());
    dpTextDrawable->SetText("COST: " + std::to_string(static_cast<int>(m_CurrentDP)));
    
    m_GameOverText->SetVisible(false);
    m_RestartText->SetVisible(false);
    
    // Clear active enemies
    for (auto* enemy : m_ActiveEnemies) {
        m_EnemyPool->returnEnemy(enemy);
    }
    m_ActiveEnemies.clear();

    // Reset operators
    for (auto& op : m_Operators) {
        op->SetVisible(false);
        op->reset();
    }
    m_AmiyaIcon->SetVisible(true);
    m_ChenIcon->SetVisible(true);
    m_AngelinaIcon->SetVisible(true);
    m_RedIcon->SetVisible(true);
    m_EyjafjallaIcon->SetVisible(true);
    m_TexasIcon->SetVisible(true);
    m_UmirinIcon->SetVisible(true);
    
    // Reset text
    auto enemyCountDrawable = std::dynamic_pointer_cast<Util::Text>(m_EnemyCountText->GetDrawable());
    enemyCountDrawable->SetText(std::to_string(m_EscapedEnemies) + " / " + std::to_string(MAX_ESCAPED_ENEMIES));

    // Reset wave manager
    m_CurrentOperation->getWaveManager().reset();

    m_BattleBGM->Play();

    LOG_DEBUG("Game reset");
}

} // namespace Arknights
