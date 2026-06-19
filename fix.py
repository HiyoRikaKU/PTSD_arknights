import sys

content = open('src/Arknights/Scenes/GameScene.cpp', encoding='utf-8').read()

start_marker = r'''    m_MissionCompletedBar->SetVisible(false);
    m_Root.AddChild(m_MissionCompletedBar);'''

end_marker = r'''    m_AmiyaCostText->m_Transform.translation = {m_AmiyaIcon->m_Transform.translation.x, m_AmiyaIcon->m_Transform.translation.y + 65.0f};
    m_Root.AddChild(m_AmiyaCostText);'''

correct_middle = r'''

    m_MissionCompletedImage = std::make_shared<ExGameObject>(
        std::make_shared<Util::Image>(
            std::string(RESOURCE_DIR) + "/UI/game_condition/mission_completed.png"),
        8   
    );
    m_MissionCompletedImage->m_Transform.translation = {LOGO_OFFSCREEN_X, LOGO_TARGET_Y};
    m_MissionCompletedImage->m_Transform.scale       = {0.9f, 0.9f};
    m_MissionCompletedImage->SetVisible(false);
    m_Root.AddChild(m_MissionCompletedImage);
    // ────────────────────────────────────────────────────────────────

    std::string winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin.jpg";
    if (m_StageId == "0-2") {
        winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin/0_2_star3.png";
    } else if (m_StageId == "0-3") {
        winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin/0_3_star3.png";
    } else if (m_StageId == "0-4") {
        winImagePath = std::string(RESOURCE_DIR) + "/UI/game_condition/yourWin/0_4_star3.png";
    }

    m_YourWinImage = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(winImagePath), 11);
    m_YourWinImage->m_Transform.translation = {0.0f, 0.0f};
    m_YourWinImage->m_Transform.scale = {1.0f, 1.0f};
    m_YourWinImage->SetVisible(false);
    m_Root.AddChild(m_YourWinImage);

    m_MissionFailedImage = std::make_shared<ExGameObject>(std::make_shared<Util::Image>(std::string(RESOURCE_DIR) + "/UI/game_condition/mission_failed.png"), 11);
    m_MissionFailedImage->m_Transform.translation = {0.0f, 0.0f};
    m_MissionFailedImage->m_Transform.scale = {0.9f, 0.9f};
    m_MissionFailedImage->SetVisible(false);
    m_Root.AddChild(m_MissionFailedImage);

    m_DPText = std::make_shared<ExGameObject>(std::make_shared<Util::Text>(std::string(RESOURCE_DIR) + "/font/NotoSerifTC.ttf", 40, "COST: 10", Util::Color(255, 255, 0)), 2);
    m_DPText->m_Transform.translation = {620, -250};
    m_Root.AddChild(m_DPText);

    // Cost labels
'''

start_idx = content.find(start_marker)
end_idx = content.find(end_marker)

if start_idx != -1 and end_idx != -1:
    new_content = content[:start_idx + len(start_marker)] + correct_middle + content[end_idx:]
    open('src/Arknights/Scenes/GameScene.cpp', 'w', encoding='utf-8').write(new_content)
    print('Fix applied.')
else:
    print('Failed to find markers.')
