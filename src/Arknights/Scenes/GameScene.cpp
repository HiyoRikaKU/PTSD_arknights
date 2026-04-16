#include "Arknights/Scenes/GameScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Arknights/Map/Operation1.hpp"
#include "Arknights/Scenes/LobbyScene.hpp"
#include "config.hpp"

#include <iomanip>
#include <sstream>

namespace Arknights {


GameScene::GameScene() {
}

void GameScene::init() {
    LOG_DEBUG("Initializing GameScene");
    
    // 1. Initialize Operation
    m_CurrentOperation = std::make_unique<Operation1>();
    m_CurrentOperation->getMap()->SetVisible(true);
    m_Root.AddChild(m_CurrentOperation->getMap());

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

    m_MissionCompletedImage = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/mission_completed.jpg"), 11);
    m_MissionCompletedImage->m_Transform.translation = {-1200.0f, 0.0f};
    m_MissionCompletedImage->m_Transform.scale = {0.52f, 0.52f};
    m_MissionCompletedImage->SetVisible(false);
    m_Root.AddChild(m_MissionCompletedImage);

    m_YourWinImage = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin.jpg"), 11);
    m_YourWinImage->m_Transform.translation = {0.0f, 0.0f};
    m_YourWinImage->m_Transform.scale = {0.53f, 0.53f};
    m_YourWinImage->SetVisible(false);
    m_Root.AddChild(m_YourWinImage);

    m_MissionFailedImage = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/mission_failed.jpg"), 11);
    m_MissionFailedImage->m_Transform.translation = {0.0f, 0.0f};
    m_MissionFailedImage->m_Transform.scale = {0.56f, 0.56f};
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

    // 7. Battle BGM
    m_BattleBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/battle/battle.mp3");

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
    }

    enemy->spawn(m_CurrentOperation->getWaypoints(), event.hp, event.speed, m_CurrentOperation->getHomography());
    m_ActiveEnemies.push_back(enemy);
}

void GameScene::update(float deltaTime) {
    // Cap deltaTime to prevent huge jumps after loading or lag spikes
    if (deltaTime > 100.0f) {
        deltaTime = 16.666f;
    }

    if (m_IsGameOver) {
        if (m_ResultPhase == ResultPhase::VICTORY_SLIDE) {
            constexpr float kSlideDurationMs = 2000.0f;
            const float halfWindowWidth = static_cast<float>(WINDOW_WIDTH) * 0.5f;
            const float halfImageWidth = m_MissionCompletedImage->GetScaledSize().x * 0.5f;
            const float kStartX = -halfWindowWidth - halfImageWidth;
            const float kEndX = halfWindowWidth - halfImageWidth;
            m_ResultTimer += deltaTime;
            const float t = std::min(1.0f, m_ResultTimer / kSlideDurationMs);
            m_MissionCompletedImage->m_Transform.translation = {kStartX + (kEndX - kStartX) * t, 0.0f};
            if (t >= 1.0f) {
                m_ResultPhase = ResultPhase::VICTORY_WAIT_BEFORE_WIN;
                m_ResultTimer = 0.0f;
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
                Core::SceneManager::getInstance().replaceScene(std::make_shared<LobbyScene>());
            }
        }
        return;
    }

    m_WaveTimer += deltaTime;

    // DP Generation
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

    // Update Enemies
    for (std::size_t i = 0; i < m_ActiveEnemies.size();) {
        Enemy *enemy = m_ActiveEnemies[i];
        enemy->update(deltaTime);

        if (!enemy->isActive()) {
            if (enemy->reachedEnd()) {
                m_EscapedEnemies++;
            } else {
                m_KilledEnemies++;
            }
            m_EnemyPool->returnEnemy(enemy);
            m_ActiveEnemies[i] = m_ActiveEnemies.back();
            m_ActiveEnemies.pop_back();

            auto enemyCountDrawable = std::dynamic_pointer_cast<Util::Text>(m_EnemyCountText->GetDrawable());
            enemyCountDrawable->SetText(std::to_string(m_EscapedEnemies) + " / " + std::to_string(MAX_ESCAPED_ENEMIES));
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
        op->update(deltaTime);
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
            if (m_Operators.size() >= 1 && m_CurrentDP >= m_Operators[0]->getDeploymentCost()) {
                m_DraggedOperator = m_Operators[0]; m_DraggedIcon = m_AmiyaIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_ChenIcon->GetVisible() && glm::distance(mousePos, m_ChenIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 2 && m_CurrentDP >= m_Operators[1]->getDeploymentCost()) {
                m_DraggedOperator = m_Operators[1]; m_DraggedIcon = m_ChenIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_AngelinaIcon->GetVisible() && glm::distance(mousePos, m_AngelinaIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 3 && m_CurrentDP >= m_Operators[2]->getDeploymentCost()) {
                m_DraggedOperator = m_Operators[2]; m_DraggedIcon = m_AngelinaIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_RedIcon->GetVisible() && glm::distance(mousePos, m_RedIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 4 && m_CurrentDP >= m_Operators[3]->getDeploymentCost()) {
                m_DraggedOperator = m_Operators[3]; m_DraggedIcon = m_RedIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_EyjafjallaIcon->GetVisible() && glm::distance(mousePos, m_EyjafjallaIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 5 && m_CurrentDP >= m_Operators[4]->getDeploymentCost()) {
                m_DraggedOperator = m_Operators[4]; m_DraggedIcon = m_EyjafjallaIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_TexasIcon->GetVisible() && glm::distance(mousePos, m_TexasIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 6 && m_CurrentDP >= m_Operators[5]->getDeploymentCost()) {
                m_DraggedOperator = m_Operators[5]; m_DraggedIcon = m_TexasIcon;
                m_DraggedOperator->SetVisible(true); m_DraggedIcon->SetVisible(false);
            }
        } else if (m_UmirinIcon->GetVisible() && glm::distance(mousePos, m_UmirinIcon->m_Transform.translation) < 60.0f) {
            if (m_Operators.size() >= 7 && m_CurrentDP >= m_Operators[6]->getDeploymentCost()) {
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
                    m_CurrentDP -= m_DraggedOperator->getDeploymentCost();
                    auto dpTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_DPText->GetDrawable());
                    dpTextDrawable->SetText("COST: " + std::to_string(static_cast<int>(m_CurrentDP)));
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
    auto enemyCountDrawable = std::dynamic_pointer_cast<Util::Text>(m_EnemyCountText->GetDrawable());
    enemyCountDrawable->SetText("0 / 4");
    m_CurrentOperation->getWaveManager().reset();
    m_BattleBGM->Play();
}

void GameScene::beginVictorySequence() {
    m_IsGameOver = true;
    m_IsVictory = true;
    m_ResultPhase = ResultPhase::VICTORY_SLIDE;
    m_ResultTimer = 0.0f;
    cleanupCharactersForResult();
    const float halfWindowWidth = static_cast<float>(WINDOW_WIDTH) * 0.5f;
    const float halfImageWidth = m_MissionCompletedImage->GetScaledSize().x * 0.5f;
    m_MissionCompletedImage->m_Transform.translation = {-halfWindowWidth - halfImageWidth, 0.0f};
    m_MissionCompletedImage->SetVisible(true);
    m_YourWinImage->SetVisible(false);
    m_MissionFailedImage->SetVisible(false);
}

void GameScene::beginFailureSequence() {
    m_IsGameOver = true;
    m_IsVictory = false;
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
}

void GameScene::onEnter() {
    LOG_DEBUG("Entering GameScene");
    if (m_BattleBGM) m_BattleBGM->Play();
}

void GameScene::onExit() {
    LOG_DEBUG("Exiting GameScene");
    if (m_BattleBGM) m_BattleBGM->FadeOut(500);
}

} // namespace Arknights
