# SkewSprite UI Reconstruction Progress

## Goal

重現 Arknights Home / Recruit 類型 UI 時，單純使用 Figma 的 `X, Y, W, H` 只能處理矩形的平移與縮放。若 UI component 本身是被斜切成平行四邊形，`translation + scale + rotation` 不足以精準還原。

目前方向是新增一個 `SkewSprite` drawable：

- atlas crop 只負責指定來源圖上的矩形區域。
- Figma 量測值改為四個角座標。
- renderer 用四個 vertex position 搭配 atlas UV，直接把 crop 出來的圖貼到任意四邊形上。

## Key Idea

不要讓 crop 負責變形角度。

```text
atlas crop = 來源圖上的 x, y, w, h
Figma quad = canvas 上的 topLeft, topRight, bottomRight, bottomLeft
SkewSprite = source rect UV + destination quad vertices
```

這樣同一個 atlas 區塊可以放到 1600x900 canvas 上任意位置，也可以自由形成平行四邊形。

## Current Implementation

已新增：

- `include/Arknights/UI/SkewSprite.hpp`
- `src/Arknights/UI/SkewSprite.cpp`
- `files.cmake` 已加入 `SkewSprite.cpp` / `SkewSprite.hpp`
- `CMakeLists.txt` 已替主專案 target 補上 `PTSD_ASSETS_DIR`，讓 `SkewSprite` 可以使用 PTSD 內建 `Base.vert` / `Base.frag`

### Public Types

```cpp
struct AtlasRect {
    float x;
    float y;
    float w;
    float h;
};

struct Quad {
    glm::vec2 topLeft;
    glm::vec2 topRight;
    glm::vec2 bottomRight;
    glm::vec2 bottomLeft;
};
```

### Figma Coordinate Helpers

```cpp
glm::vec2 FigmaToWorld(glm::vec2 point, float screenW = 1600.0f, float screenH = 900.0f);
Quad FigmaQuadToWorld(const Quad& quad, float screenW = 1600.0f, float screenH = 900.0f);
```

Figma 座標系：

```text
origin = top-left
Y down
```

PTSD 座標系：

```text
origin = screen center
Y up
```

轉換公式：

```cpp
world.x = figma.x - screenW / 2
world.y = -(figma.y - screenH / 2)
```

## Usage Example

```cpp
#include "Arknights/UI/SkewSprite.hpp"

using namespace Arknights::UI;

AtlasRect src {
    8.0f,    // atlas x
    238.0f,  // atlas y
    582.0f,  // atlas w
    64.0f    // atlas h
};

Quad figmaQuad {
    {1128.0f, 632.0f}, // topLeft
    {1392.0f, 632.0f}, // topRight
    {1392.0f, 792.0f}, // bottomRight
    {1128.0f, 792.0f}  // bottomLeft
};

auto sprite = std::make_shared<SkewSprite>(
    std::string(RESOURCE_DIR) + "/img/UI_RECRUIT_BASE_STATES.png",
    src,
    FigmaQuadToWorld(figmaQuad)
);

auto obj = std::make_shared<Util::GameObject>(sprite, 20.0f);
m_Root.AddChild(obj);
```

`SkewSprite` 的 vertex 已經是 world position，所以這類物件通常不需要再設定：

```cpp
obj->m_Transform.translation
obj->m_Transform.scale
obj->m_Transform.rotation
```

保持預設即可。

## How To Implement After Measuring

你量完 atlas 裡要放進 canvas 的 component 後，需要準備兩組資料：

### 1. Atlas Source Rect

這是來源圖 `UI_RECRUIT_BASE_STATES.png` 裡的 crop 範圍。

```text
atlasX = component 左上角 X
atlasY = component 左上角 Y
atlasW = component 寬度
atlasH = component 高度
```

填進：

```cpp
Arknights::UI::AtlasRect src {
    atlasX,
    atlasY,
    atlasW,
    atlasH
};
```

注意：這裡永遠是 atlas 圖上的水平矩形 crop，不需要也不應該在這裡處理 skew 角度。

### 2. Figma Destination Quad

這是你希望它出現在 1600x900 canvas 上的位置。請在 Figma 量四個頂點：

```text
topLeft     = 左上角
topRight    = 右上角
bottomRight = 右下角
bottomLeft  = 左下角
```

填進：

```cpp
Arknights::UI::Quad figmaQuad {
    {topLeftX, topLeftY},
    {topRightX, topRightY},
    {bottomRightX, bottomRightY},
    {bottomLeftX, bottomLeftY}
};
```

然後建立 sprite：

```cpp
auto sprite = std::make_shared<Arknights::UI::SkewSprite>(
    std::string(RESOURCE_DIR) + "/img/UI_RECRUIT_BASE_STATES.png",
    src,
    Arknights::UI::FigmaQuadToWorld(figmaQuad)
);

auto obj = std::make_shared<Util::GameObject>(sprite, 20.0f);
m_Root.AddChild(obj);
```

### Full Example

假設你量到：

```text
atlas rect:
x = 8
y = 238
w = 582
h = 64

Figma quad:
topLeft     = (1128, 632)
topRight    = (1392, 632)
bottomRight = (1408, 792)
bottomLeft  = (1112, 792)
```

程式碼：

```cpp
using Arknights::UI::AtlasRect;
using Arknights::UI::FigmaQuadToWorld;
using Arknights::UI::Quad;
using Arknights::UI::SkewSprite;

AtlasRect src {
    8.0f,
    238.0f,
    582.0f,
    64.0f
};

Quad figmaQuad {
    {1128.0f, 632.0f},
    {1392.0f, 632.0f},
    {1408.0f, 792.0f},
    {1112.0f, 792.0f}
};

auto sprite = std::make_shared<SkewSprite>(
    std::string(RESOURCE_DIR) + "/img/UI_RECRUIT_BASE_STATES.png",
    src,
    FigmaQuadToWorld(figmaQuad)
);

auto obj = std::make_shared<Util::GameObject>(sprite, 20.0f);
m_Root.AddChild(obj);
```

如果你要放到 `LobbyScene`，通常是在 `LobbyScene::createMainButtons()` 或另外新增 `createSkewUi()` 裡建立，並在 `init()` 呼叫該函式。

## Current Rendering Approach

`SkewSprite` 建立自己的 `VertexArray`：

```text
positions:
topLeft
bottomLeft
bottomRight
topRight

uv:
source left/top
source left/bottom
source right/bottom
source right/top

indices:
0, 1, 2
0, 2, 3
```

它仍然沿用 PTSD 的 `Base.vert` / `Base.frag` shader 與 `Matrices` uniform。

## Important Limitation

目前 `SkewSprite::GetSize()` 回傳 `{1, 1}`，因為實際大小已經寫進 vertex position。這是刻意設計，讓 `GameObject` 的 transform 不再干涉 quad 的座標。

因此：

- 適合用來精準放置 Figma 量出來的 skew UI。
- 不適合拿來當一般 `Button` 的碰撞大小來源。
- 若要點擊判定，需要另外做 polygon hit test 或用簡化矩形區域。

## Next Steps

1. 從 `UI_RECRUIT_BASE_STATES.png` 實際量 atlas rect。
2. 在 Figma 量該 component 的四個角。
3. 在 `LobbyScene` 或測試場景中放入一個 `SkewSprite` 實測。
4. 若 UV 出現上下顛倒或邊界 bleeding，再調整 UV 或加半像素 inset。

## Build Status

已執行：

```powershell
cmake --build build
```

結果：

- `src/Arknights/UI/SkewSprite.cpp` 已成功編譯成 object。
- 整體 build 目前仍失敗，但失敗點在既有 PTSD/GLEW 子專案：

```text
i386 architecture of input file ... glew.rc.obj is incompatible with i386:x86-64 output
```

這不是 `SkewSprite` 新增程式碼造成的錯誤，而是目前 Windows MinGW/GLEW resource object 的 32/64-bit 架構不一致問題。

## Why Not MCP

這個功能不需要外部 MCP 才能完成。核心問題是渲染管線缺少「atlas rect + custom quad」這個 drawable。直接在 C++ 專案內新增 `SkewSprite` 比透過 MCP 產生流程更穩定，也比較容易之後接進 `LobbyScene`、Button hit test 或 UI layout helper。
