#include <DX3D/Game/Game.h>
#include <DX3D/Window/Window.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Core/Logger.h>
#include <DX3D/Game/Display.h>
#include <DX3D/Graphics/SwapChain.h>

#include <Windows.h>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
    m_loggerPtr(&m_logger), m_randomEngine(std::random_device{}())
{
    m_graphicsEngine = std::make_unique<GraphicsEngine>(GraphicsEngineDesc{ m_logger });
    m_display = std::make_unique<Display>(DisplayDesc{ {m_logger,desc.windowSize},m_graphicsEngine->getGraphicsDevice() });

    // Initialize camera stuff
    m_camera = std::make_unique<Camera>();
    m_inputManager = std::make_unique<InputManager>();
    m_camera->setPerspective(DirectX::XM_PIDIV4, static_cast<float>(desc.windowSize.width) / desc.windowSize.height, 0.1f, 1000.0f);
    m_camera->setPosition({ 0.0f, 0.0f, -5.0f });
    m_graphicsEngine->setCamera(m_camera.get());

    m_lastFrameTime = std::chrono::steady_clock::now();

    spawnCircle();

    DX3DLogInfo("Game initialized.");
}

dx3d::Game::~Game()
{
    DX3DLogInfo("Game is shutting down...");
}

void dx3d::Game::onInternalUpdate()
{
    // --- Delta Time Calculation ---
    auto currentTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
    m_lastFrameTime = currentTime;

    // --- Input and Logic ---
    m_inputManager->update();
    handleInput();

    // --- Physics Update ---
    DXGI_SWAP_CHAIN_DESC d;
    m_display->getSwapChain().m_swapChain->GetDesc(&d);
    float screenWidth = static_cast<float>(d.BufferDesc.Width);
    float screenHeight = static_cast<float>(d.BufferDesc.Height);

    for (auto& circle : m_circles)
    {
        // Update position
        circle.position.x += circle.velocity.x * deltaTime;
        circle.position.y += circle.velocity.y * deltaTime;

        // Boundary collision check
        if (circle.position.x - circle.radius < 0) {
            circle.velocity.x = abs(circle.velocity.x);
        }
        if (circle.position.x + circle.radius > screenWidth) {
            circle.velocity.x = -abs(circle.velocity.x);
        }
        if (circle.position.y - circle.radius < 0) {
            circle.velocity.y = abs(circle.velocity.y);
        }
        if (circle.position.y + circle.radius > screenHeight) {
            circle.velocity.y = -abs(circle.velocity.y);
        }
    }

    // --- Rendering ---
    POINT screenMousePos = m_inputManager->getScreenMousePosition();
    HWND hwnd = static_cast<HWND>(m_display->getWindowHandle());
    ScreenToClient(hwnd, &screenMousePos);
    m_graphicsEngine->updateLightData(
        { (float)screenMousePos.x, (float)screenMousePos.y },
        { screenWidth, screenHeight }
    );

    m_graphicsEngine->clearCircles();

    for (const auto& circle : m_circles)
    {
        float ndc_x = (circle.position.x / screenWidth) * 2.0f - 1.0f;
        float ndc_y = (circle.position.y / screenHeight) * -2.0f + 1.0f;
        float ndc_radius = (circle.radius / screenHeight) * 2.0f;

        m_graphicsEngine->addCircle(ndc_x, ndc_y, ndc_radius, 32, circle.color.x, circle.color.y, circle.color.z, circle.color.w);
    }

    m_graphicsEngine->render(m_display->getSwapChain());
}

void dx3d::Game::handleInput()
{
    if (m_inputManager->isKeyDown(VK_ESCAPE))
    {
        DX3DLogInfo("Pressed [ESC] - Closing App Window");
        m_isRunning = false;
        return;
    }

    // Spawn new circle (debounced)
    bool spaceIsDown = m_inputManager->isKeyDown(VK_SPACE);
    if (spaceIsDown && !m_spaceWasDown)
    {
        DX3DLogInfo("Pressed [SPACEBAR] - Spawning New Circle");
        spawnCircle();
    }
    m_spaceWasDown = spaceIsDown;

    // Remove last circle (debounced)
    bool backspaceIsDown = m_inputManager->isKeyDown(VK_BACK);
    if (backspaceIsDown && !m_backspaceWasDown)
    {
        DX3DLogInfo("Pressed [BACKSPACE] - Removing Last Circle");
        if (!m_circles.empty())
        {
            m_circles.pop_back();
        }
    }
    m_backspaceWasDown = backspaceIsDown;

    // Remove all circles
    if (m_inputManager->isKeyDown(VK_DELETE))
    {
        DX3DLogInfo("Pressed [DELETE] - Removing All Circles");
        m_circles.clear();
    }
}

void dx3d::Game::spawnCircle()
{
    DXGI_SWAP_CHAIN_DESC d;
    m_display->getSwapChain().m_swapChain->GetDesc(&d);
    float screenWidth = static_cast<float>(d.BufferDesc.Width);
    float screenHeight = static_cast<float>(d.BufferDesc.Height);

    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * DirectX::XM_PI);
    std::uniform_real_distribution<float> speedDist(100.0f, 200.0f);
    std::uniform_real_distribution<float> colorDist(0.5f, 1.0f);
    std::uniform_real_distribution<float> radiusDist(20.0f, 50.0f);

    BouncingCircle newCircle;
    newCircle.position = { screenWidth / 2.0f, screenHeight / 2.0f };
    newCircle.radius = radiusDist(m_randomEngine);

    float angle = angleDist(m_randomEngine);
    float speed = speedDist(m_randomEngine);
    newCircle.velocity = { cos(angle) * speed, sin(angle) * speed };

    newCircle.color = { colorDist(m_randomEngine), colorDist(m_randomEngine), colorDist(m_randomEngine), 1.0f };

    m_circles.push_back(newCircle);
}
