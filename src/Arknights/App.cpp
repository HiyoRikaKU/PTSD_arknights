#include "Arknights/App.hpp"
#include "Arknights/Scenes/LoginScene.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/Image.hpp"

namespace Arknights {

void App::start() {
    LOG_TRACE("App Start");

    preloadHeavyAssets();
    
    // Initialize with LoginScene (user starts at login page)
    auto loginScene = std::make_shared<LoginScene>();
    m_SceneManager.pushScene(loginScene);
    
    m_CurrentState = State::UPDATE;
}

void App::update() {
    float deltaTime = Util::Time::GetDeltaTimeMs();
    
    // Update current scene (this also calls m_Root.Update() for the current scene)
    m_SceneManager.update(deltaTime);
    
    // Draw current scene
    m_SceneManager.render();
    
    // Check for exit
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::end() { 
    m_SceneManager.clear();
    LOG_TRACE("App End");
}

void App::reset() {
    // Reset logic is now handled within scenes or by re-creating scenes
}

void App::preloadHeavyAssets() {
    if (!m_PreloadedImages.empty()) return;

    const std::vector<std::string> heavyPaths = {
        std::string(RESOURCE_DIR) + "/UI/splash_screen1.jpg",
        std::string(RESOURCE_DIR) + "/UI/splash_screen2.jpg",
        std::string(RESOURCE_DIR) + "/UI/splash_screen3.jpg",
        std::string(RESOURCE_DIR) + "/UI/login_loading/LoginScene1.png",
        std::string(RESOURCE_DIR) + "/UI/login_loading/LoginScene2.png",
        std::string(RESOURCE_DIR) + "/UI/login_loading/LoginScene3.png",
        std::string(RESOURCE_DIR) + "/login_page.jpg",
        std::string(RESOURCE_DIR) + "/UI/loadingScene/main_loading.png",
        std::string(RESOURCE_DIR) + "/UI/loadingScene/avg_0_3.png",
        std::string(RESOURCE_DIR) + "/UI/UI_HOME_FRONT_BKG.png",
        std::string(RESOURCE_DIR) + "/map/operation1.jpg"
    };

    m_PreloadedImages.reserve(heavyPaths.size());
    for (const auto& path : heavyPaths) {
        m_PreloadedImages.push_back(std::make_shared<Util::Image>(path));
    }

    LOG_INFO("Preloaded {} heavy image assets", m_PreloadedImages.size());
}

} // namespace Arknights
