#pragma once

#include "Core/Drawable.hpp"
#include "Core/Program.hpp"
#include "Core/Texture.hpp"
#include "Core/UniformBuffer.hpp"
#include "Core/VertexArray.hpp"
#include "Util/AssetStore.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace Arknights::UI {

struct AtlasRect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

struct Quad {
    glm::vec2 topLeft;
    glm::vec2 topRight;
    glm::vec2 bottomRight;
    glm::vec2 bottomLeft;
};

inline glm::vec2 FigmaToWorld(const glm::vec2& point,
                              float screenW = 1600.0f,
                              float screenH = 900.0f) {
    return {
        point.x - screenW * 0.5f,
        -(point.y - screenH * 0.5f)
    };
}

inline Quad FigmaQuadToWorld(const Quad& quad,
                             float screenW = 1600.0f,
                             float screenH = 900.0f) {
    return {
        FigmaToWorld(quad.topLeft, screenW, screenH),
        FigmaToWorld(quad.topRight, screenW, screenH),
        FigmaToWorld(quad.bottomRight, screenW, screenH),
        FigmaToWorld(quad.bottomLeft, screenW, screenH)
    };
}

class SkewSprite final : public Core::Drawable {
public:
    SkewSprite(const std::string& atlasPath, const AtlasRect& sourceRect,
               const Quad& worldQuad);

    void Draw(const Core::Matrices& data) override;
    glm::vec2 GetSize() const override { return {1.0f, 1.0f}; }

private:
    void InitProgram();
    void InitVertexArray(const AtlasRect& sourceRect, const Quad& worldQuad);

    static constexpr int UNIFORM_SURFACE_LOCATION = 0;

    static std::unique_ptr<Core::Program> s_Program;
    static Util::AssetStore<std::shared_ptr<SDL_Surface>> s_Store;

    std::unique_ptr<Core::VertexArray> m_VertexArray;
    std::unique_ptr<Core::UniformBuffer<Core::Matrices>> m_UniformBuffer;
    std::unique_ptr<Core::Texture> m_Texture;
    glm::vec2 m_AtlasSize = {1.0f, 1.0f};
};

} // namespace Arknights::UI
