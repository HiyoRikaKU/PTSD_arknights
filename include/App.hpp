#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export
#include "Util/Renderer.hpp"
#include "Util/GameObject.hpp"

#include "Util/BGM.hpp"
#include "Util/SFX.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Enemy.hpp"
#include "EnemyPool.hpp"
#include "Operation.hpp"
#include "WaveManager.hpp"
#include "Operator.hpp"

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    void ValidTask();
    void SpawnEnemy(const SpawnEvent& event);

private:
    State m_CurrentState = State::START;

    Util::Renderer m_Root;

    std::shared_ptr<Util::GameObject> m_LoginPage;
    std::unique_ptr<Util::BGM> m_LoginBGM;
    std::unique_ptr<Util::SFX> m_LoginSFX;

    std::unique_ptr<Operation> m_CurrentOperation;
    std::unique_ptr<Util::BGM> m_BattleBGM;

    std::shared_ptr<Util::GameObject> m_Text;

    bool m_IsLoggedIn = false;

    std::unique_ptr<EnemyPool> m_EnemyPool;
    std::vector<Enemy*> m_ActiveEnemies; // Active list (O(1) remove via swap-pop)
    std::vector<std::string> m_EnemyAnimationPathsGopro;
    std::vector<std::string> m_EnemyAnimationPathsBigbo;
    
    std::unique_ptr<WaveManager> m_WaveManager;
    float m_WaveTimer = 0.0f;

    std::vector<std::shared_ptr<Operator>> m_Operators;

    static constexpr std::size_t ENEMY_POOL_SIZE = 32;
    static constexpr float TILE_SIZE = 100.0F;
};

#endif
