#pragma once
#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/Texture.h>

namespace dx3d
{
    class TexturedCube final : public Cube
    {
    public:
        TexturedCube(const GraphicsResourceDesc& desc, const std::wstring& texturePath);

        void render(GraphicsEngine* engine) override;

    private:
        std::shared_ptr<Texture> m_texture;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
    };
}

