#ifndef ARKNIGHTS_APP_HPP
#define ARKNIGHTS_APP_HPP

#include <vector>
#include <string>
#include <memory>

#include "pch.hpp" // IWYU pragma: export
#include "Util/Renderer.hpp"
#include "Util/GameObject.hpp"
#include "Util/BGM.hpp"
#include "Util/SFX.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

#include "Arknights/Entity/Enemy.hpp"
#include "Arknights/Map/Operation.hpp"
#include "Arknights/Map/Operation1.hpp"
#include "Arknights/Entity/Operator.hpp"

namespace Arknights {

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    void start();
    void update();
    void end();
    void reset();

    State getCurrentState() const { return m_CurrentState; }

private:
    void validTask();
    void spawnEnemy(const SpawnEvent& event);

private:
    State m_CurrentState = State::START;

    Util::Renderer m_Root;

    std::shared_ptr<Util::GameObject> m_LoginPage;
    std::shared_ptr<Util::GameObject> m_AmiyaIcon;
    std::shared_ptr<Util::GameObject> m_ChenIcon;
    std::shared_ptr<Util::GameObject> m_AngelinaIcon;
    std::shared_ptr<Util::GameObject> m_RedIcon;
    std::shared_ptr<Util::GameObject> m_EyjafjallaIcon;
    std::shared_ptr<Util::GameObject> m_TexasIcon;
    std::shared_ptr<Util::GameObject> m_UmirinIcon;
    std::unique_ptr<Util::BGM> m_LoginBGM;
    std::unique_ptr<Util::SFX> m_LoginSFX;

    std::unique_ptr<Operation> m_CurrentOperation;
    std::unique_ptr<Util::BGM> m_BattleBGM;

    bool m_IsLoggedIn = false;

    std::unique_ptr<EnemyPool> m_EnemyPool;
    std::vector<Enemy*> m_ActiveEnemies; 
    std::vector<std::string> m_EnemyAnimationPathsGopro;
    std::vector<std::string> m_EnemyDiePathsGopro;
    std::vector<std::string> m_EnemyAnimationPathsBigbo;
    std::vector<std::string> m_EnemyDiePathsBigbo;
    std::vector<std::string> m_EnemyAnimationPathsTrslim;
    std::vector<std::string> m_EnemyDiePathsTrslim;
    
    float m_WaveTimer = 0.0f;
    int m_EscapedEnemies = 0;
    int m_KilledEnemies = 0;
    int m_TotalEnemies = 0;
    bool m_IsGameOver = false;

    float m_CurrentDP = 10.0f;
    float m_DPAccumulator = 0.0f;

    std::shared_ptr<Util::GameObject> m_EnemyCountText;
    std::shared_ptr<Util::GameObject> m_GameOverText;
    std::shared_ptr<Util::GameObject> m_RestartText;

    std::shared_ptr<Util::GameObject> m_DPText;
    std::shared_ptr<Util::GameObject> m_AmiyaCostText;
    std::shared_ptr<Util::GameObject> m_ChenCostText;
    std::shared_ptr<Util::GameObject> m_AngelinaCostText;
    std::shared_ptr<Util::GameObject> m_RedCostText;
    std::shared_ptr<Util::GameObject> m_EyjafjallaCostText;
    std::shared_ptr<Util::GameObject> m_TexasCostText;
    std::shared_ptr<Util::GameObject> m_UmirinCostText;

    std::vector<std::shared_ptr<Operator>> m_Operators;
    std::shared_ptr<Operator> m_DraggedOperator = nullptr;
    std::shared_ptr<Util::GameObject> m_DraggedIcon = nullptr;

    std::shared_ptr<Operator> m_ChoosingDirectionOperator = nullptr;
    std::shared_ptr<Util::GameObject> m_ChoosingDirectionIcon = nullptr;

    static constexpr std::size_t ENEMY_POOL_SIZE = 64;
    static constexpr float TILE_SIZE = 100.0F;
    static constexpr int MAX_ESCAPED_ENEMIES = 4;
};

} // namespace Arknights

#endif
