#include <DX3D/Graphics/Model.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/Shader.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>

#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include <../Vendor/tinyobjloader/tiny_obj_loader.h>

namespace dx3d
{
    Model::Model(const GraphicsResourceDesc& gDesc, const char* model_path, const wchar_t* texture_path)
        : GameObject(gDesc)
    {
        // --- Model Data Loading (remains the same) ---
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, model_path, nullptr, true))
        {
            DX3DLogThrowError("Failed to load model: " + std::string(model_path) + "\n" + warn + err);
        }

        std::vector<Vertex> vertices;
        std::vector<UINT> indices;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex = {};
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                if (index.texcoord_index >= 0 && !attrib.texcoords.empty()) {
                    vertex.texcoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }
                vertices.push_back(vertex);
                indices.push_back(static_cast<UINT>(indices.size()));
            }
        }
        m_indexCount = static_cast<UINT>(indices.size());

        // --- Vertex and Index Buffers (remains the same) ---
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = vertices.data();
        DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &data, &m_vertexBuffer), "Failed to create model vertex buffer.");

        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.ByteWidth = sizeof(UINT) * m_indexCount;
        data.pSysMem = indices.data();
        DX3DGraphicsLogThrowOnFail(m_device.CreateBuffer(&bufferDesc, &data, &m_indexBuffer), "Failed to create model index buffer.");

        Shader::ShaderDesc vs_desc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Vertex, "main", "vs_5_0" };
        m_vertexShader = std::make_unique<Shader>(vs_desc);
        if (!m_vertexShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/TexturedVertexShader.hlsl"))
            DX3DLogThrowError("Failed to load textured vertex shader");

        Shader::ShaderDesc ps_desc = { { m_logger, m_graphicsDevice, m_device, m_factory }, Shader::Type::Pixel, "main", "ps_5_0" };
        m_pixelShader = std::make_unique<Shader>(ps_desc);
        if (!m_pixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/PixelShader.hlsl"))
            DX3DLogThrowError("Failed to load pixel shader");

        m_texturedPixelShader = std::make_unique<Shader>(ps_desc);
        if (!m_texturedPixelShader->loadFromFile("DX3D/Source/DX3D/Graphics/Shaders/TexturedPixelShader.hlsl"))
            DX3DLogThrowError("Failed to load textured pixel shader");

        D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        DX3DGraphicsLogThrowOnFail(m_device.CreateInputLayout(layout, ARRAYSIZE(layout), m_vertexShader->getByteCode().data(), m_vertexShader->getByteCode().size(), &m_inputLayout), "Failed to create model input layout.");

        if (texture_path)
        {
            m_texture = std::make_shared<Texture>(gDesc, texture_path);
        }
    }

    void Model::render(GraphicsEngine* engine)
    {
        auto context = engine->getDeviceContext()->m_context.Get();

        engine->updateConstantBuffer(m_worldMatrix, engine->getViewMatrix(), engine->getProjectionMatrix());

        context->IASetInputLayout(m_inputLayout.Get());
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        context->VSSetShader(m_vertexShader->getVertexShader(), nullptr, 0);

        if (m_texture)
        {
            context->PSSetShader(m_texturedPixelShader->getPixelShader(), nullptr, 0);

            ID3D11SamplerState* sampler = engine->getSamplerState();
            context->PSSetSamplers(0, 1, &sampler);

            ID3D11ShaderResourceView* srv = m_texture->getShaderResourceView();
            context->PSSetShaderResources(0, 1, &srv);
        }
        else
        {
            engine->updateMaterialConstantBuffer(Vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
            context->PSSetShader(m_pixelShader->getPixelShader(), nullptr, 0);
        }

        context->DrawIndexed(m_indexCount, 0, 0);
    }
}