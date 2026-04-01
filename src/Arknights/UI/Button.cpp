#include "Arknights/UI/Button.hpp"
#include "Util/Keycode.hpp"

namespace Arknights {
namespace UI {

Button::Button(const std::string& imagePath, const glm::vec2& position, float zIndex) {
    auto image = std::make_shared<Util::Image>(imagePath);
    SetDrawable(image);
    m_Transform.translation = position;
    SetZIndex(zIndex);
    
    m_Size = image->GetSize();
    m_OriginalScale = glm::vec2(1.0f);
    m_Transform.scale = m_OriginalScale;
    
    init();
}

Button::Button(const std::string& text, const std::string& fontPath, int fontSize,
               const glm::vec2& position, const glm::vec2& size, float zIndex) {
    m_TextDrawable = std::make_shared<Util::Text>(fontPath, fontSize, text, Util::Color(255, 255, 255));
    SetDrawable(m_TextDrawable);
    m_Transform.translation = position;
    SetZIndex(zIndex);
    
    m_Size = size;
    m_OriginalScale = glm::vec2(1.0f);
    m_Transform.scale = m_OriginalScale;
    
    init();
}

void Button::init() {
    // Button is ready for use
}

void Button::update(float /*deltaTime*/) {
    glm::vec2 mousePos = Util::Input::GetCursorPosition();
    bool wasHovered = m_IsHovered;
    m_IsHovered = containsPoint(mousePos);
    
    bool mouseDown = Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB);
    
    // Handle hover effect
    if (m_IsHovered && !wasHovered) {
        m_Transform.scale = m_OriginalScale * m_HoverScale;
    } else if (!m_IsHovered && wasHovered) {
        m_Transform.scale = m_OriginalScale;
    }
    
    // Handle click
    if (m_IsHovered && mouseDown) {
        m_IsPressed = true;
        m_Transform.scale = m_OriginalScale * m_ClickScale;
    } else {
        if (m_IsPressed && m_WasPressed && !mouseDown) {
            // Click released
            if (m_OnClick) {
                m_OnClick();
            }
            m_Transform.scale = m_IsHovered ? m_OriginalScale * m_HoverScale : m_OriginalScale;
        }
        m_IsPressed = false;
    }
    
    m_WasPressed = mouseDown;
}

bool Button::containsPoint(const glm::vec2& point) const {
    glm::vec2 pos = m_Transform.translation;
    glm::vec2 halfSize = m_Size * m_Transform.scale * 0.5f;
    
    return point.x >= pos.x - halfSize.x && point.x <= pos.x + halfSize.x &&
           point.y >= pos.y - halfSize.y && point.y <= pos.y + halfSize.y;
}

} // namespace UI
} // namespace Arknights
