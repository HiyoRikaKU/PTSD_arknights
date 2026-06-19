#ifndef ARKNIGHTS_UI_FADE_IMAGE_HPP
#define ARKNIGHTS_UI_FADE_IMAGE_HPP

#include <memory>
#include <string>

#include <glm/fwd.hpp>

#include "Core/Drawable.hpp"
#include "Core/Program.hpp"
#include "Core/Texture.hpp"
#include "Core/UniformBuffer.hpp"
#include "Core/VertexArray.hpp"

#include "Util/AssetStore.hpp"
#include "Util/Transform.hpp"

namespace Arknights {
namespace UI {

class FadeImage : public Core::Drawable {
public:
    explicit FadeImage(const std::string &filepath);
    ~FadeImage() override = default;

    glm::vec2 GetSize() const override { return m_Size; };
    void SetImage(const std::string &filepath);
    void Draw(const Core::Matrices &data) override;

    void SetAlpha(float alpha) { m_Alpha = alpha; }
    float GetAlpha() const { return m_Alpha; }

private:
    void InitProgram();
    void InitVertexArray();

    static constexpr int UNIFORM_SURFACE_LOCATION = 0;

    static std::unique_ptr<Core::Program> s_Program;
    static std::unique_ptr<Core::VertexArray> s_VertexArray;
    std::unique_ptr<Core::UniformBuffer<Core::Matrices>> m_UniformBuffer;
    static Util::AssetStore<std::shared_ptr<SDL_Surface>> s_Store;

    std::unique_ptr<Core::Texture> m_Texture = nullptr;
    std::string m_Path;
    glm::vec2 m_Size;
    float m_Alpha = 1.0f;
    GLint m_AlphaLocation = -1;
};

} // namespace UI
} // namespace Arknights

#endif
