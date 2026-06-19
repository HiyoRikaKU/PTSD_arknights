#ifndef ARKNIGHTS_ZONESCENE_HPP
#define ARKNIGHTS_ZONESCENE_HPP

#include <memory>
#include <vector>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/UI/Button.hpp"
#include "Util/GameObject.hpp"

namespace Arknights {

class ZoneScene : public Scene {
public:
    ZoneScene();
    ~ZoneScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;

    void onEnter() override;
    void onExit() override;

private:
    void onBackClicked();
    void onHomeClicked();
    void onEvilTimeClicked();

private:
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<UI::Button> m_BackButton;
    std::shared_ptr<UI::Button> m_HomeButton;
    std::shared_ptr<UI::Button> m_EvilTimeButton;
    std::vector<std::shared_ptr<UI::Button>> m_Buttons;

    bool m_RequestLobby = false;
    bool m_RequestStageSelect = false;
};

} // namespace Arknights

#endif
