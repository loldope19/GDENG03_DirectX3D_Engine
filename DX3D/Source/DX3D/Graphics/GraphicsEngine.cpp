#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/GraphicsLogUtils.h>
#include <DX3D/Graphics/Circle.h>

using namespace dx3d;

GraphicsEngine::GraphicsEngine(const GraphicsEngineDesc& desc) : Base(desc.base)
{
    m_graphicsDevice = std::make_shared<GraphicsDevice>(GraphicsDeviceDesc{ m_logger });

    auto& device = *m_graphicsDevice;
    m_deviceContext = device.createDeviceContext();

    constexpr char shaderSourceCode[] =
        R"(
void VSMain()
{
}
void PSMain()
{
}
)";
    constexpr char shaderSourceName[] = "Basic";
    constexpr auto shaderSourceCodeSize = std::size(shaderSourceCode);

    auto vs = device.compileShader({ shaderSourceName, shaderSourceCode, shaderSourceCodeSize,
        "VSMain", ShaderType::VertexShader });
    auto ps = device.compileShader({ shaderSourceName, shaderSourceCode, shaderSourceCodeSize,
        "PSMain", ShaderType::PixelShader });

    m_pipeline = device.createGraphicsPipelineState({ *vs,*ps });

    GraphicsResourceDesc gDesc = { {m_logger}, m_graphicsDevice,
                                *m_graphicsDevice->m_d3dDevice.Get(),
                                *m_graphicsDevice->m_dxgiFactory.Get() };
    m_triangleManager = std::make_unique<Triangle>(gDesc);
    m_rectangleManager = std::make_unique<Rectangle>(gDesc);
    m_cubeManager = std::make_unique<Cube>(gDesc);
    m_circleManager = std::make_unique<Circle>(gDesc);

    //m_triangleManager->initializeSharedResources();
    //m_rectangleManager->initializeSharedResources();
    //m_cubeManager->initializeSharedResources();
    m_circleManager->initializeSharedResources();


    // light constant buffer
    D3D11_BUFFER_DESC lightBufferDesc = {};
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightConstantBufferData);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    DX3DGraphicsLogThrowOnFail(
        m_graphicsDevice->m_d3dDevice->CreateBuffer(&lightBufferDesc, nullptr, &m_lightConstantBuffer),
        "Failed to create light constant buffer"
    );
}

GraphicsEngine::~GraphicsEngine()
{
}

GraphicsDevice& GraphicsEngine::getGraphicsDevice() noexcept
{
    return *m_graphicsDevice;
}

void dx3d::GraphicsEngine::updateLightData(
    const DirectX::XMFLOAT2& mousePos, const DirectX::XMFLOAT2& screenRes)
{
    m_lightBufferData.lightScreenPos = mousePos;
    m_lightBufferData.screenResolution = screenRes;
    m_screenResolution = screenRes;
}

void GraphicsEngine::addTriangle(float posX, float posY, float size, float r, float g, float b, float a)
{
    std::vector<TriangleVertex> vertices;
    if (r < 0 || g < 0 || b < 0) {
        vertices = {
            { posX,         posY + size / 2, 0.0f, 0.0f, 0.0f, 0.0f, a },  // Top: Red
            { posX + size / 2, posY - size / 2, 0.0f, 0.0f, 1.0f, 0.0f, a },  // Bottom right: Green
            { posX - size / 2, posY - size / 2, 0.0f, 0.0f, 0.0f, 1.0f, a }   // Bottom left: Blue
        };
    }
    else {
        vertices = {
            { posX,         posY + size / 2, 0.0f, r, g, b, a },  // Top vertex
            { posX + size / 2, posY - size / 2, 0.0f, r, g, b, a },  // Bottom right vertex
            { posX - size / 2, posY - size / 2, 0.0f, r, g, b, a }   // Bottom left vertex
        };
    }

    m_triangleManager->createTriangle(vertices);
}

void dx3d::GraphicsEngine::addRectangle(float posX, float posY, float width, float height, float r, float g, float b, float a)
{
    std::vector<RectangleVertex> vertices;
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    if (r < 0 || g < 0 || b < 0) {
        vertices = {
            { posX - halfWidth, posY + halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, a },  // Top-left: Green
            { posX + halfWidth, posY + halfHeight, 0.0f, 1.0f, 1.0f, 0.0f, a },  // Top-right: Yellow
            { posX + halfWidth, posY - halfHeight, 0.0f, 0.0f, 0.0f, 1.0f, a },  // Bottom-right: Blue
            { posX - halfWidth, posY - halfHeight, 0.0f, 1.0f, 0.0f, 0.0f, a }   // Bottom-left: Red
        };
    }
    else {
        vertices = {
            { posX - halfWidth, posY + halfHeight, 0.0f, r, g, b, a },  // Top-left
            { posX + halfWidth, posY + halfHeight, 0.0f, r, g, b, a },  // Top-right
            { posX + halfWidth, posY - halfHeight, 0.0f, r, g, b, a },  // Bottom-right
            { posX - halfWidth, posY - halfHeight, 0.0f, r, g, b, a }   // Bottom-left
        };
    }

    m_rectangleManager->createRectangle(vertices);
}

void dx3d::GraphicsEngine::addCube(float posX, float posY, float posZ, float size, float r, float g, float b, float a)
{
    std::vector<CubeVertex> vertices;
    float halfSize = size / 2.0f;

    if (r < 0 || g < 0 || b < 0) {
        vertices = {
            { posX - halfSize, posY + halfSize, posZ + halfSize, 1.0f, 0.0f, 0.0f, a },  // Red
            { posX + halfSize, posY + halfSize, posZ + halfSize, 0.0f, 1.0f, 0.0f, a },  // Green
            { posX + halfSize, posY - halfSize, posZ + halfSize, 0.0f, 0.0f, 1.0f, a },  // Blue
            { posX - halfSize, posY - halfSize, posZ + halfSize, 1.0f, 1.0f, 0.0f, a },  // Yellow

            { posX - halfSize, posY + halfSize, posZ - halfSize, 1.0f, 0.0f, 1.0f, a },  // Magenta
            { posX + halfSize, posY + halfSize, posZ - halfSize, 0.0f, 1.0f, 1.0f, a },  // Cyan
            { posX + halfSize, posY - halfSize, posZ - halfSize, 1.0f, 1.0f, 1.0f, a },  // White
            { posX - halfSize, posY - halfSize, posZ - halfSize, 0.5f, 0.5f, 0.5f, a }   // Gray
        };
    }
    else {
        vertices = {
            { posX - halfSize, posY + halfSize, posZ + halfSize, r, g, b, a },
            { posX + halfSize, posY + halfSize, posZ + halfSize, r, g, b, a },
            { posX + halfSize, posY - halfSize, posZ + halfSize, r, g, b, a },
            { posX - halfSize, posY - halfSize, posZ + halfSize, r, g, b, a },

            { posX - halfSize, posY + halfSize, posZ - halfSize, r, g, b, a },
            { posX + halfSize, posY + halfSize, posZ - halfSize, r, g, b, a },
            { posX + halfSize, posY - halfSize, posZ - halfSize, r, g, b, a },
            { posX - halfSize, posY - halfSize, posZ - halfSize, r, g, b, a }
        };
    }

    m_cubeManager->createCube(vertices);
}

void dx3d::GraphicsEngine::addCircle(float posX, float posY, float radius, int segments, float r, float g, float b, float a)
{
    if (segments < 3) return; // circle needs at least 3 segments

    if (m_screenResolution.y == 0) return;

    const float aspectRatio = m_screenResolution.x / m_screenResolution.y;

    std::vector<CircleVertex> vertices;
    std::vector<UINT> indices;

    float fixedR = r, fixedG = g, fixedB = b;
    if (r < 0 || g < 0 || b < 0) { 
        fixedR = fixedG = fixedB = 1.0f;
    }

    vertices.push_back({ posX, posY, 0.0f, fixedR, fixedG, fixedB, a });

    const float angleStep = 2.0f * DirectX::XM_PI / segments;
    for (int i = 0; i < segments; ++i)
    {
        float currentAngle = i * angleStep;
        float x = posX + (radius / aspectRatio) * cos(currentAngle);
        float y = posY + radius * sin(currentAngle);
        vertices.push_back({ x, y, 0.0f, fixedR, fixedG, fixedB, a });
    }

    for (int i = 1; i <= segments; ++i)
    {
        indices.push_back(0); 
        indices.push_back((i % segments) + 1);
        indices.push_back(i);
    }

    m_circleManager->createCircle(vertices, indices);
}

void dx3d::GraphicsEngine::clearCircles()
{
    if (m_circleManager)
    {
        m_circleManager->clear();
    }
}

void GraphicsEngine::render(SwapChain& swapChain)
{
    auto& context = *m_deviceContext;
    context.clearAndSetBackBuffer(swapChain, { 0.0f, 0.0f, 0.0f, 1.0f });

    // shaders used by my shape managers will override this so will comment this out for now
    // context.setGraphicsPipelineState(*m_pipeline);

    // update and set the light constant buffer for my pixel shader
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context.m_context->Map(m_lightConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr)) {
        memcpy(mappedResource.pData, &m_lightBufferData, sizeof(LightConstantBufferData));
        context.m_context->Unmap(m_lightConstantBuffer.Get(), 0);
    }

    // set constant buffer to a specific slot for the pixel shader (e.g., slot 1, (b1))
    context.m_context->PSSetConstantBuffers(1, 1, m_lightConstantBuffer.GetAddressOf());

    // this thing matches the viewport to the window size
    D3D11_VIEWPORT viewport = {};
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain.m_swapChain->GetDesc(&desc);
    viewport.Width = static_cast<float>(desc.BufferDesc.Width);
    viewport.Height = static_cast<float>(desc.BufferDesc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context.m_context->RSSetViewports(1, &viewport);

    //m_triangleManager->render(*context.m_context.Get());
    //m_rectangleManager->render(*context.m_context.Get());
    //m_cubeManager->render(*context.m_context.Get());
    m_circleManager->render(*context.m_context.Get());

    auto& device = *m_graphicsDevice;
    device.executeCommandList(context);
    swapChain.present();
}