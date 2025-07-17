#pragma once
#include <DX3D/Graphics/GraphicsResource.h>
#include <string>

#include <DirectXTex.h>

namespace dx3d
{
    class Texture final : public GraphicsResource
    {
    public:
        Texture(const GraphicsResourceDesc& desc, const std::wstring& filePath);

        ID3D11ShaderResourceView* getShaderResourceView() const { return m_shaderResourceView.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
    };
}