#ifndef ARKNIGHTS_STAGESELECTSCENE_HPP
#define ARKNIGHTS_STAGESELECTSCENE_HPP

#include <memory>
#include <string>
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
    void onStageClicked(const std::string& stageId);
    void onStartActionClicked();
    void onBackClicked();

private:
    std::shared_ptr<Util::GameObject> m_SelectBackground;
    std::shared_ptr<Util::GameObject> m_ConfirmBackground;
    std::vector<std::shared_ptr<UI::Button>> m_StageButtons;
    std::shared_ptr<UI::Button> m_StartActionButton;
    std::shared_ptr<UI::Button> m_BackButton;
    std::vector<std::shared_ptr<UI::Button>> m_Buttons;
    std::string m_SelectedStageId;
    bool m_RequestStartOperation = false;
    bool m_RequestBack = false;
};

} // namespace Arknights

#endif
