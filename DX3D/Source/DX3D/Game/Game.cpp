#include <DX3D/Game/Game.h>
#include <DX3D/Window/Window.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Core/Logger.h>
#include <DX3D/Game/Display.h>
#include <DX3D/Core/EngineTime.h>
#include <memory>

// Include the new primitive headers
#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/Plane.h>
#include <DX3D/Graphics/Sphere.h>
#include <DX3D/Graphics/Cylinder.h>
#include <DX3D/Graphics/Capsule.h>

dx3d::Game::Game(const GameDesc& desc) :
	Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
	m_loggerPtr(&m_logger)
{
	EngineTime::initialize();

	m_graphicsEngine = std::make_unique<GraphicsEngine>(GraphicsEngineDesc{ {m_logger}, desc.windowSize.width, desc.windowSize.height });
	
	m_display = std::make_unique<Display>(DisplayDesc{ {m_logger,desc.windowSize}, m_graphicsEngine->getGraphicsDevice() });
	GraphicsResourceDesc res_desc = m_graphicsEngine->getGraphicsResourceDesc();

	auto plane = std::make_unique<Plane>(res_desc);
	plane->setPosition({0, -2, 0});
	plane->setScale({20, 1, 20});
	m_graphicsEngine->addGameObject(std::move(plane));

	auto cube = std::make_unique<Cube>(res_desc);
	cube->setPosition({0, -1, 0});
	m_graphicsEngine->addGameObject(std::move(cube));

	auto sphere = std::make_unique<Sphere>(res_desc);
	sphere->setPosition({ -4, 0, 0 });
	sphere->setScale({ 2, 2, 2 }); // Make it bigger
	m_graphicsEngine->addGameObject(std::move(sphere));

	auto cylinder = std::make_unique<Cylinder>(res_desc);
	cylinder->setPosition({ -4, -1, -4 });
	cylinder->setScale({ 1, 1.5, 1 }); // Make it taller
	m_graphicsEngine->addGameObject(std::move(cylinder));

	auto capsule = std::make_unique<Capsule>(res_desc);
	capsule->setPosition({ 3, 0, 3 });
	m_graphicsEngine->addGameObject(std::move(capsule));

	DX3DLogInfo("Game initialized.");
}

dx3d::Game::~Game()
{
	DX3DLogInfo("Game is shutting down...");
	EngineTime::release();
}

void dx3d::Game::onUpdate(float dt)
{
	m_graphicsEngine->onUpdate(dt);
}

void dx3d::Game::onRender()
{
	m_graphicsEngine->render(m_display->getSwapChain());
}