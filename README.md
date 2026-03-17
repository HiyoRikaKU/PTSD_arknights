To compile:
cmake build
cmake --build build

# Development timeline

- Week 02：撰寫 Proposal、環境建置
  - [ ] 建立課程框架專案，實作遊戲主迴圈（處理事件 → 更新邏輯 → 繪製畫面）
  - [ ] 開啟視窗，測試基本功能（如渲染）

- Week 03：圖片、音樂收集，圖片載入、輸入處理、音樂實作
  - [ ] 收集幹員與敵人動畫 （至少 2 種職業、3 種敵人）
  - [ ] 建立素材目錄結構，收集地圖背景、地塊圖示、UI、元素、字型
  - [ ] 在視窗載入圖片
  - [ ] 按 `ESC` 關閉視窗，確認鍵盤事件正常

- Week 04：地圖實作
  - [ ] 收集音效（攻擊、死亡、費用不足、勝敗）與 BGM（關卡、主選單各 × 1）
  - [ ] 定義 `TileType` 列舉，建立 15×8 地圖陣列並渲染
  - [ ] 建立 `Enemy` 類別，實作路點（Waypoint）追蹤移動
  
- Week 05：場景管理器
  - [ ] 建立 `Scene` 抽象類別，實作 `MenuScene` 與 `GameScene`
  - [ ] 實作場景切換機制
  
- Week 06：敵人實作
  - [ ] 敵人刷新消失
  - [ ] 敵人路徑移動

- Week 07：幹員架構
  - [ ] 定義 `Operator` 基礎類別，實作 `Guard` 與 `Sniper` 職業
  - [ ] 在地圖上以色塊顯示幹員位置

- Week 08：幹員部署系統
  - [ ] 建立部署選單 UI，實作點選幹員 → 選擇格子 → 選擇朝向 → 部署確認流程
  - [ ] 處理部署合法性判斷（格子種類、是否已佔用）

- Week 09：攻擊範圍、目標鎖定與傷害計算
  - [ ] 以格子偏移定義攻擊範圍，懸停時高亮，實作目標優先度與攻擊計時器
  - [ ] 實作物理／法術傷害公式，繪製血條，敵人死亡後移除並給予費用獎勵

- Week 10：費用（DP）系統與波次生成
  - [ ] 實作費用自動生成、部署消耗、撤退回收與 UI 顯示
  - [ ] 建立 `WaveManager`，依排程生成敵人，最後一波消滅後勝利

- Week 11：動畫系統與素材套入
  - [ ] 建立 `Animator` 類別，套入幹員與敵人動畫
  - [ ] 套入地圖背景圖與地塊圖示，取代色塊

- Week 12：音效整合與粒子效果
  - [ ] 套入音效與 BGM，實作傷害浮字（向上飄動並淡出）

- Week 13：UI 完善
  - [ ] 完整 HUD（費用、基地 HP、波次進度）、幹員屬性面板、暫停選單、2 倍速

- Week 14：勝負判定與結算畫面
  - [ ] 實作勝負判定，結算畫面顯示統計數據與星級評分（1～3 星）
  - [ ] 套入主選單背景圖與開始按鈕

- Week 15：整合測試與 Bug 修正
  - [ ] 完整跑過主選單 → 遊戲 → 結算流程，修復邊界 Bug

- Week 16：優化與文件
  - [ ] 程式碼重構，效能確認（穩定 60 FPS）
  - [ ] 撰寫 `README.md`

- Week 17：提交
  - [ ] 拍攝 Demo 影片
  - [ ] 製作遊戲簡報
  - [ ] 驗收並提交

# PTSD Template

This is a [PTSD](https://github.com/ntut-open-source-club/practical-tools-for-simple-design) framework template for students taking OOPL2024s.

## Quick Start

1. Use this template to create a new repository
   ![github screenshot](https://github.com/ntut-rick/ptsd-template/assets/126899559/ef62242f-03ed-481d-b858-12b730c09beb)

2. Clone your repository

   ```bash
   git clone YOUR_GIT_URL --recursive
   ```

3. Build your project

  > [!WARNING]
  > Please build your project in `Debug` because our `Release` path is broken D:
   
   ```sh
   cmake -DCMAKE_BUILD_TYPE=Debug -B build # -G Ninja
   ```
   better read [PTSD README](https://github.com/ntut-open-source-club/practical-tools-for-simple-design)
