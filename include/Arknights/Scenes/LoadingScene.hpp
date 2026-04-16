#ifndef ARKNIGHTS_LOADINGSCENE_HPP
#define ARKNIGHTS_LOADINGSCENE_HPP

#include <memory>
#include <string>
#include "Arknights/Scenes/Scene.hpp"
#include "Util/GameObject.hpp"

namespace Arknights {

class LoadingScene : public Scene {
public:
    explicit LoadingScene(std::string stageId = "0-2");
    ~LoadingScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void cleanup() override;
    
    void onEnter() override;
    void onExit() override;

private:
    void startLoadingGameScene();
    void updateLoadingVisual();

private:
    std::string m_StageId;

    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_LoadingImageA;
    std::shared_ptr<Util::GameObject> m_LoadingImageB;

    std::shared_ptr<class GameScene> m_PreparedGameScene;

    float m_Timer = 0.0f;
    bool m_HasStartedLoading = false;
    bool m_AssetsLoaded = false;
    bool m_HasTransitioned = false;

    static constexpr float IMAGE_DURATION_MS = 1800.0f;
};

} // namespace Arknights

#endif
