#include <DX3D/Graphics/TexturedCube.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/Shader.h>
#include <DX3D/Math/Vec2.h>
#include <vector>

namespace dx3d
{
    struct TexturedCubeVertex
    {
        Vec3 pos;
        Vec2 tex;
    };

    TexturedCube::TexturedCube(const GraphicsResourceDesc& desc, const std::wstring& texturePath)
        : Cube(desc)
    {
        m_texture = std::make_shared<Texture>(desc, texturePath);
        //setName("Textured Cube");

        std::vector<TexturedCubeVertex> vertices =
        {
            // Front Face
            { Vec3(-0.5f, -0.5f, -0.5f), Vec2(0.0f, 1.0f) },
            { Vec3(-0.5f,  0.5f, -0.5f), Vec2(0.0f, 0.0f) },
            { Vec3(0.5f,  0.5f, -0.5f), Vec2(1.0f, 0.0f) }, 
            { Vec3(0.5f, -0.5f, -0.5f), Vec2(1.0f, 1.0f) }, 

            // Back Face
            { Vec3(0.5f, -0.5f,  0.5f), Vec2(0.0f, 1.0f) }, 
            { Vec3(0.5f,  0.5f,  0.5f), Vec2(0.0f, 0.0f) }, 
            { Vec3(-0.5f,  0.5f,  0.5f), Vec2(1.0f, 0.0f) },
            { Vec3(-0.5f, -0.5f,  0.5f), Vec2(1.0f, 1.0f) }
        };

        std::vector<UINT> indices = {
            0,1,2, 0,2,3,
            4,5,6, 4,6,7,
            1,6,5, 1,2,5,
            0,3,4, 0,4,7,
            3,2,5, 3,5,4,
            7,6,1, 7,1,0
        };

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = (UINT) sizeof(TexturedCubeVertex) * (UINT) vertices.size();
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = vertices.data();
        DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer), "Failed to create textured cube vertex buffer");

        Shader::ShaderDesc vsDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Vertex, "main", "vs_5_0" };
        m_vertexShader = std::make_unique<Shader>(vsDesc);
        m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/TexturedVertexShader.hlsl");

        Shader::ShaderDesc psDesc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Pixel, "main", "ps_5_0" };
        m_pixelShader = std::make_unique<Shader>(psDesc);
        m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/TexturedPixelShader.hlsl");

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        DX3DGraphicsLogThrowOnFail(m_device.CreateInputLayout(layoutDesc, 2, m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout), "Failed to create textured cube input layout");
    }

    void TexturedCube::onUpdate(float dt)
    {
        GameObject::onUpdate(dt);
    }

    void TexturedCube::render(GraphicsEngine* engine)
    {
        auto context = engine->getDeviceContext()->m_context.Get();

        context->VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);
        context->PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);

        ID3D11SamplerState* sampler = engine->getSamplerState();
        context->PSSetSamplers(0, 1, &sampler);
        ID3D11ShaderResourceView* srv = m_texture->getShaderResourceView();
        context->PSSetShaderResources(0, 1, &srv);

        UINT stride = sizeof(TexturedCubeVertex);
        UINT offset = 0;
        context->IASetInputLayout(m_inputLayout.Get());
        context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        engine->updateConstantBuffer(m_worldMatrix, engine->getViewMatrix(), engine->getProjectionMatrix());
        context->DrawIndexed(36, 0, 0);
    }
}