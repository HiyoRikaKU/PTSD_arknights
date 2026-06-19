#ifndef ARKNIGHTS_HEALTH_BAR_HPP
#define ARKNIGHTS_HEALTH_BAR_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <memory>
#include <string>
#include <algorithm>

namespace Arknights {

/**
 * @class HealthBar
 * @brief A class representing a health bar for game entities.
 */
class HealthBar : public Util::GameObject {
public:
    // Target size in world pixels at full health
    static inline glm::vec2 s_DefaultSize = {80.0f, 8.0f}; 
    static inline float s_DefaultOffset = 50.0f; 

    HealthBar(float zIndex, const std::string& imagePath = "/red.png") {
        m_FillImage = std::make_shared<Util::Image>(
            std::string(RESOURCE_DIR) + imagePath
        );
        m_Fill = std::make_shared<Util::GameObject>(
            m_FillImage,
            zIndex
        );

        m_Fill->SetVisible(false);
        m_Fill->SetPivot({0, 0}); 

        AddChild(m_Fill);
        SetZIndex(zIndex);
    }

    void SetValue(float current, float max) {
        m_CurrentHp = current;
        m_MaxHp = max;
    }

    void SetVisible(const bool visible) {
        m_Visible = visible;
        m_Fill->SetVisible(visible);
    }
    
    void Update(const glm::vec2& characterPos, float offset = s_DefaultOffset) {
        m_Transform.translation = characterPos + glm::vec2{0, offset};
        
        if (!m_Visible) return;

        float percent = (m_MaxHp > 0) ? std::max(0.0f, std::min(1.0f, m_CurrentHp / m_MaxHp)) : 0.0f;
        
        // Scale fill to match current health percentage
        glm::vec2 fillImgSize = m_FillImage->GetSize();
        glm::vec2 currentFillSize = { s_DefaultSize.x * percent, s_DefaultSize.y };
        m_Fill->m_Transform.scale = currentFillSize / fillImgSize;
        
        // Keep it centered above the character
        m_Fill->m_Transform.translation = m_Transform.translation;
        
        m_Fill->SetVisible(m_Visible && percent > 0.001f);
    }

private:
    std::shared_ptr<Util::Image> m_FillImage;
    std::shared_ptr<Util::GameObject> m_Fill;
    float m_CurrentHp = 100.0f;
    float m_MaxHp = 100.0f;
    bool m_Visible = false;
};

} // namespace Arknights

#endif // ARKNIGHTS_HEALTH_BAR_HPP
