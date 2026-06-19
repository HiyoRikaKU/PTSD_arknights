#include "Arknights/Scenes/LobbyScene.hpp"
#include "Arknights/Scenes/ZoneScene.hpp"
#include "Arknights/Core/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include <algorithm>
#include <random>

namespace Arknights {

LobbyScene::LobbyScene() {
}

void LobbyScene::init() {
    LOG_DEBUG("Initializing LobbyScene");

    m_LobbyBGM = std::make_unique<Util::BGM>(std::string(RESOURCE_DIR) + "/SFX/lobby.mp3");
    m_DuctorSFX = std::make_unique<Util::SFX>(std::string(RESOURCE_DIR) + "/SFX/Ductor.mp3");
    m_HazeVoiceSFX = std::make_unique<Util::SFX>(std::string(RESOURCE_DIR) + "/SFX/はぜボイス.mp3");
    m_HazeVoice2SFX = std::make_unique<Util::SFX>(std::string(RESOURCE_DIR) + "/SFX/はぜボイス2.mp3");

    createBackground();        // background image + character slider
    createLobbyUI();           // static lobbyUI.png overlay (right side)
    createTerminalButton();    // only interactive button

    m_Initialized = true;
}

void LobbyScene::createBackground() {
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/UI_HOME_FRONT_BKG.png"),
        0
    );
    m_Background->m_Transform.scale =
        glm::vec2(1600.0f, 900.0f) / m_Background->GetScaledSize();
    m_Root.AddChild(m_Background);

    createCharacterSlider();
}

void LobbyScene::createCharacterSlider() {
    m_CharacterArtPaths = {
        std::string(RESOURCE_DIR) + "/charactor/Haze_Skin_1.png",
        std::string(RESOURCE_DIR) + "/charactor/Yahata_Umiri_Elite.png"
    };
    m_CharacterScales = {
        {0.64f, 0.64f},
        {0.7f, 0.7f}
    };

    m_CharacterArts.clear();
    for (std::size_t i = 0; i < m_CharacterArtPaths.size(); ++i) {
        auto art = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(m_CharacterArtPaths[i]),
            1
        );
        const glm::vec2 scale =
            i < m_CharacterScales.size() ? m_CharacterScales[i] : glm::vec2(0.4f, 0.4f);
        art->m_Transform.translation = m_CharacterBasePos;
        art->m_Transform.scale = scale;
        art->SetVisible(false);
        m_Root.AddChild(art);
        m_CharacterArts.push_back(art);
    }

    if (!m_CharacterArts.empty()) {
        m_CurrentCharacterIndex = 0;
        m_CharacterArts[m_CurrentCharacterIndex]->SetVisible(true);
        m_CharacterArt = m_CharacterArts[m_CurrentCharacterIndex];
    }
}


void LobbyScene::createLobbyUI() {
    m_LobbyUI = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/lobby/lobbyUI.png"),2  );
    m_LobbyUI->m_Transform.scale =
        glm::vec2(1600.0f, 900.0f) / m_LobbyUI->GetScaledSize();
    m_LobbyUI->m_Transform.translation = {0.0f, 0.0f};

    m_Root.AddChild(m_LobbyUI);
}

void LobbyScene::createTerminalButton() {
    const std::string imgPath =
        std::string(RESOURCE_DIR) + "/UI/lobby/terminal.png";

    m_TerminalButton = std::make_shared<UI::Button>(imgPath, glm::vec2(0.0f, 0.0f), 3.0f);

    m_TerminalButton->setHitBox(glm::vec2(350.0f, 238.0f), glm::vec2(400.0f, 200.0f));

    m_TerminalButton->setHoverScale(1.0f); 
    m_TerminalButton->setClickScale(1.0f); 

    m_TerminalButton->setOnClick([this]() { onTerminalClicked(); });

    m_Root.AddChild(m_TerminalButton);
}

void LobbyScene::update(float deltaTime) {
    // Character slide 
    if (!m_IsCharacterSliding) {
        if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
            startCharacterSlide(1);
        } else if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
            startCharacterSlide(-1);
        }
    }
    updateCharacterSlide(deltaTime);
    updateCharacterVoiceButton(deltaTime);

    updateTerminalButton(deltaTime);

    if (m_RequestStageSelect) {
        m_RequestStageSelect = false;
        auto zoneScene = std::make_shared<ZoneScene>();
        Core::SceneManager::getInstance().replaceScene(zoneScene);
    }
}


void LobbyScene::updateTerminalButton(float deltaTime) {
    if (!m_TerminalButton) return;
    if (m_FadeState == FadeState::IDLE) {
        m_TerminalButton->update(deltaTime);
    } else if (m_FadeState == FadeState::WAITING) {
        // Calculate new alpha (fades down to 0.8 instead of 0.0)
        m_FadeTimer += deltaTime;
        float alpha = 1.0f - 0.2f * (m_FadeTimer / FADE_DURATION_MS);
        if (alpha < 0.8f) alpha = 0.8f;
        
        m_TerminalButton->setAlpha(alpha);

        if (m_FadeTimer >= FADE_DURATION_MS) {
            m_TerminalButton->setAlpha(1.0f); // Reset for when returning to lobby
            m_RequestStageSelect = true;
            m_FadeState = FadeState::IDLE;
        }
    }
}

void LobbyScene::onTerminalClicked() {
    if (m_FadeState != FadeState::IDLE) return;
    LOG_INFO("Terminal button clicked");
    
    // Start fading
    m_FadeState = FadeState::WAITING;
    m_FadeTimer = 0.0f;
}

void LobbyScene::updateCharacterVoiceButton(float /*deltaTime*/) {
    const bool mouseDown = Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB);
    const bool canClickHaze =
        !m_IsCharacterSliding &&
        m_CurrentCharacterIndex == 0 &&
        m_CharacterArt &&
        isPointInHazeArt(Util::Input::GetCursorPosition());

    if (canClickHaze && mouseDown) {
        m_WasMousePressedOnHaze = true;
    } else if (m_WasMousePressedOnHaze && !mouseDown) {
        if (canClickHaze) {
            onHazeClicked();
        }
        m_WasMousePressedOnHaze = false;
    } else if (!mouseDown) {
        m_WasMousePressedOnHaze = false;
    }
}

bool LobbyScene::isPointInHazeArt(const glm::vec2& point) const {
    if (!m_CharacterArt) return false;

    const glm::vec2 center = m_CharacterArt->m_Transform.translation;
    const glm::vec2 halfSize = m_CharacterArt->GetScaledSize() * 0.5f;
    return point.x >= center.x - halfSize.x &&
           point.x <= center.x + halfSize.x &&
           point.y >= center.y - halfSize.y &&
           point.y <= center.y + halfSize.y;
}

void LobbyScene::onHazeClicked() {
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<int> distribution(0, 1);

    if (distribution(generator) == 0) {
        if (m_HazeVoiceSFX) m_HazeVoiceSFX->Play();
    } else {
        if (m_HazeVoice2SFX) m_HazeVoice2SFX->Play();
    }
}

void LobbyScene::startCharacterSlide(int direction) {
    if (m_CharacterArts.size() < 2 || direction == 0 || m_IsCharacterSliding) return;

    const int count = static_cast<int>(m_CharacterArts.size());
    int nextIndex = (m_CurrentCharacterIndex + direction + count) % count;
    if (nextIndex == m_CurrentCharacterIndex) return;

    m_NextCharacterIndex = nextIndex;
    m_CharacterSlideDirection = direction;
    m_CharacterSlideTimer = 0.0f;
    m_IsCharacterSliding = true;

    auto& incoming = m_CharacterArts[m_NextCharacterIndex];
    incoming->SetVisible(true);
    incoming->m_Transform.scale =
        m_NextCharacterIndex < static_cast<int>(m_CharacterScales.size())
            ? m_CharacterScales[m_NextCharacterIndex]
            : glm::vec2(0.4f, 0.4f);
    incoming->m_Transform.translation = {
        m_CharacterBasePos.x + CHARACTER_SLIDE_DISTANCE * static_cast<float>(direction),
        m_CharacterBasePos.y
    };
}

void LobbyScene::updateCharacterSlide(float deltaTime) {
    if (!m_IsCharacterSliding || m_NextCharacterIndex < 0) return;

    m_CharacterSlideTimer += deltaTime;
    const float t =
        glm::clamp(m_CharacterSlideTimer / CHARACTER_SLIDE_DURATION_MS, 0.0f, 1.0f);
    const float offset =
        CHARACTER_SLIDE_DISTANCE * t * static_cast<float>(m_CharacterSlideDirection);

    auto& current = m_CharacterArts[m_CurrentCharacterIndex];
    auto& incoming = m_CharacterArts[m_NextCharacterIndex];
    current->m_Transform.translation = {m_CharacterBasePos.x - offset, m_CharacterBasePos.y};
    incoming->m_Transform.translation = {
        m_CharacterBasePos.x +
            CHARACTER_SLIDE_DISTANCE * static_cast<float>(m_CharacterSlideDirection) - offset,
        m_CharacterBasePos.y
    };

    if (t >= 1.0f) {
        current->SetVisible(false);
        current->m_Transform.translation = m_CharacterBasePos;

        m_CurrentCharacterIndex = m_NextCharacterIndex;
        m_NextCharacterIndex = -1;
        m_IsCharacterSliding = false;
        m_CharacterSlideDirection = 0;
        m_CharacterArt = m_CharacterArts[m_CurrentCharacterIndex];

        m_CharacterArt->SetVisible(true);
        m_CharacterArt->m_Transform.translation = m_CharacterBasePos;
        m_CharacterArt->m_Transform.scale =
            m_CurrentCharacterIndex < static_cast<int>(m_CharacterScales.size())
                ? m_CharacterScales[m_CurrentCharacterIndex]
                : glm::vec2(0.4f, 0.4f);
    }
}

void LobbyScene::cleanup() {
    LOG_DEBUG("Cleaning up LobbyScene");
    m_LobbyBGM.reset();
    m_DuctorSFX.reset();
    m_HazeVoiceSFX.reset();
    m_HazeVoice2SFX.reset();
}

void LobbyScene::onEnter() {
    LOG_DEBUG("Entering LobbyScene");
    if (m_DuctorSFX) {
        m_DuctorSFX->Play();
    }
    if (m_LobbyBGM) {
        m_LobbyBGM->Play();
    }
}

void LobbyScene::onExit() {
    LOG_DEBUG("Exiting LobbyScene");
    if (m_LobbyBGM) {
        m_LobbyBGM->FadeOut(250);
    }
}

} // namespace Arknights
