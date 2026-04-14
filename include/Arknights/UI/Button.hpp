#ifndef ARKNIGHTS_UI_BUTTON_HPP
#define ARKNIGHTS_UI_BUTTON_HPP

#include <functional>
#include <memory>
#include <string>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include "Util/Input.hpp"

namespace Arknights {
namespace UI {

class Button : public Util::GameObject {
public:
    using ClickCallback = std::function<void()>;

    // Image-based button
    Button(const std::string& imagePath, const glm::vec2& position, float zIndex = 10.0f);
    
    // Text-based button with background
    Button(const std::string& text, const std::string& fontPath, int fontSize, 
           const glm::vec2& position, const glm::vec2& size, float zIndex = 10.0f);

    void update(float deltaTime);
    
    void setOnClick(ClickCallback callback) { m_OnClick = callback; }
    
    void setHoverScale(float scale) { m_HoverScale = scale; }
    void setClickScale(float scale) { m_ClickScale = scale; }
    
    bool isHovered() const { return m_IsHovered; }
    bool isPressed() const { return m_IsPressed; }

private:
    void init();
    bool containsPoint(const glm::vec2& point) const;

private:
    ClickCallback m_OnClick;
    
    glm::vec2 m_Size;
    glm::vec2 m_OriginalScale;
    
    bool m_IsHovered = false;
    bool m_IsPressed = false;
    bool m_WasPressed = false;
    
    float m_HoverScale = 1.1f;
    float m_ClickScale = 0.95f;
    
    std::shared_ptr<Util::Text> m_TextDrawable;
};

} // namespace UI
} // namespace Arknights

#endif
