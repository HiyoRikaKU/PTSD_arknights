#pragma once
#include "Arknights/Scenes/Scene.hpp"
#include "Util/GameObject.hpp"

namespace Arknights {

class ReturnLoadingScene : public Scene {
public:
    ReturnLoadingScene() = default;
    ~ReturnLoadingScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;

    void onEnter() override;
    void onExit() override;

private:
    std::shared_ptr<Util::GameObject> m_LoadingImage;
    float m_Timer = 0.0f;
    const float DURATION_MS = 2000.0f;
};

} // namespace Arknights
