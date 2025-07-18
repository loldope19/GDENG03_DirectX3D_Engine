#pragma once
#include <DX3D/Game/GameObject.h>
#include <DX3D/Graphics/Texture.h>
#include <DX3D/Graphics/Shader.h>
#include <string>
#include <memory>

namespace dx3d
{
    class Model final : public GameObject
    {
    public:
        Model(const GraphicsResourceDesc& desc, const char* model_path, const wchar_t* texture_path = nullptr);
        virtual void render(GraphicsEngine* engine) override;

    private:
        struct Vertex
        {
            Vec3 position;
            Vec2 texcoord;
        };

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        std::unique_ptr<Shader> m_vertexShader;
        std::unique_ptr<Shader> m_pixelShader;
        std::unique_ptr<Shader> m_texturedPixelShader;

        std::shared_ptr<Texture> m_texture;
        UINT m_indexCount = 0;
    };
}