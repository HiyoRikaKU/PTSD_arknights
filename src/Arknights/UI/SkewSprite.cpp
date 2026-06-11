#include "Arknights/UI/SkewSprite.hpp"

#include "Core/IndexBuffer.hpp"
#include "Core/TextureUtils.hpp"
#include "Core/VertexBuffer.hpp"
#include "Util/Logger.hpp"
#include "Util/MissingTexture.hpp"
#include "config.hpp"

#include <SDL_image.h>

namespace {

std::shared_ptr<SDL_Surface> LoadSkewSpriteSurface(const std::string& filepath) {
    auto surface = std::shared_ptr<SDL_Surface>(IMG_Load(filepath.c_str()),
                                                SDL_FreeSurface);

    if (surface == nullptr) {
        surface = {GetMissingTextureSDLSurface(), SDL_FreeSurface};
        LOG_ERROR("Failed to load skew sprite atlas: '{}'", filepath);
        LOG_ERROR("{}", IMG_GetError());
    }

    return surface;
}

} // namespace

namespace Arknights::UI {

SkewSprite::SkewSprite(const std::string& atlasPath,
                       const AtlasRect& sourceRect,
                       const Quad& worldQuad) {
    if (s_Program == nullptr) {
        InitProgram();
    }

    m_UniformBuffer = std::make_unique<Core::UniformBuffer<Core::Matrices>>(
        *s_Program, "Matrices", 0);

    auto surface = s_Store.Get(atlasPath);
    m_AtlasSize = {surface->w, surface->h};
    m_Texture = std::make_unique<Core::Texture>(
        Core::SdlFormatToGlFormat(surface->format->format), surface->w,
        surface->h, surface->pixels);

    InitVertexArray(sourceRect, worldQuad);
}

void SkewSprite::Draw(const Core::Matrices& data) {
    m_UniformBuffer->SetData(0, data);

    m_Texture->Bind(UNIFORM_SURFACE_LOCATION);
    s_Program->Bind();
    s_Program->Validate();

    m_VertexArray->Bind();
    m_VertexArray->DrawTriangles();
}

void SkewSprite::InitProgram() {
    s_Program =
        std::make_unique<Core::Program>(PTSD_ASSETS_DIR "/shaders/Base.vert",
                                        PTSD_ASSETS_DIR "/shaders/Base.frag");
    s_Program->Bind();

    GLint location = glGetUniformLocation(s_Program->GetId(), "surface");
    glUniform1i(location, UNIFORM_SURFACE_LOCATION);
}

void SkewSprite::InitVertexArray(const AtlasRect& sourceRect,
                                 const Quad& worldQuad) {
    const float u0 = sourceRect.x / m_AtlasSize.x;
    const float v0 = sourceRect.y / m_AtlasSize.y;
    const float u1 = (sourceRect.x + sourceRect.w) / m_AtlasSize.x;
    const float v1 = (sourceRect.y + sourceRect.h) / m_AtlasSize.y;

    m_VertexArray = std::make_unique<Core::VertexArray>();

    m_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            worldQuad.topLeft.x, worldQuad.topLeft.y,
            worldQuad.bottomLeft.x, worldQuad.bottomLeft.y,
            worldQuad.bottomRight.x, worldQuad.bottomRight.y,
            worldQuad.topRight.x, worldQuad.topRight.y,
        },
        2));

    m_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            u0, v0,
            u0, v1,
            u1, v1,
            u1, v0,
        },
        2));

    m_VertexArray->SetIndexBuffer(
        std::make_unique<Core::IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2,
            0, 2, 3,
        }));
}

std::unique_ptr<Core::Program> SkewSprite::s_Program = nullptr;
Util::AssetStore<std::shared_ptr<SDL_Surface>> SkewSprite::s_Store(
    LoadSkewSpriteSurface);

} // namespace Arknights::UI
