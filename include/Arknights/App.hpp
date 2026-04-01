#ifndef ARKNIGHTS_APP_HPP
#define ARKNIGHTS_APP_HPP

#include "pch.hpp" // IWYU pragma: export
#include "Arknights/Core/SceneManager.hpp"

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

    State getCurrentState() const { return m_CurrentState; }

private:
    State m_CurrentState = State::START;
    Core::SceneManager& m_SceneManager = Core::SceneManager::getInstance();
};

} // namespace Arknights

#endif
