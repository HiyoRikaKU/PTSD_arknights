# PTSD Arknights — 程式架構說明

## 目錄

1. [專案概述](#專案概述)
2. [目錄結構](#目錄結構)
3. [核心架構](#核心架構)
   - [主迴圈與 App](#主迴圈與-app)
   - [場景管理（Core/SceneManager）](#場景管理-corescenemanager)
   - [場景層（Scenes）](#場景層-scenes)
   - [實體層（Entity）](#實體層-entity)
   - [地圖層（Map）](#地圖層-map)
   - [UI 層（UI）](#ui-層-ui)
4. [遊戲流程](#遊戲流程)
5. [關卡設計方式](#關卡設計方式)
6. [幹員系統](#幹員系統)
7. [敵人系統](#敵人系統)
8. [素材資源](#素材資源)
9. [已知問題（toDeveloper.txt）](#已知問題todevelopertxt)

---

## 專案概述

本專案是以 [PTSD](https://github.com/ntut-open-source-club/practical-tools-for-simple-design) 框架（基於 SDL2）開發的 Arknights（明日方舟）塔防遊戲仿作。  
使用 **C++17**，以 CMake 建置，僅支援 `Debug` 模式（`-DCMAKE_BUILD_TYPE=Debug`）。

---

## 目錄結構

```
PTSD_arknights/
├── CMakeLists.txt          # 建置設定
├── files.cmake             # 來源/標頭檔清單
├── src/
│   ├── main.cpp            # 程式進入點
│   └── Arknights/
│       ├── App.cpp
│       ├── Core/           # SceneManager
│       ├── Entity/         # Enemy, Operator
│       ├── Map/            # Operation (關卡基礎), Operation1 (第一關)
│       ├── Scenes/         # LoginScene, LobbyScene, GameScene
│       └── UI/             # Button
├── include/
│   ├── config.hpp          # 全域設定（覆蓋 PTSD 預設）
│   └── Arknights/
│       ├── App.hpp
│       ├── Core/
│       ├── Entity/
│       ├── Map/
│       ├── Scenes/
│       └── UI/             # Button.hpp, HealthBar.hpp
├── Resources/
│   ├── charactor/
│   │   ├── operator/       # 各幹員動畫幀（PNG 序列）
│   │   └── enemy/          # 各敵人動畫幀
│   ├── map/                # 關卡背景圖
│   ├── UI/                 # UI 素材
│   ├── font/               # 字型
│   ├── SFX/                # 音效
│   └── login.mp3           # 登入頁 BGM
└── docs/                   # Doxygen 文件設定
```

---

## 核心架構

### 主迴圈與 App

`src/main.cpp` 取得 PTSD 的 `Core::Context` 單例，並以狀態機驅動 `Arknights::App`：

```
START → UPDATE（每幀）→ END
```

- **START**：建立 `LoginScene` 並推入 `SceneManager`。  
- **UPDATE**：每幀呼叫 `SceneManager::update()` 與 `render()`；按 `ESC` 或關閉視窗時進入 END。  
- **END**：清空場景堆疊並結束。

`App` 持有 `Core::SceneManager` 的單例參考，自身不直接持有任何遊戲物件。

---

### 場景管理 Core/SceneManager

`SceneManager` 是 **Singleton**，內部以 `std::stack<std::shared_ptr<Scene>>` 管理場景。

| 方法 | 說明 |
|------|------|
| `pushScene(scene)` | 將新場景推入堆疊並初始化 |
| `popScene()` | 彈出當前場景 |
| `replaceScene(scene)` | 彈出當前場景後推入新場景 |
| `update(deltaTime)` | 呼叫堆頂場景的 `update()` |
| `render()` | 呼叫堆頂場景的 `render()` |
| `clear()` | 清空整個堆疊 |

---

### 場景層 Scenes

所有場景繼承自抽象基類 `Scene`：

```
Scene（抽象）
├── init()       純虛函式
├── update()     純虛函式
├── render()     虛函式（預設空）
├── cleanup()    虛函式（預設空）
├── onEnter()    虛函式（場景被推入時呼叫）
└── onExit()     虛函式（場景被彈出時呼叫）
```

每個場景內部持有 `Util::Renderer m_Root`，所有遊戲物件需加入 `m_Root` 的子節點才會被渲染。

#### LoginScene
- 顯示登入背景圖與 **START** 按鈕。  
- 點擊後以 `replaceScene` 切換至 `LobbyScene`。  
- 播放登入頁 BGM（`login.mp3`）。

#### LobbyScene
- 顯示大廳背景、使用者資訊、資源數值、主選單按鈕（出擊 / 幹員 / 編隊）。  
- 點擊「出擊（Stage）」後切換至 `GameScene`。

#### GameScene
- 遊戲主場景，包含：
  - 地圖渲染（`Operation`）
  - 幹員拖放部署
  - 敵人生成 / 移動 / 死亡
  - DP（部署費用）累積系統
  - 波次計時器
  - 勝負判定（逃脫敵人數 ≥ 4 → 失敗）

---

### 實體層 Entity

#### Operator（幹員）

```
Util::GameObject
└── Operator
    ├── Amiya
    ├── Chen
    ├── Angelina
    ├── Red
    ├── Eyjafjalla
    ├── Texas
    └── Umirin
```

每個幹員擁有：
- **屬性**：`hp`、`attack`、`deploymentCost`、`attackInterval`、`blockCount`
- **狀態**：`IDLE`、`ATTACK`、`SKILL`、`DEAD`
- **方向**：`UP`、`DOWN`、`LEFT`、`RIGHT`
- **動畫**：Idle 與 Attack 兩組 PNG 序列（偶數幀已刪除以節省載入時間）
- **血條**：`HealthBar` 子物件
- **攻擊範圍**：由 `isInAttackRange()` 依幹員類型與朝向計算

#### Enemy（敵人）

三種敵人類型：`gopro`（標準）、`bigbo`（高血厚甲）、`trslim`（快速脆皮）

- 沿預定路點（`m_GridWaypoints`）移動，使用透視變換（Homography）將格子座標轉換為螢幕座標。
- 狀態：`ALIVE`、`DYING`、`DEAD`
- 支援受阻（`setBlocked`）：被幹員擋住時停止移動並（理論上）攻擊幹員。
- 使用 **EnemyPool**（物件池，預設大小 64）管理生命週期以避免頻繁記憶體配置。

---

### 地圖層 Map

#### Operation（基礎關卡）

定義關卡所需資料：

| 元件 | 說明 |
|------|------|
| `TileType` 矩陣 | 定義每格類型：`OBSTACLE(x)`、`GROUND(f)`、`HIGH_GROUND(h)`、`SPAWN(r)`、`BASE(b)` |
| Homography | 由 4 組對應點計算透視矩陣，將格子索引映射至螢幕像素座標 |
| `WaveManager` | 管理波次時間軸（`SpawnEvent` 列表），依時間戳生成敵人 |

#### Operation1（第一關）

繼承 `Operation`，硬編碼 4×10 地圖矩陣、敵人路點與 5 波敵人生成排程（約 49 秒）。

---

### UI 層 UI

#### Button
- 支援圖片型與文字型兩種按鈕。
- 滑鼠懸停放大（`m_HoverScale = 1.1`），點擊縮小（`m_ClickScale = 0.95`）。
- 透過 `setOnClick(callback)` 設定點擊事件。

#### HealthBar
- 使用 `red.png` 縮放來呈現血量百分比。
- 跟隨角色位置更新，顯示在角色上方。

---

## 遊戲流程

```
程式啟動
   │
   ▼
LoginScene（START 按鈕）
   │
   ▼
LobbyScene（選擇出擊）
   │
   ▼
GameScene
   ├── 初始化地圖、幹員列表、敵人池
   ├── 玩家從底部 UI 拖拽幹員圖示到地圖格子
   ├── 放置後選擇朝向（上/下/左/右）
   ├── WaveManager 按時間軸生成敵人
   ├── 幹員偵測射程內敵人並自動攻擊
   ├── 敵人血量歸零 → 死亡動畫 → 回收至 EnemyPool
   ├── 敵人抵達終點 → 逃脫計數 +1
   └── 逃脫數 ≥ 4 → Game Over；所有敵人消滅 → Victory
```

---

## 關卡設計方式

1. 撰寫 `TileType` 二維矩陣（字元縮寫：`x/f/h/r/b`）定義地圖格局。
2. 使用 AI 輔助計算 Homography 的四個對應點（`srcPoints` → `dstPoints`），讓格子中心對齊背景圖的實際位置。
3. 敵人路點以 `(row, col)` 起點加上相對步驟向量疊加計算。
4. 敵人波次以 `WaveManager::addSpawnEvent(時間戳ms, 敵人類型, hp, speed)` 排程。

---

## 幹員系統

| 幹員 | 費用 | HP | 攻擊 | 格擋 | 備註 |
|------|------|----|------|------|------|
| Amiya（阿米婭） | 10 | 1000 | 120 | 1 | 遠程術師 |
| Chen（陳） | 18 | 1200 | 80 | 2 | 近戰前衛，雙段傷害 |
| Angelina（安潔莉娜） | 14 | 1100 | 100 | 1 | 遠程，可緩速敵人 |
| Red（紅） | 6 | 900 | 150 | 1 | 特殊角色，便宜但血薄、只擋一個 |
| Eyjafjalla（艾雅法拉） | 20 | 1200 | 200 | 1 | 高攻術師，**目前邏輯為攻擊而非治療** |
| Texas（德克薩斯） | 12 | 1100 | 100 | 2 | 近戰前衛，格擋上限偏低 |
| Umirin | 10 | 1000 | 120 | 1 | 遠程支援 |

> **注意**：Eyjafjalla 設定上應為回血角，但目前實作為攻擊。

---

## 敵人系統

| 類型 | 說明 |
|------|------|
| gopro | 標準速度、中等血量 |
| bigbo | 速度慢、高血量（坦克型） |
| trslim | 速度快、血量低（快攻型） |

---

## 素材資源

- **幹員動畫**：位於 `Resources/charactor/operator/<name>/`，包含 `Idle_XX.png` 與 `Attack_XX.png` 序列（偶數幀已刪除）。
- **敵人動畫**：位於 `Resources/charactor/enemy/`。
- **地圖背景**：位於 `Resources/map/`。
- **完整幹員名稱清單**：參見 `Resources/charactor/operator/` 目錄，或查閱 [PRTS Wiki 幹員一覽](https://prts.wiki/w/%E5%B9%B2%E5%91%98%E4%B8%80%E8%A7%88)。

---

## 已知問題（toDeveloper.txt）

以下問題來自開發者備忘錄 `toDeveloper.txt`：

### UI / 顯示

| # | 問題 | 說明 |
|---|------|------|
| 1 | **Cost 顯示位置太低** | DP 數值文字顯示在螢幕過低的位置，影響可讀性。 |
| 2 | **角色 cost 不清楚** | 底部幹員選取介面上的費用數字不夠明顯。 |
| 3 | **cost 不足時無法放置的提示不清楚** | 玩家嘗試部署時若 DP 不足，缺乏明確的視覺／音效反饋。 |
| 4 | **血條顏色異常（彩色）** | 推測是 `red.png` 方形圖片被過度壓扁導致顏色顯示不正確。 |
| 5 | **圖片與視窗大小不匹配** | 素材未隨視窗縮放，可考慮實作「選擇視窗大小」功能並讓素材跟著拉伸。 |

### 遊戲邏輯

| # | 問題 | 說明 |
|---|------|------|
| 6 | **敵人不會攻擊幹員** | 敵人被幹員格擋後，`m_TargetOperator` 有設定，但實際攻擊邏輯尚未實作。 |
| 7 | **點擊戰鬥後初始化未完成就開始生怪** | `GameScene::init()` 與波次計時器之間存在競態，敵人在場景完全初始化前就開始生成。 |

### 穩定性

| # | 問題 | 說明 |
|---|------|------|
| 8 | **遊戲結束時 Segment Fault** | 結束時會觸發 segfault，但不影響正常遊戲過程（推測是資源釋放順序問題）。 |
| 9 | **大量 debug log** | 程式碼中散落許多 `LOG_TRACE` / `LOG_DEBUG` 輸出，正式版本應清理。 |

### 素材

| # | 問題 | 說明 |
|---|------|------|
| 10 | **角色上下被截 / 攻擊時位移** | 由 `spine-export-cli` 輸出的問題，素材本身就有偏移，暫時無解。 |

### 待實作功能

| # | 功能 | 說明 |
|---|------|------|
| 11 | **選關介面** | 目前 LobbyScene 的 Stage 按鈕直接進入第一關，缺少關卡選擇頁面。 |
| 12 | **第二關** | 目前只有 `Operation1`，尚未開發第二關。 |
| 13 | **編隊功能** | 開發者認為編隊功能可能不必要，可視需求取捨。 |

---

> 最後更新：2026-04-16
