#ifndef ARKNIGHTS_LOGINSCENE_HPP
#define ARKNIGHTS_LOGINSCENE_HPP

#include <memory>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/UI/Button.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/BGM.hpp"

namespace Arknights {

/**
 * @brief Login scene - Shows login page with START button
 * 
 * This scene is shown when the game first starts.
 * After clicking START, it transitions to LobbyScene.
 */
class LoginScene : public Scene {
public:
    LoginScene();
    ~LoginScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;
    
    void onEnter() override;
    void onExit() override;

private:
    void createLoginUI();
    void onStartButtonClicked();

private:
    // Login page background
    std::shared_ptr<Util::GameObject> m_LoginBackground;
    
    // START button
    std::shared_ptr<UI::Button> m_StartButton;
    
    // BGM
    std::unique_ptr<Util::BGM> m_LoginBGM;
};

} // namespace Arknights

#endif // ARKNIGHTS_LOGINSCENE_HPP
