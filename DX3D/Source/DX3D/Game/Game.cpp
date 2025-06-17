#include <DX3D/Game/Game.h>
#include <DX3D/Window/Window.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Core/Logger.h>
#include <DX3D/Game/Display.h>
#include <DX3D/Core/EngineTime.h>

dx3d::Game::Game(const GameDesc& desc) :
    Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
    m_loggerPtr(&m_logger)
{
    EngineTime::initialize();

    m_graphicsEngine = std::make_unique<GraphicsEngine>(GraphicsEngineDesc{ m_logger });
    m_display = std::make_unique<Display>(DisplayDesc{ {m_logger,desc.windowSize},m_graphicsEngine->getGraphicsDevice() });

    DX3DLogInfo("Game initialized.");
}

dx3d::Game::~Game()
{
    DX3DLogInfo("Game is shutting down...");
    EngineTime::release();
}

void dx3d::Game::onUpdate(float dt, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float deltaX, float deltaY)
{
    m_graphicsEngine->onUpdate(dt, moveForward, moveBackward, moveLeft, moveRight, deltaX, deltaY);
}

void dx3d::Game::onRender()
{
    m_graphicsEngine->render(m_display->getSwapChain());
}
