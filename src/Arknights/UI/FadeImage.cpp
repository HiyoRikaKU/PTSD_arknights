#include "Arknights/UI/FadeImage.hpp"
#include "Util/Logger.hpp"
#include "Core/TextureUtils.hpp"
#include "Util/MissingTexture.hpp"
#include "config.hpp"

#include <SDL_image.h>

namespace Arknights {
namespace UI {

static std::shared_ptr<SDL_Surface> LoadFadeSurface(const std::string &filepath) {
    auto surface = std::shared_ptr<SDL_Surface>(IMG_Load(filepath.c_str()), SDL_FreeSurface);
    if (surface == nullptr) {
        surface = std::shared_ptr<SDL_Surface>(GetMissingTextureSDLSurface(), SDL_FreeSurface);
        LOG_ERROR("Failed to load image: '{}'", filepath);
        LOG_ERROR("{}", IMG_GetError());
    }
    return surface;
}

std::unique_ptr<Core::Program> FadeImage::s_Program = nullptr;
std::unique_ptr<Core::VertexArray> FadeImage::s_VertexArray = nullptr;
Util::AssetStore<std::shared_ptr<SDL_Surface>> FadeImage::s_Store(LoadFadeSurface);

FadeImage::FadeImage(const std::string &filepath) : m_Path(filepath) {
    if (s_Program == nullptr) {
        InitProgram();
    }
    if (s_VertexArray == nullptr) {
        InitVertexArray();
    }

    m_UniformBuffer = std::make_unique<Core::UniformBuffer<Core::Matrices>>(*s_Program, "Matrices", 0);
    
    // Retrieve alpha location
    m_AlphaLocation = glGetUniformLocation(s_Program->GetId(), "alpha");

    auto surface = s_Store.Get(filepath);

    m_Texture = std::make_unique<Core::Texture>(
        Core::SdlFormatToGlFormat(surface->format->format), surface->w,
        surface->h, surface->pixels);
    m_Size = {surface->w, surface->h};
}

void FadeImage::SetImage(const std::string &filepath) {
    auto surface = s_Store.Get(filepath);
    m_Texture->UpdateData(Core::SdlFormatToGlFormat(surface->format->format),
                          surface->w, surface->h, surface->pixels);
    m_Size = {surface->w, surface->h};
}

void FadeImage::Draw(const Core::Matrices &data) {
    m_UniformBuffer->SetData(0, data);

    m_Texture->Bind(UNIFORM_SURFACE_LOCATION);
    s_Program->Bind();
    
    // Set custom alpha
    if (m_AlphaLocation != -1) {
        glUniform1f(m_AlphaLocation, m_Alpha);
    }
    
    s_Program->Validate();

    s_VertexArray->Bind();
    s_VertexArray->DrawTriangles();
}

void FadeImage::InitProgram() {
    s_Program = std::make_unique<Core::Program>(
        std::string(RESOURCE_DIR) + "/shaders/Fade.vert",
        std::string(RESOURCE_DIR) + "/shaders/Fade.frag");
    s_Program->Bind();

    GLint location = glGetUniformLocation(s_Program->GetId(), "surface");
    glUniform1i(location, UNIFORM_SURFACE_LOCATION);
}

void FadeImage::InitVertexArray() {
    s_VertexArray = std::make_unique<Core::VertexArray>();

    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            -0.5F, 0.5F,
            -0.5F, -0.5F,
            0.5F, -0.5F,
            0.5F, 0.5F,
        }, 2));

    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{
            0.0F, 0.0F,
            0.0F, 1.0F,
            1.0F, 1.0F,
            1.0F, 0.0F,
        }, 2));

    s_VertexArray->SetIndexBuffer(
        std::make_unique<Core::IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2,
            0, 2, 3,
        }));
}

} // namespace UI
} // namespace Arknights
