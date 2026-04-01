#include "Arknights/App.hpp"
#include "Arknights/Scenes/LoginScene.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

namespace Arknights {

void App::start() {
    LOG_TRACE("App Start");
    
    // Initialize with LoginScene (user starts at login page)
    auto loginScene = std::make_shared<LoginScene>();
    m_SceneManager.pushScene(loginScene);
    
    m_CurrentState = State::UPDATE;
}

void App::update() {
    float deltaTime = Util::Time::GetDeltaTimeMs();
    
    // Update current scene
    m_SceneManager.update(deltaTime);
    
    // Check for exit
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::end() { 
    LOG_TRACE("App End");
}

} // namespace Arknights
