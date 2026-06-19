# GameScene UI / Audio Handoff

## 目前狀態摘要

這份文件記錄目前已完成的功能、正在修改中的 GameScene UI 需求，以及下一位接手者需要注意的未完成事項。

目前工作區不是乾淨狀態，已有多個使用者提供或先前修改的未提交檔案。不要直接 `git reset` 或回復未確認的變更。

## 已完成需求

### Lobby / Zone / Selection 導航

已完成：

- `LobbyScene` 點 terminal 後不再直接進 `StageSelectScene`，改為先進 `ZoneScene`。
- 新增 `ZoneScene`：
  - 背景使用 `Resources/UI/zone/next.png`
  - 左上 `<` 回 `LobbyScene`
  - home icon 回 `LobbyScene`
  - 點右上「黑暗時代 / EVIL TIME」區域進 `StageSelectScene`
- `StageSelectScene`：
  - 移除原本「返回」文字按鈕
  - 加上 `Resources/UI/UI_component/back.png`
  - `<` 回 `ZoneScene`
  - home icon 回 `LobbyScene`
- `files.cmake` 已加入 `ZoneScene.cpp/.hpp`

相關檔案：

- `include/Arknights/Scenes/ZoneScene.hpp`
- `src/Arknights/Scenes/ZoneScene.cpp`
- `src/Arknights/Scenes/LobbyScene.cpp`
- `include/Arknights/Scenes/StageSelectScene.hpp`
- `src/Arknights/Scenes/StageSelectScene.cpp`
- `files.cmake`

驗證：

- `cmake --build .\build --target PTSD_Arknights` 曾成功。

### Lobby 音訊

已完成：

- 進入 `LobbyScene` 時播放：
  - `Resources/SFX/Ductor.mp3`
  - `Resources/SFX/lobby.mp3` 作為 lobby BGM
- `LoginScene::onExit()` 不再對 `login.mp3` 做 400ms fade out，避免進 lobby 後舊音樂淡出重疊。
- 點目前顯示的 Haze 立繪時，隨機播放：
  - `Resources/SFX/はぜボイス.mp3`
  - `Resources/SFX/はぜボイス2.mp3`

注意：

- Haze 點擊目前使用角色圖片縮放後的矩形範圍判定，不是逐像素 alpha 判定。

相關檔案：

- `include/Arknights/Scenes/LobbyScene.hpp`
- `src/Arknights/Scenes/LobbyScene.cpp`
- `src/Arknights/Scenes/LoginScene.cpp`

驗證：

- `cmake --build .\build --target PTSD_Arknights` 曾成功。

## GameScene UI 需求

使用者最新需求：

- 在 `GameScene` 實作 `Resources/UI/UI_component/gameUI.png`
- 左上角設定 icon：
  - 點擊後在遊戲上方顯示 `Resources/UI/UI_component/scene.png`
  - 可選擇 Cheat Mode / Normal Mode，整合原本純文字功能
  - 可選擇退出行動
- 點退出行動後顯示 `Resources/UI/UI_component/exit.png`
  - 點右下角「放棄行動」區塊回到 `StageSelectScene`
  - 點「返回」回遊戲
- 右上角第一格播放鍵：
  - 切換目前遊戲倍速 x1 / x2
- 右上角最右邊按鍵：
  - 暫停遊戲
  - 顯示 `Resources/UI/UI_component/pause.png`
  - 按 `ESC` 回到目前進行中的遊戲
- 追加需求：
  - 點擊任何按鍵都播放 `Resources/SFX/click.mp3`

## GameScene 已完成部分

已完成並編譯驗證。

已改：

- `include/Arknights/Scenes/GameScene.hpp`
  - 新增 overlay 成員：
    - `m_GameUIOverlay`
    - `m_SettingsOverlayImage`
    - `m_ExitOverlayImage`
    - `m_PauseOverlayImage`
  - 新增按鈕：
    - `m_SettingsButton`
    - `m_ExitCancelButton`
    - `m_ExitConfirmButton`
  - 新增狀態：
    - `m_IsExitConfirmOpen`
  - 新增 click SFX：
    - `m_ClickSFX`
  - 宣告 helper：
    - `openExitConfirm()`
    - `closeExitConfirm()`
    - `returnToStageSelect()`
    - `playClickSFX()`

- `src/Arknights/Scenes/GameScene.cpp`
  - 已加入：
    - `#include "Arknights/Scenes/StageSelectScene.hpp"`
    - `constexpr glm::vec2 kScreenSize{1600.0f, 900.0f};`
  - UI 初始化區已部分替換：
    - 加入 `gameUI.png` 常駐 HUD
    - 加入 `scene.png` 設定 overlay
    - 加入 `exit.png` 退出確認 overlay
    - 加入 `pause.png` 暫停 overlay
    - 左上設定、右上倍速、右上暫停已改為透明 hitbox
    - 設定 overlay 中保留文字按鈕 `Normal Mode` / `Cheat Mode` / `退出行動`
    - 加入 `click.mp3` 載入
  - `update()` 已修改：
    - `ESC` 嘗試關閉 exit/settings/pause overlay
    - `P` 與 `NUM_2` 會先播放 click SFX
    - paused/settings/exit confirm 時停止 gameplay update

- `updateButtons()` 已根據 `m_IsHubOpen` / `m_IsExitConfirmOpen` / `m_IsPaused` 控制 overlay 和透明按鈕。
- 已補上：
  - `toggleHub()`
  - `openExitConfirm()`
  - `closeExitConfirm()`
  - `returnToStageSelect()`
  - `playClickSFX()`

## 實作邏輯

1. `GameScene::updateButtons(float deltaTime)`
   - `showBaseHud = !m_IsGameOver && !m_IsHubOpen && !m_IsExitConfirmOpen`
   - `showPauseMenu = showBaseHud && m_IsPaused`
   - `showSettingsMenu = !m_IsGameOver && m_IsHubOpen && !m_IsExitConfirmOpen`
   - `showExitConfirm = !m_IsGameOver && m_IsExitConfirmOpen`
   - 顯示/更新：
     - `m_GameUIOverlay`: `!m_IsGameOver`
     - `m_SettingsOverlayImage`: `showSettingsMenu`
     - `m_ExitOverlayImage`: `showExitConfirm`
     - `m_PauseOverlayImage`: `showPauseMenu`
     - `m_SettingsButton`, `m_PauseButton`, `m_SpeedButton`: base HUD 狀態下更新
     - `m_NormalModeButton`, `m_CheatModeButton`, `m_ExitButton`: settings menu 狀態下更新
     - `m_ExitCancelButton`, `m_ExitConfirmButton`: exit confirm 狀態下更新

2. helper 實作
   - `toggleHub()`
     - 開關 `m_IsHubOpen`
     - 開啟時清掉拖曳中的 operator 狀態
   - `openExitConfirm()`
     - `m_IsExitConfirmOpen = true`
     - `m_IsHubOpen = false`
     - `m_IsPaused = false`
   - `closeExitConfirm()`
     - 回遊戲：`m_IsExitConfirmOpen = false`
   - `returnToStageSelect()`
     - `Core::SceneManager::getInstance().replaceScene(std::make_shared<StageSelectScene>())`
   - `playClickSFX()`
     - `if (m_ClickSFX) m_ClickSFX->Play();`

3. 檢查 callbacks
   - 設定 icon：`playClickSFX(); toggleHub();`
   - 暫停 icon：`playClickSFX(); togglePause();`
   - 倍速 icon：`playClickSFX(); setGameSpeed(...)`
   - Normal/Cheat：`playClickSFX(); setCheatMode(...)`
   - 退出行動：`playClickSFX(); openExitConfirm();`
   - exit 返回：`playClickSFX(); closeExitConfirm();`
   - exit 放棄行動：`playClickSFX(); returnToStageSelect();`

4. 編譯驗證
   - 使用：
     - `cmake --build .\build --target PTSD_Arknights`
   - 不建議用完整 `cmake --build .\build` 作為唯一驗證，因為先前完整 build 會卡在 PTSD/GLEW helper target 的 `.rc.obj` 32/64-bit 架構不相容，這是既有工具鏈問題。

## 已知素材狀態

已確認存在：

- `Resources/UI/UI_component/gameUI.png`
- `Resources/UI/UI_component/scene.png`
- `Resources/UI/UI_component/exit.png`
- `Resources/UI/UI_component/pause.png`
- `Resources/SFX/click.mp3`

素材觀察：

- `gameUI.png` 是 1600x900 RGBA，透明背景。
- `exit.png` 是 1600x900 RGBA，底部左半是返回，右半是放棄行動。
- `pause.png` 是 1600x900 RGBA，含半透明遮罩與 PAUSE 文字。
- `scene.png` 是 1600x900 RGBA，但檢查到沒有非黑色可見內容，主要像是半透明黑色遮罩。因此目前設計是在 `scene.png` 上疊原本功能的文字按鈕，避免使用者看不到可選項。

## 注意事項

- 使用者特別說「有不懂請問，不要幻想」。如果 `scene.png` 實際應該有設計稿但目前檔案看起來全黑，接手者應確認是否素材放錯，或是否接受在遮罩上疊文字按鈕。
- 目前 `GameScene` UI 修改已收尾並通過主 target 編譯。
- 工作區已有其他未提交變更與未追蹤素材，例如 `ReturnLoadingScene`、`ZoneScene`、UI/SFX 圖檔等，接手時應先 `git status --short` 確認範圍。
