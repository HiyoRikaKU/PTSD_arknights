# 2026 OOPL Final Report

## 組別資訊

組別：第七組
組員：08陳子揚、60郭東旭
復刻遊戲：明日方舟

## 專案簡介

### 遊戲簡介

明日方舟 Arknights 是一款策略塔防手機遊戲。玩家在地圖格子上部署幹員，攔截沿預定路線前進的敵人。

- **幹員部署**：從底部面板拖放幹員至地圖格子，放置後選擇朝向（上/下/左/右），幹員自動攻擊範圍內敵人
- **敵人系統**：4 種敵人類型（gopro、bigbo、trslim、yokai），沿路點行走，被幹員格擋後停止並攻擊
- **DP（部署費用）系統**：初始 10 DP，每秒自動 +1，部署幹員消耗不同費用
- **波次系統**：每關 5 波敵人，依時間軸生成
- **勝負判定**：消滅所有敵人則勝利；4 名以上敵人抵達終點則失敗
- **4 個關卡**
- **7 位幹員**：Amiya、Chen、Angelina、Red、Eyjafjalla、Texas、Umirin

### 組別分工

**08陳子揚 (PurpleSheep)**

- 專案初始架構與 App 主迴圈
- 敵人移動與路徑系統（Homography 透視變換）
- 幹員拖放部署系統
- 7 位幹員類別實作與屬性設計
- 音效 / BGM 整合
- 動畫素材收集與處理（偶數幀刪除優化）
- EnemyPool 物件池
- WaveManager 波次排程
- Operation1 初始關卡定義

**60郭東旭 (Hiyori)**

- 資料驅動架構重構（StageRepository / StageFactory / StageDefinition）
- 場景系統（Login / Lobby / StageSelect / Loading / Game）
- 敵人對幹員戰鬥系統與動畫
- 3 關卡資料定義與素材
- Yokai 巡邏型敵人
- Cheat / Normal 模式切換
- 任務完成 / 失敗狀態機
- 資源預載入與登入動畫
- UI 調整（暫停 / 2 倍速 / Cost 顯示）
- SkewSprite 自定義渲染

## 遊戲介紹

### 遊戲規則

1. 遊戲開始時進入登入畫面，播放開場動畫後進入大廳
2. 在大廳選擇「終端」進入關卡選擇畫面，選擇關卡後進入戰鬥
3. 戰鬥中從底部幹員面板拖放幹員至地圖格子：
   - 遠程幹員只能放在高地
   - 近戰幹員放在地面或部署點
4. 放置後選擇幹員朝向，影響攻擊範圍方向
5. DP（部署費用）初始 10 點，每秒 +1，每位幹員有不同部署花費
6. 幹員自動攻擊範圍內敵人；近戰幹員可格擋敵人使其停下
7. 敵人被消滅後播放死亡動畫；抵達基地則計入逃脫數
8. 逃脫數 ≥ 4 則任務失敗；所有波次敵人消滅則任務完成
9. 按 P 鍵暫停，按暫停按鈕暫停，可切換 2 倍速
10. Cheat 模式下可無視 DP 限制部署幹員

### 遊戲畫面

[待附截圖 - 登入畫面]
[待附截圖 - 大廳畫面]
[待附截圖 - 關卡選擇畫面]
[待附截圖 - 戰鬥畫面]
[待附截圖 - 任務完成畫面]
[待附截圖 - 任務失敗畫面]

## 程式設計

### 程式架構

本專案採用 **狀態機 + 場景堆疊** 架構，核心流程如下：

```
main.cpp → App (START / UPDATE / END 狀態機)
              │
              └── SceneManager (Singleton，管理場景堆疊)
                    │
                    ├── LoginScene    → 開場動畫、資源預載入
                    ├── LobbyScene     → 大廳、關卡入口
                    ├── StageSelectScene → 關卡選擇
                    ├── LoadingScene   → 載入畫面
                    └── GameScene      → 戰鬥主場景
```

目錄結構：

```
src/Arknights/
├── App.cpp                    # 狀態機主迴圈
├── Core/SceneManager.cpp      # 場景堆疊管理 Singleton
├── Entity/
│   ├── Enemy.cpp              # 敵人（4 種類型、3 狀態、物件池）
│   └── Operator.cpp           # 幹員（7 種子類、攻擊範圍、動畫）
├── Map/
│   ├── Operation.cpp           # 關卡基礎（Homography、WaveManager）
│   ├── Operation1.cpp          # 舊版第一關（已由 StageRepository 取代）
│   ├── StageFactory.cpp        # 工廠模式建立關卡
│   └── StageRepository.cpp    # 資料驅動的 3 關定義
├── Scenes/
│   ├── LoginScene.cpp          # 6 階段登入動畫
│   ├── LobbyScene.cpp          # 大廳（角色滑動、即時時鐘）
│   ├── StageSelectScene.cpp    # 關卡選擇
│   ├── LoadingScene.cpp        # 雙圖載入畫面
│   └── GameScene.cpp           # 戰鬥主邏輯（~991 行）
└── UI/
    ├── Button.cpp              # 懸停/點擊動畫按鈕
    └── SkewSprite.cpp          # 平行四邊形 UI 渲染
```

類別繼承關係：

```
Scene (抽象基類)
├── LoginScene
├── LobbyScene
├── StageSelectScene
├── LoadingScene
└── GameScene

Util::GameObject
├── Operator (基底)
│   ├── Amiya, Chen, Angelina, Red, Eyjafjalla, Texas, Umirin
└── Enemy (含 EnemyPool 物件池)

Util::Drawable
└── SkewSprite (自定義四邊形渲染)
```

### 程式技術

| 技術 | 說明 |
|------|------|
| **設計模式 - Singleton** | SceneManager 為全域唯一場景管理器 |
| **設計模式 - Factory** | StageFactory 根據 StageDefinition 建立 Operation 物件 |
| **設計模式 - Object Pool** | EnemyPool 預配置 64 個 Enemy 物件，避免頻繁記憶體配置 |
| **設計模式 - Observer** | Button 透過 setOnClick 回呼實現事件訂閱 |
| **資料驅動架構** | StageRepository 定義 3 關卡資料（地圖矩陣、路點、波次、校準點），StageFactory 轉換為運行時物件 |
| **Homography 透視變換** | 以 Gauss-Jordan 消去法計算 3×3 透視矩陣，將格子座標映射至螢幕像素，實現等距視角的格子定位 |
| **狀態機** | App（START/UPDATE/END）、Enemy（ALIVE/DYING/DEAD）、Operator（IDLE/ATTACK/DYING/DEAD）、GameScene（多階段勝負流程） |
| **場景堆疊** | SceneManager 以 stack 管理 Scene 生命週期，支援 push/pop/replace，含延遲清理機制 |
| **資源預載入** | LoginScene 在開場動畫期間預建立 GameScene，LoadingScene 無縫銜接 |
| **動畫系統** | PNG 序列幀動畫（偶數幀刪除優化），3 組動畫集（Idle/Attack/Die），方向翻轉 |
| **Cheat Mode** | 切換 Normal/Cheat 模式，Cheat 模式免 DP 部署且不計逃脫數 |

### 使用到 AI/AI Agent 的部分

1. **Copilot Agent**：自動生成 `architecture.md` 程式架構說明文件
2. **AI 輔助 Homography 計算**：關卡地圖的四組對應點（格子座標 ↔ 螢幕像素）由 AI 輔助計算與校準
3. **AI 輔助程式碼撰寫與重構**：部分場景邏輯、戰鬥系統、UI 排版的程式碼由 AI（ChatGPT / Copilot）協助生成與除錯
4. **Gemini**：輔助程式碼撰寫與除錯，用於場景邏輯實作與問題排查
5. **OpenCode (AI Agent)**：以 AI Agent 模式輔助程式碼開發、重構建議與即時除錯

## 結語

### 問題與解決方法

| # | 問題 | 解決方法 |
|---|------|----------|
| 1 | 敵人路徑計算不準確 | 使用 Homography 透視變換，將格子座標精確映射至螢幕像素位置 |
| 2 | 場景切換時資源載入導致卡頓 | 在 LoginScene 動畫播放期間預建立 GameScene，LoadingScene 無縫銜接 |
| 3 | 敵人頻繁建立/銷毀造成效能問題 | 實作 EnemyPool 物件池，預配置 64 個 Enemy 物件重複使用 |
| 4 | 只有單一關卡，新增關卡需改程式碼 | 重構為資料驅動架構，新增關卡只需在 StageRepository 加入定義 |
| 5 | 動畫資源過大造成載入緩慢 | 刪除偶數幀，僅保留奇數幀 PNG，減少一半載入量 |
| 6 | 登入畫面閃爍 | 實作 dither 方式的 crossfade 減少畫面跳動 |
| 7 | 敵人被格擋後不會攻擊 | 新增敵人對幹員的攻擊邏輯與攻擊動畫切換 |
| 8 | 角色攻擊動畫與傷害判定不同步 | 以 damageDelays 延遲列表讓傷害判定與動畫幀同步 |
| 9 | Yokai 敵人需要特殊巡邏路徑（0-3、0-4 關） | 為 Yokai 類型實作獨立的巡邏模式路點生成邏輯 |
| 10 | 遊戲結束時 segfault | 已知問題，資源釋放順序導致，不影響正常遊玩 |

### 自評

| 項次 | 項目 | 完成 |
|------|------|------|
| 1 | 完成專案並可正常遊玩 | V |
| 2 | 完成專案權限改為 public | V |
| 3 | 具有 debug mode 的功能 | V |
| 4 | 解決專案上所有 Memory Leak 的問題 | |
| 5 | 報告中沒有任何錯字，以及沒有任何一項遺漏 | |
| 6 | 報告至少保持基本的美感，人類可讀 | V |

- 項次 4：已使用 EnemyPool 物件池降低配置開銷，但程式結束時仍有 segfault，未完全解決
- 項次 5：遊戲畫面截圖待補

### 心得

本次專案復刻明日方舟的核心塔防玩法，從零開始建構遊戲架構。過程中最大的挑戰是 Homography 透視變換的精確校準——必須讓格子座標完美對齊地圖背景圖的實際位置，反覆調試才達到可接受的精準度。另外，從一開始的單一硬編碼關卡重構為資料驅動架構（StageRepository），也是一個重要的架構決策，讓新增關卡變得非常簡單。

開發過程中兩人分工明確：陳子揚負責基礎玩法系統（敵人路徑、幹員部署、音效、動畫素材），郭東旭負責架構重構與功能擴展（場景系統、戰鬥系統、多關卡、UI 優化）。透過 GitHub 的 branch 和 PR 機制協作，最終順利整合所有功能。

### 貢獻比例

113590008 陳子揚45% ： 113590060 郭東旭55%