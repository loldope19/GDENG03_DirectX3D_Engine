#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/AnimatedRectangle.h>
#include <DX3D/Core/EngineTime.h>
#include <DX3D/Math/Vec3.h>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace dx3d;

bool GraphicsEngine::s_rotationEnabled = false;

GraphicsEngine::GraphicsEngine(const GraphicsEngineDesc& desc) : Base(desc.base)
{
    m_graphicsDevice = std::make_shared<GraphicsDevice>(GraphicsDeviceDesc{ m_logger });

    auto& device = *m_graphicsDevice;
    m_deviceContext = device.createDeviceContext();

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    m_graphicsDevice->m_d3dDevice->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStateCullNone);


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
    m_animatedRectangleManager = std::make_unique<AnimatedRectangle>(gDesc);

    m_triangleManager->initializeSharedResources();
    m_rectangleManager->initializeSharedResources();
    m_animatedRectangleManager->initializeSharedResources();

    //addRectangle(0.0f, 0.0f, 0.5f, 0.5f);
    
    // Slide 13 - Engine Time
    /*
    std::array<VertexState, 4> stateA_verts = { {
            {-0.1f,  0.7f, 0.0f, {1.0f, 1.0f, 0.0f, 1.0f}}, // Yellow
            { 0.8f,  0.7f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}, // Blue
            { 0.7f, -0.7f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}}, // Red
            {-0.4f, -0.1f, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}}  // Green
        } 
    };

    std::array<VertexState, 4> stateB_verts = { {
            {-0.8f,  0.2f, 0.0f, {1.0f, 1.0f, 0.0f, 1.0f}}, // Yellow
            { 0.2f,  0.2f, 0.0f, {1.0f, 1.0f, 1.0f, 1.0f}}, // White
            { 0.3f, -0.4f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}, // Blue
            {-0.7f, -0.9f, 0.0f, {0.1f, 0.0f, 0.1f, 1.0f}}  // Black
        } 
    };
    */

    // Slide 14 - Replication
    /*
    std::array<VertexState, 4> stateA_verts = { {
            {-0.1f,  0.8f, 0.0f, {1.0f, 1.0f, 0.0f, 1.0f}}, // Yellow
            { 0.9f,  0.8f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}, // Blue
            { 0.1f, -0.7f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}}, // Red
            {-0.3f, -0.1f, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}}  // Green
        }
    };

    std::array<VertexState, 4> stateB_verts = { {
            {-0.9f,  0.2f, 0.0f, {1.0f, 1.0f, 0.0f, 1.0f}}, // Yellow
            {-0.7f, -0.9f, 0.0f, {1.0f, 1.0f, 1.0f, 1.0f}}, // White (same position as black)
            { 1.0f, -0.4f, 0.0f, {0.0f, 0.0f, 2.0f, 1.0f}}, // Blue
            {-0.7f, -0.9f, 0.0f, {0.1f, 0.0f, 0.1f, 1.0f}}  // Black
        }
    };
    */

    // addAnimatedRectangle(stateA_verts, stateB_verts);

    // Slide 30 - Depth Checking
    // In the absence of an Engine UI, I'm rendering both primitives by code first
    // Cube is rendered first
    createCube({ 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f }, {0.0f, 0.0f, 1.0f}, 2.0f);
    // Plane is rendered next
    createPlane({ 0.0f, 0.0f, 0.0f }, { 10.0f, 1.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

    m_camera = std::make_unique<Camera>(Vec3{ 0.0f, 0.0f, -10.0f }, 10.0f, 15.0f);

    float width = 1280.0f;
    float height = 720.0f;
    m_projectionMatrix = Matrix4x4::perspective(90.0f, width / height, 0.1f, 1000.0f);
}

GraphicsEngine::~GraphicsEngine()
{
}

GraphicsDevice& GraphicsEngine::getGraphicsDevice() noexcept
{
    return *m_graphicsDevice;
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



void dx3d::GraphicsEngine::createCube(const Vec3& position, const Vec3& scale, const Vec3& rotationAxis, float rotationSpeed)
{
    GraphicsResourceDesc gDesc = {
        {m_logger},
        m_graphicsDevice,
        *m_graphicsDevice->m_d3dDevice.Get(),
        *m_graphicsDevice->m_dxgiFactory.Get() };

    auto cube = std::make_unique<Cube>(gDesc, rotationAxis, rotationSpeed);
    cube->setPosition(position);
    cube->setScale(scale);
    m_cubes.push_back(std::move(cube));
}

void dx3d::GraphicsEngine::addAnimatedRectangle(const std::array<VertexState, 4>& state_A_vertices, const std::array<VertexState, 4>& state_B_vertices)
{
    std::vector<AnimatedRectangleVertex> animatedVertices(4);

    for (int i = 0; i < 4; ++i) {
        animatedVertices[i].posX_A = state_A_vertices[i].x;
        animatedVertices[i].posY_A = state_A_vertices[i].y;
        animatedVertices[i].posZ_A = state_A_vertices[i].z;
        animatedVertices[i].r_A = state_A_vertices[i].color.x;
        animatedVertices[i].g_A = state_A_vertices[i].color.y;
        animatedVertices[i].b_A = state_A_vertices[i].color.z;
        animatedVertices[i].a_A = state_A_vertices[i].color.w;

        animatedVertices[i].posX_B = state_B_vertices[i].x;
        animatedVertices[i].posY_B = state_B_vertices[i].y;
        animatedVertices[i].posZ_B = state_B_vertices[i].z;
        animatedVertices[i].r_B = state_B_vertices[i].color.x;
        animatedVertices[i].g_B = state_B_vertices[i].color.y;
        animatedVertices[i].b_B = state_B_vertices[i].color.z;
        animatedVertices[i].a_B = state_B_vertices[i].color.w;
    }

    if (m_animatedRectangleManager) {
        m_animatedRectangleManager->createAnimatedRectangle(animatedVertices);
    }
}

void dx3d::GraphicsEngine::createPlane(const Vec3& position, const Vec3& scale, const Vec4& color)
{
    GraphicsResourceDesc gDesc = { {m_logger}, m_graphicsDevice, *m_graphicsDevice->m_d3dDevice.Get(), *m_graphicsDevice->m_dxgiFactory.Get() };
    auto plane = std::make_unique<Plane>(gDesc, position, scale, color);
    m_planes.push_back(std::move(plane));
}

void GraphicsEngine::onUpdate(float dt, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float deltaX, float deltaY)
{
    m_camera->update(dt, moveForward, moveBackward, moveLeft, moveRight, deltaX, deltaY);

    float engine_dt = static_cast<float>(EngineTime::getDeltaTime());
    for (auto& cube : m_cubes)
    {
        cube->update(engine_dt);
    }

    if (m_animatedRectangleManager) {
        static float accumulatedTime = 0.0f;
        accumulatedTime += engine_dt;
        float interpFactor = (sinf(accumulatedTime * 1.0f) + 1.0f) / 2.0f;
        m_animatedRectangleManager->updateAnimation(*m_deviceContext->m_context.Get(), interpFactor);
    }
}

void GraphicsEngine::toggleRotation()
{
    s_rotationEnabled = !s_rotationEnabled;
}

bool GraphicsEngine::isRotationEnabled()
{
    return s_rotationEnabled;
}

void GraphicsEngine::render(SwapChain& swapChain)
{
    auto& context = *m_deviceContext;
    context.clearAndSetBackBuffer(swapChain, { 0.f, 0.27f, 0.4f, 1.0f });
    context.setGraphicsPipelineState(*m_pipeline);

    D3D11_VIEWPORT viewport = {};
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain.m_swapChain->GetDesc(&desc);
    viewport.Width = static_cast<float>(desc.BufferDesc.Width);
    viewport.Height = static_cast<float>(desc.BufferDesc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context.m_context->RSSetViewports(1, &viewport);

    m_viewMatrix = m_camera->getViewMatrix();

    m_triangleManager->render(*context.m_context.Get());
    m_rectangleManager->render(*context.m_context.Get());

    for (auto& cube : m_cubes)
    {
        cube->render(*m_deviceContext->m_context.Get(), m_viewMatrix, m_projectionMatrix);
    }

    // Removed back-face culling temporarily for plane
    context.m_context->RSSetState(m_rasterizerStateCullNone.Get());
    for (auto& plane : m_planes)
    {
        plane->render(*m_deviceContext->m_context.Get(), m_viewMatrix, m_projectionMatrix);
    }
    context.m_context->RSSetState(nullptr);

    if (m_animatedRectangleManager) {
        m_animatedRectangleManager->render(*context.m_context.Get());
    }

    m_graphicsDevice->executeCommandList(context);
    swapChain.present();
}