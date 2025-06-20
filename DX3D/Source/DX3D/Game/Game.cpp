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

	const Vec3 cardScale = { 2.5f, 3.5f, 1.0f };
	const float leanAngle = 1.1f;
	const Vec4 frontColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Vec4 backColor = { 0.1f, 0.2f, 0.7f, 1.0f };

	auto card1_L = std::make_unique<Plane>(res_desc);
	card1_L->setColor(backColor);
	card1_L->setScale(cardScale);
	card1_L->setRotation({ 0, 0, leanAngle });
	card1_L->setPosition({ -2.6f, 1.6f, 0 });
	m_graphicsEngine->addGameObject(std::move(card1_L));

	auto card1_R = std::make_unique<Plane>(res_desc);
	card1_R->setColor(frontColor);
	card1_R->setScale(cardScale);
	card1_R->setRotation({ 0, 0, -leanAngle });
	card1_R->setPosition({ -1.48f, 1.6f, 0 });
	m_graphicsEngine->addGameObject(std::move(card1_R));

	auto card2_L = std::make_unique<Plane>(res_desc);
	card2_L->setColor(backColor);
	card2_L->setScale(cardScale);
	card2_L->setRotation({ 0, 0, leanAngle });
	card2_L->setPosition({ -0.36f, 1.6f, 0 });
	m_graphicsEngine->addGameObject(std::move(card2_L));

	auto card2_R = std::make_unique<Plane>(res_desc);
	card2_R->setColor(frontColor);
	card2_R->setScale(cardScale);
	card2_R->setRotation({ 0, 0, -leanAngle });
	card2_R->setPosition({ 0.76f, 1.6f, 0 });
	m_graphicsEngine->addGameObject(std::move(card2_R));

	auto card3_L = std::make_unique<Plane>(res_desc);
	card3_L->setColor(backColor);
	card3_L->setScale(cardScale);
	card3_L->setRotation({ 0, 0, leanAngle });
	card3_L->setPosition({ 1.88f, 1.6f, 0 });
	m_graphicsEngine->addGameObject(std::move(card3_L));

	auto card3_R = std::make_unique<Plane>(res_desc);
	card3_R->setColor(frontColor);
	card3_R->setScale(cardScale);
	card3_R->setRotation({ 0, 0, -leanAngle });
	card3_R->setPosition({ 3.0f, 1.6f, 0 });
	m_graphicsEngine->addGameObject(std::move(card3_R));

	//-----------------//

	auto flatCard = std::make_unique<Plane>(res_desc);
	flatCard->setColor(frontColor);
	flatCard->setScale(cardScale);
	flatCard->setRotation({ 0, 0, 0 });
	flatCard->setPosition({ 1.3f, 2.71f, 0 });
	m_graphicsEngine->addGameObject(std::move(flatCard));

	auto flatCard_2 = std::make_unique<Plane>(res_desc);
	flatCard_2->setColor(frontColor);
	flatCard_2->setScale(cardScale);
	flatCard_2->setRotation({ 0, 0, 0 });
	flatCard_2->setPosition({ -0.8f, 2.71f, 0 });
	m_graphicsEngine->addGameObject(std::move(flatCard_2));

	//-------------------//

	auto midCard1_L = std::make_unique<Plane>(res_desc);
	midCard1_L->setColor(backColor);
	midCard1_L->setScale(cardScale);
	midCard1_L->setRotation({ 0, 0, leanAngle });
	midCard1_L->setPosition({ -1.48f, 3.82f, 0 });
	m_graphicsEngine->addGameObject(std::move(midCard1_L));

	auto midCard1_R = std::make_unique<Plane>(res_desc);
	midCard1_R->setColor(frontColor);
	midCard1_R->setScale(cardScale);
	midCard1_R->setRotation({ 0, 0, -leanAngle });
	midCard1_R->setPosition({ -0.36f, 3.82f, 0 });
	m_graphicsEngine->addGameObject(std::move(midCard1_R));

	auto midCard2_L = std::make_unique<Plane>(res_desc);
	midCard2_L->setColor(backColor);
	midCard2_L->setScale(cardScale);
	midCard2_L->setRotation({ 0, 0, leanAngle });
	midCard2_L->setPosition({ 0.76f, 3.82f, 0 });
	m_graphicsEngine->addGameObject(std::move(midCard2_L));

	auto midCard2_R = std::make_unique<Plane>(res_desc);
	midCard2_R->setColor(frontColor);
	midCard2_R->setScale(cardScale);
	midCard2_R->setRotation({ 0, 0, -leanAngle });
	midCard2_R->setPosition({ 1.88f, 3.82f, 0 });
	m_graphicsEngine->addGameObject(std::move(midCard2_R));

	//-----------------//

	auto flatCard_3 = std::make_unique<Plane>(res_desc);
	flatCard_3->setColor(frontColor);
	flatCard_3->setScale(cardScale);
	flatCard_3->setRotation({ 0, 0, 0 });
	flatCard_3->setPosition({ 0.25f, 4.94f, 0 });
	m_graphicsEngine->addGameObject(std::move(flatCard_3));

	//-----------------//

	auto topCard_L = std::make_unique<Plane>(res_desc);
	topCard_L->setColor(backColor);
	topCard_L->setScale(cardScale);
	topCard_L->setRotation({ 0, 0, leanAngle });
	topCard_L->setPosition({ -0.36f, 6.04f, 0 });
	m_graphicsEngine->addGameObject(std::move(topCard_L));

	auto topCard_R = std::make_unique<Plane>(res_desc);
	topCard_R->setColor(frontColor);
	topCard_R->setScale(cardScale);
	topCard_R->setRotation({ 0, 0, -leanAngle });
	topCard_R->setPosition({ 0.76f, 6.04f, 0 });
	m_graphicsEngine->addGameObject(std::move(topCard_R));

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