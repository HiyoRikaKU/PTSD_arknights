#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export
#include "Util/Renderer.hpp"
#include "Util/GameObject.hpp"

#include "Util/BGM.hpp"
#include "Util/SFX.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"

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

private:
    State m_CurrentState = State::START;

    Util::Renderer m_Root;
    std::shared_ptr<Util::GameObject> m_Amiya;

    std::shared_ptr<Util::GameObject> m_LoginPage;
    std::unique_ptr<Util::BGM> m_LoginBGM;
    std::unique_ptr<Util::SFX> m_LoginSFX;

    std::shared_ptr<Util::GameObject> m_Map;
    std::unique_ptr<Util::BGM> m_BattleBGM;

    std::shared_ptr<Util::GameObject> m_Text;

    bool m_IsLoggedIn = false;
};

#endif
