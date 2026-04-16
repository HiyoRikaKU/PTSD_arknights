#ifndef ARKNIGHTS_LOADINGSCENE_HPP
#define ARKNIGHTS_LOADINGSCENE_HPP

#include <memory>
#include "Arknights/Scenes/Scene.hpp"
#include "Util/GameObject.hpp"

namespace Arknights {

class LoadingScene : public Scene {
public:
    LoadingScene();
    ~LoadingScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;
    
    void onEnter() override;
    void onExit() override;

private:
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_LoadingText;
    
    float m_Timer = 0.0f;
    bool m_HasStartedLoading = false;
};

} // namespace Arknights

#endif
