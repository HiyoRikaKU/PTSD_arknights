# 2026 OOPL Final Report

## 組別資訊

組別：第七組

組員：<br>
113590008 陳子揚<br>
113590060 郭東旭

復刻遊戲：明日方舟（Arknights）

---

## 專案簡介

### 遊戲簡介

明日方舟（Arknights）是一款策略塔防手機遊戲。玩家在等距透視地圖上部署幹員，阻擋沿固定路線前進的敵人，在 DP（部署費用）的限制下運用不同幹員的技能與範圍，守住基地不讓敵人通過。

本次復刻保留原作的核心玩法機制，包含：

- **幹員部署**：從底部面板拖放幹員至地圖格子，放置後選擇朝向（上／下／左／右），幹員自動攻擊射程內的敵人
- **敵人系統**：4 種敵人類型（gopro、bigbo、trslim、yokai），沿路點行走；近戰幹員可格擋敵人使其停步並展開對攻
- **DP 系統**：初始 10 DP，每秒自動 +1，各幹員部署消耗不同費用
- **波次系統**：每關依時間軸排定 5 波敵人，由 WaveManager 統一排程生成
- **勝負判定**：消滅全部波次敵人→任務完成；≥ 4 名敵人抵達終點→任務失敗
- **4 個關卡**
- **7 位幹員**：Amiya、Chen、Angelina、Red、Eyjafjalla、Texas、Umirin

### 組別分工

| 組員 | 學號 | 主要負責項目 |
|------|------|-------------|
| 陳子揚  | 113590008 | 專案初始架構與 App 主迴圈；敵人移動與路徑系統（Homography 透視變換）；幹員拖放部署系統；7 位幹員類別實作與屬性設計；登入畫面、戰鬥場景BGM；角色動畫素材收集與處理（偶數幀刪除優化）；EnemyPool 物件池；WaveManager 波次排程；Operation 1 初始關卡定義；報告初稿|
| 郭東旭  | 113590060 | 資料驅動架構重構（StageRepository／StageFactory／StageDefinition）；場景系統（LoginScene／LobbyScene／StageSelectScene／LoadingScene／GameScene）；SceneManager Singleton；敵人對幹員戰鬥系統與傷害動畫同步；關卡 0-2 ～ 0-4 資料定義與素材；Yokai 巡邏型敵人路點邏輯；Cheat／Normal 模式切換；任務完成／失敗多階段狀態機；遊戲UI介面復刻；資源預載入與登入動畫；大廳 BGM、角色語音、按鈕點擊 音效整合；UI 細節（暫停／2 倍速／Cost 顯示）；SkewSprite 自定義四邊形渲染器；拍攝影片+上字幕；修改撰寫報告|

---

## 遊戲介紹

### 遊戲規則

**1. 開場流程**

遊戲啟動後進入登入畫面，播放 9 階段開場動畫（Splash × → 登入圖  → 幀動畫）後進入大廳。大廳顯示目前時間與隨機幹員立繪，點擊「終端」進入關卡選擇畫面。

**2. 部署幹員**

- 從底部面板將幹員拖放至地圖格子，近戰幹員只能放在地面格（`GROUND`），遠程幹員只能放在高地格（`HIGH_GROUND`）
- 放置後依照選擇器，選擇朝向後幹員正式部署並開始自動攻擊
- 放置後右邊會出現幹員的屬性、HP和類型

**3. DP 系統**

初始 10 DP，每秒 +1，每位幹員有不同部署費用；Cheat 模式下可無視 DP 限制部署任意幹員。

**4. 戰鬥**

- 幹員自動攻擊射程內的敵人，傷害判定與攻擊動畫幀同步（`m_DamageDelays`）
- 近戰幹員可格擋敵人，使敵人停步並反擊幹員；被格擋的敵人會播放攻擊動畫並對幹員扣血
- 敵人死亡後播放死亡動畫，由 EnemyPool 回收；抵達終點則計入逃脫數

**5. 波次與勝負**

- WaveManager 依時間軸（毫秒）排程每波敵人的出現時機與屬性（HP、速度、類型）
- 塔血量 = 0  → 任務失敗；全波次敵人消滅 → 任務完成
- 任務結束後進入多階段結果畫面（VICTORY_SLIDE → VICTORY_SHOW_WIN 或 FAILURE_SHOW）

**6. 其他操作**

- 按畫面右上方的 `play/pause` 按鈕，可以暫停遊戲
- 按畫面右上方的 `×1` / `×2` 按鈕，可以 1 倍速或 2 倍速模式切換
- 按畫面右上方的 `settings(齒輪)` 按鈕，可以切換 `Cheat Mode` 與 `Normal Mode`，以及中途放棄遊戲
  - `Cheat Mode` 切換可免費部署，並不計點數，即使敵人衝進塔內，防守血量也不會扣

### 遊戲畫面

載入畫面<br>
![載入畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/UI/login_loading/LoginScene1.png)

登入畫面<br>
![載入畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/UI/login_animation/ezgif-frame-022.jpg)

進到大廳畫面<br>
![進到大廳畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/lobby1.png)

進到大廳畫面<br>
![進到大廳畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/lobby.png)

關卡選擇畫面<br>
![關卡選擇畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/selection.png)

戰鬥畫面<br>
![戰鬥畫面 0-3](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/operation.png)

暫停畫面<br>
![暫停畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/pause.png)

遊戲內設定畫面<br>
![遊戲內設定畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/settings.png)

放棄遊戲畫面<br>
![放棄遊戲畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/giveup.png)

任務完成畫面<br>
![任務完成畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/win.png)

任務失敗畫面<br>
![任務失敗畫面](https://raw.githubusercontent.com/HiyoRikaKU/PTSD_arknights/main/Resources/img/fail.png)

---

## 程式設計

### 程式架構

本專案採用**狀態機 + 場景堆疊**架構，主迴圈由 `App` 驅動，場景生命週期由 `SceneManager` Singleton 統一管理：

```
main.cpp
  └── App（START / UPDATE / END 狀態機）
        └── SceneManager（Singleton，std::stack<Scene>）
              ├── LoginScene          開場動畫、資源預載入
              ├── LobbyScene          大廳、立繪切換、終端入口
              ├── ZoneScene           章節地圖選擇（Evil Time 入口）
              ├── StageSelectScene    關卡選擇
              ├── LoadingScene        雙圖載入過渡（→ GameScene）
              ├── GameScene           戰鬥主場景（~1000 行）
              └── ReturnLoadingScene  戰鬥結束後返回大廳的載入過渡
```

目錄結構：

```
src/Arknights/
├── App.cpp
├── Core/
│   └── SceneManager.cpp
├── Entity/
│   ├── Enemy.cpp
│   └── Operator.cpp
├── Map/
│   ├── Operation.cpp
│   ├── Operation1.cpp
│   ├── StageFactory.cpp
│   └── StageRepository.cpp
├── Scenes/
│   ├── Scene.cpp
│   ├── LoginScene.cpp
│   ├── LobbyScene.cpp
│   ├── ZoneScene.cpp
│   ├── StageSelectScene.cpp
│   ├── LoadingScene.cpp
│   ├── GameScene.cpp
│   └── ReturnLoadingScene.cpp
└── UI/
    ├── Button.cpp
    ├── FadeImage.cpp
    └── SkewSprite.cpp

include/Arknights/
├── Core/
│   ├── InputAdapter.hpp
│   └── SceneManager.hpp
├── Entity/
│   ├── Enemy.hpp
│   └── Operator.hpp
├── Map/
│   ├── Operation.hpp
│   ├── Operation1.hpp
│   ├── StageDefinition.hpp
│   ├── StageFactory.hpp
│   └── StageRepository.hpp
├── Scenes/
│   ├── Scene.hpp
│   ├── LoginScene.hpp
│   ├── LobbyScene.hpp
│   ├── ZoneScene.hpp
│   ├── StageSelectScene.hpp
│   ├── LoadingScene.hpp
│   ├── GameScene.hpp
│   └── ReturnLoadingScene.hpp
└── UI/
    ├── Button.hpp
    ├── FadeImage.hpp
    ├── HealthBar.hpp
    └── SkewSprite.hpp
```

類別繼承關係：

```
Scene（抽象基類）
├── LoginScene
├── LobbyScene
├── ZoneScene
├── StageSelectScene
├── LoadingScene
├── GameScene
└── ReturnLoadingScene

Util::GameObject
├── Operator（基底）
│   └── Amiya / Chen / Angelina / Red / Eyjafjalla / Texas / Umirin
└── Enemy（含 EnemyPool 物件池）

Core::Drawable
├── SkewSprite（自定義任意四邊形渲染）
└── FadeImage（帶 alpha 控制的淡入淡出圖片渲染）
```

---

### 程式技術

**1. 設計模式 — Singleton（SceneManager）**

`SceneManager` 以 Singleton 維護唯一的場景堆疊，所有場景切換（push／pop／replace）皆透過它執行，並實作延遲清理機制（`m_DeferredCleanupScenes`），防止在 `update()` 迴圈中途釋放正在使用的物件。

```cpp
class SceneManager {
public:
    static SceneManager& getInstance();

    void pushScene(std::shared_ptr<Scene> scene);
    void popScene();
    void replaceScene(std::shared_ptr<Scene> scene);

    void setPreparedGameScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<Scene> takePreparedGameScene();

private:
    SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    std::stack<std::shared_ptr<Scene>> m_SceneStack;
    std::shared_ptr<Scene> m_PreparedGameScene = nullptr;
    std::vector<std::shared_ptr<Scene>> m_DeferredCleanupScenes;
    bool m_IsUpdating = false;
};
```

**2. 設計模式 — Factory（StageFactory）**

`StageFactory` 從 `StageDefinition` 資料結構建立 `Operation` 運行時物件，負責初始化地圖、路點、Homography 校準點與波次排程，讓呼叫端只需傳入關卡 ID 即可取得完整的關卡物件。

```cpp
class StageFactory {
public:
    static std::unique_ptr<Operation> createOperation(const StageDefinition& def);
    static std::unique_ptr<Operation> createOperationByStageId(const std::string& stageId);
};
```

```cpp
std::unique_ptr<Operation> StageFactory::createOperation(const StageDefinition& def) {
    auto operation = std::make_unique<Operation>(
        def.mapImagePath,
        def.waypoints,
        def.tileMatrix,
        def.homography.srcPoints,
        def.homography.dstPoints
    );
    auto& waveManager = operation->getWaveManager();
    for (const auto& event : def.wave) {
        waveManager.addSpawnEvent(event.timestamp, event.enemyType, event.hp, event.speed);
    }
    return operation;
}
```

**3. 設計模式 — Object Pool（EnemyPool）**

`EnemyPool` 預先配置固定數量的 `Enemy` 物件，以 `getEnemy()` / `returnEnemy()` 取用與歸還，避免每波敵人頻繁執行 `new`／`delete` 造成的效能問題。

```cpp
// GameScene 初始化
m_EnemyPool = std::make_unique<EnemyPool>(
    ENEMY_POOL_SIZE,
    m_EnemyAnimationPathsGopro,
    m_CurrentOperation->getWaypoints()
);

// 從池中借用敵人
Enemy* enemy = m_EnemyPool->getEnemy();

// 死亡或到達終點後歸還
m_EnemyPool->returnEnemy(enemy);
```

**4. WaveManager 波次排程系統**

`WaveManager` 以時間軸（毫秒）管理所有敵人的生成排程。`SpawnEvent` 記錄每筆生成事件的時間戳、敵人類型、HP 與速度；`shouldSpawn()` 每幀被 GameScene 的遊戲計時器輪詢，滿足時間條件時輸出下一筆事件並推進索引，讓 GameScene 只需一個 while 迴圈即可在同一幀生成多隻敵人。

```cpp
struct SpawnEvent {
    float timestamp;
    std::string enemyType;
    float hp;
    float speed;
};

class WaveManager {
public:
    void addSpawnEvent(float timestamp, const std::string& enemyType,
                       float hp, float speed) {
        m_Timeline.push_back({timestamp, enemyType, hp, speed});
    }

    bool shouldSpawn(float currentTime, SpawnEvent& outEvent) {
        if (m_CurrentEventIndex < m_Timeline.size() &&
            currentTime >= m_Timeline[m_CurrentEventIndex].timestamp) {
            outEvent = m_Timeline[m_CurrentEventIndex++];
            return true;
        }
        return false;
    }

    bool isAllSpawned() const { return m_CurrentEventIndex >= m_Timeline.size(); }
    void reset() { m_CurrentEventIndex = 0; }

private:
    std::vector<SpawnEvent> m_Timeline;
    std::size_t m_CurrentEventIndex = 0;
};
```

GameScene 每幀在同一個 while 迴圈消費所有到期事件，並在全數生成完且場上無存活敵人時觸發勝利判定：

```cpp
SpawnEvent event;
while (m_CurrentOperation->getWaveManager().shouldSpawn(m_WaveTimer, event)) {
    spawnEnemy(event);
}

if (m_CurrentOperation->getWaveManager().isAllSpawned() && m_ActiveEnemies.empty()) {
    beginVictorySequence();
}
```

**5. 幹員繼承體系與攻擊範圍設計**

`Operator` 為所有幹員的基底類別，封裝生命值、攻擊力、部署費用、格擋數、三組動畫（Idle／Attack／Die）與傷害同步計時器。7 位幹員各繼承 `Operator` 並在建構子中設定個人屬性，包含各角色不同的攻擊範圍邏輯。

```cpp
class Operator : public Util::GameObject {
public:
    enum class State  { IDLE, ATTACK, SKILL, DYING, DEAD };
    enum class Direction { UP, DOWN, LEFT, RIGHT };
    enum class Type { AMIYA, CHEN, ANGELINA, RED, EYJAFJALLA, TEXAS, UMIRIN };

    Operator(Type type, float hp, float attack);
    virtual void update(float deltaTime);
    bool isInAttackRange(const glm::vec2& enemyGridPos) const;

protected:
    float m_Hp, m_MaxHp, m_Attack;
    int   m_DeploymentCost = 10;
    float m_AttackInterval = 1.0f;
    std::size_t m_BlockCount = 1;
    std::vector<float> m_DamageDelays = {0.5f};
    std::shared_ptr<Util::Animation> m_IdleAnimation;
    std::shared_ptr<Util::Animation> m_AttackAnimation;
    std::shared_ptr<Util::Animation> m_DieAnimation;
};

class Amiya    : public Operator { public: Amiya();    };
class Chen     : public Operator { public: Chen();     };
class Angelina : public Operator { public: Angelina(); };
class Red      : public Operator { public: Red();      };
class Eyjafjalla : public Operator { public: Eyjafjalla(); };
class Texas    : public Operator { public: Texas();    };
class Umirin   : public Operator { public: Umirin();   };
```

各幹員屬性差異（以建構子設定為準）：

| 幹員 | HP | 攻擊 | 費用 | 攻擊間隔 | 格擋數 | 特性 |
|------|----|------|------|---------|--------|------|
| Amiya | 1000 | 120 | 10 | 1.6s | 1 | 遠程，扇形 3×3 射程 |
| Chen | 1200 | 80×2 | 18 | 1.3s | 2 | 近戰，連擊兩刀 |
| Angelina | 1100 | 100 | 14 | 1.9s | 1 | 遠程，大範圍 3×4 |
| Red | 900 | 150 | 6 | 0.93s | 1 | 近戰，最快攻速 |
| Eyjafjalla | 1200 | 200 | 20 | 1.6s | 1 | 遠程，最高攻擊 |
| Texas | 1100 | 100 | 12 | 1.05s | 2 | 近戰，雙格擋 |
| Umirin | 1000 | 120 | 10 | 1.6s | 1 | 遠程，十字延伸射程 |

`isInAttackRange()` 將敵人格座標轉換為以幹員為中心、朝向對齊的局部座標系（以 `Direction` 旋轉 dr/dc），各幹員再對局部座標做獨立的範圍判斷：

```cpp
bool Operator::isInAttackRange(const glm::vec2& enemyGridPos) const {
    float dr = enemyGridPos.x - m_GridPosition.x;
    float dc = enemyGridPos.y - m_GridPosition.y;
    float tr, tc;
    switch (m_Direction) {
        case Direction::RIGHT: tr =  dr; tc =  dc; break;
        case Direction::LEFT:  tr = -dr; tc = -dc; break;
        case Direction::UP:    tr =  dc; tc = -dr; break;
        case Direction::DOWN:  tr = -dc; tc =  dr; break;
    }
    int itr = static_cast<int>(std::round(tr));
    int itc = static_cast<int>(std::round(tc));

    if (m_Type == Type::AMIYA)
        return (itr == 0 && itc <= 3) || (std::abs(itr) == 1 && itc <= 2);
    if (m_Type == Type::CHEN || m_Type == Type::RED || m_Type == Type::TEXAS)
        return itr == 0 && (itc == 0 || itc == 1);
    if (m_Type == Type::ANGELINA)
        return itr >= -1 && itr <= 1 && itc >= -1 && itc <= 2;
    if (m_Type == Type::EYJAFJALLA)
        return itr >= -1 && itr <= 1 && itc >= 0 && itc <= 2;
    if (m_Type == Type::UMIRIN)
        return (itr == 0 && itc >= -1 && itc <= 3) ||
               (std::abs(itr) == 1 && itc >= -1 && itc <= 2);
    return false;
}
```

**6. 設計模式 — Observer（Button 回呼）**

`Button` 透過 `setOnClick` 接受 lambda 回呼，實現事件訂閱，讓 UI 元素與遊戲邏輯解耦。

```cpp
m_CheatModeButton->setOnClick([this]() {
    setCheatMode(true);
});

m_NormalModeButton->setOnClick([this]() {
    setCheatMode(false);
});
```

**7. 資料驅動架構（StageRepository）**

關卡的所有靜態資料——地圖矩陣（`TileType` 二維陣列）、路點序列、Homography 校準點、波次排程——集中定義於 `StageRepository`，以純資料結構描述，新增關卡不須修改任何遊戲邏輯程式碼。

```cpp
struct StageDefinition {
    std::string id;
    std::string mapImagePath;
    std::vector<std::vector<Operation::TileType>> tileMatrix;
    std::vector<glm::vec2> waypoints;
    StageHomographyDefinition homography;
    std::vector<SpawnEvent> wave;
};
```

```cpp
constexpr Operation::TileType x = Operation::TileType::OBSTACLE;
constexpr Operation::TileType f = Operation::TileType::GROUND;
constexpr Operation::TileType h = Operation::TileType::HIGH_GROUND;
constexpr Operation::TileType r = Operation::TileType::SPAWN;
constexpr Operation::TileType b = Operation::TileType::BASE;

const std::vector<std::vector<Operation::TileType>> s_Stage03MapMatrix = {
    {x, x, x, x, x, x, x, x, x, x},
    {x, h, h, h, h, h, h, h, h, x},
    {r, h, f, f, h, f, f, f, f, x},
    {r, f, f, f, f, f, h, h, f, x},
    {x, h, h, h, h, h, h, h, f, x},
    {x, x, x, x, x, x, x, x, b, x},
};

const std::vector<SpawnEvent> s_Stage03Wave = {
    { 2000.0f, "trslim", 100.0f, 0.00126f},
    { 9000.0f, "trslim", 110.0f, 0.00126f},
    {11500.0f, "bigbo",  420.0f, 0.00035f},
    {20500.0f, "yokai",  360.0f, 0.00084f},
    // ...
};
```

**8. Homography 透視變換（Gauss-Jordan 消去法）**

等距透視地圖的格子座標無法以簡單縮放映射至螢幕像素，因此以 4 組已知的格子↔螢幕座標對應點求解 3×3 透視矩陣 **H**。矩陣的 8 個未知數透過 Gauss-Jordan 消去法求解，後續格子定位只需一次矩陣乘法。

```cpp
void Operation::computeHomography(
    const std::array<glm::vec2, 4>& src,
    const std::array<glm::vec2, 4>& dst)
{
    double A[8][9];
    for (int i = 0; i < 4; ++i) {
        float xs = src[i].x, ys = src[i].y;
        float xd = dst[i].x, yd = dst[i].y;
        A[i*2]   = {xs, ys, 1, 0, 0, 0, -xs*xd, -ys*xd, xd};
        A[i*2+1] = {0,  0,  0, xs, ys, 1, -xs*yd, -ys*yd, yd};
    }
    // Gauss-Jordan 消去...
    m_Homography    = glm::mat3(h[0],h[3],h[6], h[1],h[4],h[7], h[2],h[5],1.0f);
    m_InvHomography = glm::inverse(m_Homography);
}

glm::vec2 Operation::getTileCenterPos(float r, float c) const {
    glm::vec3 p = m_Homography * glm::vec3(c, r, 1.0f);
    return {p.x / p.z, p.y / p.z};
}
```

**9. 狀態機**

專案中多處使用狀態機管理生命週期：

| 類別 | 狀態 |
|------|------|
| `App` | `START` / `UPDATE` / `END` |
| `Enemy` | `ALIVE` / `DYING` / `DEAD` |
| `Operator` | `IDLE` / `ATTACK` / `DYING` / `DEAD` |
| `GameScene` | `ResultPhase::NONE` / `VICTORY_SLIDE` / `VICTORY_WAIT_BEFORE_WIN` / `VICTORY_SHOW_WIN` / `FAILURE_SHOW` |
| `LoginScene` | `SPLASH1_HOLD` → `SPLASH12_FADE` → ... → `LOGIN_ANIMATION`（9 階段） |

```cpp
enum class State {
    IDLE,
    ATTACK,
    DYING,
    DEAD
};

enum class ResultPhase {
    NONE,
    VICTORY_SLIDE,
    VICTORY_WAIT_BEFORE_WIN,
    VICTORY_SHOW_WIN,
    FAILURE_SHOW
};
```

**10. 場景堆疊（Scene Stack）**

`SceneManager` 以 `std::stack<std::shared_ptr<Scene>>` 管理場景，支援 `push`／`pop`／`replace` 三種切換模式，並在 `update()` 執行期間將待清理場景暫存至 `m_DeferredCleanupScenes`，確保不會在迭代中途釋放物件。

```cpp
void SceneManager::popScene() {
    m_SceneStack.top()->onExit();
    cleanupScene(m_SceneStack.top());
    m_SceneStack.pop();
    if (!m_SceneStack.empty())
        m_SceneStack.top()->onEnter();
}

void SceneManager::cleanupScene(const std::shared_ptr<Scene>& scene) {
    if (m_IsUpdating)
        m_DeferredCleanupScenes.push_back(scene);
    else
        scene->cleanup();
}
```

**11. 資源預載入**

`LoginScene` 在播放開場動畫期間，於背景建立 `GameScene` 物件並存入 `SceneManager::m_PreparedGameScene`，使 `LoadingScene` 能無縫取用已就緒的場景，消除切換時的卡頓。

```cpp
// LoginScene 動畫播放中
void LoginScene::update(float deltaTime) {
    // ...動畫推進...
    if (phase == Phase::LOGIN_ANIMATION && !m_GameScenePrepared) {
        auto gameScene = std::make_shared<GameScene>();
        SceneManager::getInstance().setPreparedGameScene(gameScene);
        m_GameScenePrepared = true;
    }
}

// LoadingScene 切換時直接取用
auto prepared = SceneManager::getInstance().takePreparedGameScene();
SceneManager::getInstance().replaceScene(prepared);
```

**12. 登入動畫（Dither Crossfade）**

`LoginScene` 實作 9 階段狀態機（`SPLASH1_HOLD` → `SPLASH12_FADE` → `SPLASH2_HOLD` → ... → `LOGIN_ANIMATION`），畫面過渡採用 dither 方式的 crossfade，以 `FADE_DITHER_TICK_MS = 40ms` 為單位逐格調整透明度，解決直接 alpha 混合造成的閃爍問題。

```cpp
enum class Phase {
    SPLASH1_HOLD, SPLASH12_FADE,
    SPLASH2_HOLD, SPLASH23_FADE,
    SPLASH3_HOLD,
    LOGIN1_SHOW, LOGIN2_SHOW, LOGIN3_SHOW,
    LOGIN_ANIMATION
};

static constexpr float SPLASH_HOLD_MS          = 2600.0f;
static constexpr float SPLASH_FADE_MS          = 40.0f;
static constexpr int   LOGIN_ANIMATION_INTERVAL_MS = 67;
static constexpr float FADE_DITHER_TICK_MS     = 40.0f;
```

**13. 動畫同步傷害（m_DamageDelays）**

幹員攻擊動畫啟動後，傷害判定不應立即生效，而需等到動畫打擊幀才算數。每位幹員維護一個 `m_DamageDelays` 向量，記錄動畫開始後各次傷害的觸發時間點（秒），由計時器驅動 `m_DamageReady` 旗標，讓 GameScene 在正確時機扣血。

```cpp
// Operator.hpp
std::vector<float> m_DamageDelays = {0.5f};
float m_DamageTimer = 0.0f;
bool  m_DamageReady = false;

bool consumeDamageTrigger() {
    if (m_DamageReady) { m_DamageReady = false; return true; }
    return false;
}
```

```cpp
// Operator.cpp — 各幹員的打擊幀設定
// Chen 連擊兩下，各有獨立打擊時間點
m_DamageDelays = {0.4f, 0.6f};

// Angelina 攻擊前搖較長
m_DamageDelays = {0.7f};

// Red 前搖極短
m_DamageDelays = {0.3f};
```

**14. SkewSprite 自定義四邊形渲染**

明日方舟大廳 UI 中的斜體文字與平行四邊形按鈕無法以一般矩形 Sprite 渲染，因此實作 `SkewSprite`，繼承 `Core::Drawable`，以 OpenGL VBO 直接指定任意四頂點座標（`Quad`）與 UV，支援從 Atlas 紋理裁切指定區域（`AtlasRect`）並以自定義四邊形輸出。

```cpp
struct AtlasRect { float x, y, w, h; };
struct Quad {
    glm::vec2 topLeft, topRight, bottomRight, bottomLeft;
};

class SkewSprite final : public ::Core::Drawable {
public:
    SkewSprite(const std::string& atlasPath,
               const AtlasRect& sourceRect,
               const Quad& worldQuad);
    void Draw(const ::Core::Matrices& data) override;
private:
    void InitVertexArray(const AtlasRect& src, const Quad& quad);
    std::unique_ptr<::Core::VertexArray>  m_VertexArray;
    std::unique_ptr<::Core::Texture>      m_Texture;
    glm::vec2 m_AtlasSize = {1.0f, 1.0f};
};
```

```cpp
void SkewSprite::InitVertexArray(const AtlasRect& src, const Quad& quad) {
    const float u0 = src.x / m_AtlasSize.x, v0 = src.y / m_AtlasSize.y;
    const float u1 = (src.x + src.w) / m_AtlasSize.x;
    const float v1 = (src.y + src.h) / m_AtlasSize.y;

    m_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            quad.topLeft.x,     quad.topLeft.y,
            quad.bottomLeft.x,  quad.bottomLeft.y,
            quad.bottomRight.x, quad.bottomRight.y,
            quad.topRight.x,    quad.topRight.y,
        }, 2));

    m_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{ u0,v0, u0,v1, u1,v1, u1,v0 }, 2));

    m_VertexArray->SetIndexBuffer(std::make_unique<Core::IndexBuffer>(
        std::vector<unsigned int>{ 0,1,2, 0,2,3 }));
}
```

**15. 大廳立繪滑動（LobbyScene）**

大廳左右切換幹員立繪時採平滑滑動補間，以 `glm::clamp` 限制插值進度，方向由 `m_CharacterSlideDirection`（+1 / -1）控制，避免直接跳幀切換。

```cpp
void LobbyScene::updateCharacterSlide(float deltaTime) {
    m_CharacterSlideTimer += deltaTime;
    const float t = glm::clamp(
        m_CharacterSlideTimer / CHARACTER_SLIDE_DURATION_MS, 0.0f, 1.0f);
    const float offset =
        CHARACTER_SLIDE_DISTANCE * t * static_cast<float>(m_CharacterSlideDirection);

    m_CharacterArts[m_CurrentCharacterIndex]->m_Transform.translation =
        {m_CharacterBasePos.x - offset, m_CharacterBasePos.y};
    m_CharacterArts[m_NextCharacterIndex]->m_Transform.translation = {
        m_CharacterBasePos.x
            + CHARACTER_SLIDE_DISTANCE * static_cast<float>(m_CharacterSlideDirection)
            - offset,
        m_CharacterBasePos.y
    };
}
```

**16. AI 工具使用**

| 工具 | 用途 |
|------|------|
| GitHub Copilot Agent | 自動生成 `architecture.md` 程式架構說明文件（獨立 branch） |
| Copilot / ChatGPT | 輔助場景邏輯、戰鬥系統、UI 排版的程式碼生成與除錯 |
| Gemini | 場景邏輯實作與問題排查 |
| OpenCode (AI Agent) | 以 Agent 模式進行程式碼重構建議與即時除錯 |
| AI 輔助 Homography 校準 | 各關卡地圖的 4 組對應點（格子↔螢幕像素）由 AI 輔助計算與微調 |

---

## 結語

### 問題與解決方法

| # | 問題 | 解決方法 |
|---|------|----------|
| 1 | 敵人路徑計算不準確 | 以 Gauss-Jordan 消去法實作 Homography，將格子座標精確映射至螢幕像素 |
| 2 | 場景切換時資源載入造成卡頓 | LoginScene 動畫播放期間預建立 GameScene 並存入 SceneManager，LoadingScene 無縫銜接 |
| 3 | 敵人頻繁建立／銷毀造成效能問題 | 實作 EnemyPool 物件池，預配置固定數量 Enemy 物件重複使用 |
| 4 | 單一關卡、新增需改程式碼 | 重構為資料驅動架構（StageRepository／StageFactory），新增關卡只需在 Repository 加入 `StageDefinition` |
| 5 | 動畫資源龐大、載入緩慢 | 刪除偶數幀，僅保留奇數幀 PNG，減少約一半的載入量 |
| 6 | 登入畫面過渡閃爍 | 改採 dither crossfade，以 40ms 為單位逐格調整透明度 |
| 7 | 敵人被格擋後不攻擊 | 新增敵人對幹員的攻擊邏輯，敵人停步後切換攻擊動畫並扣幹員血量 |
| 8 | 角色攻擊動畫與傷害判定不同步 | 以 `m_DamageDelays` 向量記錄各幀打擊時間點，計時器驅動 `m_DamageReady` 旗標，傷害判定與動畫幀同步 |
| 9 | Yokai 需要特殊巡邏路徑 | 為 Yokai 型別實作獨立的巡邏路點生成邏輯（0-3、0-4 關） |
| 10 | 遊戲結束時 segfault | 已知問題，資源釋放順序導致，EnemyPool 已降低頻繁配置開銷，但程式結束時仍未完全解決 |

### 自評

| 項次 | 項目 | 完成 |
|------|------|------|
| 1 | 完成專案並可正常遊玩 | ✓ |
| 2 | 完成後將專案權限改為 Public | ✓ |
| 3 | 具有 debug / cheat mode 功能 | ✓ |
| 4 | 解決所有 Memory Leak 問題 | ✓ |
| 5 | 報告無錯字且無遺漏項目 | ✓ |
| 6 | 報告保持基本美感，人類可讀 | ✓ |
| 7 | 遊戲畫面截圖 | ✓ |
| 8 | 透過 EnemyPool 降低配置開銷，但程式結束時仍有 segfault（資源釋放順序問題） | △ |

### 心得

本次專案以 C++ 搭配 OpenGL／SDL2 從零復刻明日方舟的核心塔防玩法，最大的技術挑戰是 **Homography 透視變換的精確校準**——必須讓格子座標完美對齊地圖背景的實際像素位置，反覆調試才達到可接受的精準度。另一個重要的架構決策是將原本硬編碼的單一關卡**重構為資料驅動架構**（StageRepository），讓新增關卡變為純資料的工作，大幅縮短第 2、3 關的開發時間。

開發過程中兩人分工明確：陳子揚負責奠定基礎玩法系統（敵人路徑、幹員部署與格擋、音效、角色動畫素材整理），郭東旭負責架構重構與功能擴展（全部場景系統、資料驅動、SkewSprite 自定義四邊形渲染器、多關卡定義、任務完成／失敗多階段狀態機、UI 細節、音效、場景動畫、最終 bug 修復）。協作透過 GitHub 的 `branch` / `branch_hiyori` 雙分支 + PR 進行，最終順利整合所有功能並完成發布打包。

### 貢獻比例

| 編號 | 學號 | 姓名 | 貢獻比例 |
|------|------|------|---------|
| 1 | 113590008 | 陳子揚 | 40% |
| 2 | 113590060 | 郭東旭 | 60% |


### 打包好的遊戲檔案
請打開`PTSD_Arknights_Release`
