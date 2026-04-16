#ifndef ARKNIGHTS_STAGESELECTSCENE_HPP
#define ARKNIGHTS_STAGESELECTSCENE_HPP

#include <memory>
#include <vector>

#include "Arknights/Scenes/Scene.hpp"
#include "Arknights/UI/Button.hpp"
#include "Util/GameObject.hpp"
#include "Util/BGM.hpp"

namespace Arknights {

class StageSelectScene : public Scene {
public:
    StageSelectScene();
    ~StageSelectScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;

    void onEnter() override;
    void onExit() override;

private:
    void onStage02Clicked();
    void onBackClicked();

private:
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<UI::Button> m_Stage02Button;
    std::shared_ptr<UI::Button> m_BackButton;
    std::vector<std::shared_ptr<UI::Button>> m_Buttons;
    bool m_RequestStage02 = false;
    bool m_RequestBack = false;
};

} // namespace Arknights

#endif
