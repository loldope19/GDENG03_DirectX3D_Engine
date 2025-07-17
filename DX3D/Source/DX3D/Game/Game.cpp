#include <DX3D/Game/Game.h>
#include <DX3D/Window/Window.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Core/Logger.h>
#include <DX3D/Game/Display.h>
#include <DX3D/Core/EngineTime.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <memory>

// Include the new primitive headers
#include <DX3D/Graphics/Cube.h>
#include <DX3D/Graphics/TexturedCube.h>
#include <DX3D/Graphics/Plane.h>
#include <DX3D/Graphics/Sphere.h>
#include <DX3D/Graphics/Cylinder.h>
#include <DX3D/Graphics/Capsule.h>

#include <DX3D/ECS/PhysicsSystem.h>
#include <DX3D/ECS/PhysicsComponent.h>

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_win32.h>
#include <IMGUI/imgui_impl_dx11.h>
#pragma comment(lib, "winmm.lib")

dx3d::Game::Game(const GameDesc& desc) :
	Base({ *std::make_unique<Logger>(desc.logLevel).release() }),
	m_loggerPtr(&m_logger)
{
	timeBeginPeriod(1);
	EngineTime::initialize();

	m_physicsSystem = std::make_unique<PhysicsSystem>();
	GraphicsEngineDesc ge_desc = { {m_logger}, desc.windowSize.width, desc.windowSize.height };
	m_graphicsEngine = std::make_unique<GraphicsEngine>(ge_desc, m_physicsSystem.get(), this);

	m_display = std::make_unique<Display>(DisplayDesc{ {m_logger,desc.windowSize}, m_graphicsEngine->getGraphicsDevice() });
	GraphicsResourceDesc res_desc = m_graphicsEngine->getGraphicsResourceDesc();
	/*
	auto myCube = std::make_unique<Cube>(res_desc);
	myCube->setPosition({ 0, 5, 0 }); // Start it 5 meters up
	myCube->setName("Falling Cube");
	auto cubePhysics = myCube->addComponent<PhysicsComponent>(m_physicsSystem.get(), PhysicsComponent::BodyShape::Box, myCube->getScale());
	cubePhysics->getRigidBody()->setLinearDamping(0.5f);
	cubePhysics->getRigidBody()->setAngularDamping(0.5f);
	m_graphicsEngine->addGameObject(std::move(myCube));

	auto groundPlane = std::make_unique<Plane>(res_desc);
	groundPlane->setPosition({ 0, 0, 0 });
	groundPlane->setScale({ 20, 0.01f, 20 });
	groundPlane->setName("Ground");
	auto groundPhysics = groundPlane->addComponent<PhysicsComponent>
		(m_physicsSystem.get(), PhysicsComponent::BodyShape::StaticPlane, groundPlane->getScale(), 
		Vec3{ 0.0f, -groundPlane->getScale().y / 2.0f, 0.0f });
	groundPhysics->getRigidBody()->setType(reactphysics3d::BodyType::STATIC);
	reactphysics3d::Material& groundMaterial = groundPhysics->getRigidBody()->getCollider(0)->getMaterial();
	groundMaterial.setFrictionCoefficient(0.7f);
	m_graphicsEngine->addGameObject(std::move(groundPlane));

	auto plane = std::make_unique<Plane>(res_desc);
	plane->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	plane->setScale(Vec3(20.0f, 20.0f, 20.0f));
	plane->setName("Plane");
	m_graphicsEngine->addGameObject(std::move(plane));
	
	auto cube = std::make_unique<Cube>(res_desc);
	cube->setScale(Vec3(3.0f, 3.0f, 3.0f));
	cube->setName("Cube");
	m_graphicsEngine->addGameObject(std::move(cube));

	auto texCube = std::make_unique<TexturedCube>(res_desc, L"Assets/Textures/lmao.png");
	texCube->setPosition({ 0, 2, 0 });
	m_graphicsEngine->addGameObject(std::move(texCube));
	*/

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	// Initialize the backends
	ImGui_ImplWin32_Init(m_display->getHandle());
	ImGui_ImplDX11_Init(m_graphicsEngine->getGraphicsDevice().m_d3dDevice.Get(),
		m_graphicsEngine->getGraphicsDevice().m_d3dContext.Get());

	DX3DLogInfo("Game initialized.");
}

dx3d::Game::~Game()
{
	DX3DLogInfo("Game is shutting down...");

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	EngineTime::release();
	timeEndPeriod(1);
}

void dx3d::Game::play() { m_engineState = EngineState::Playing; }
void dx3d::Game::pause() { m_engineState = EngineState::Paused; }
void dx3d::Game::stop() { m_engineState = EngineState::Stopped; }
dx3d::Game::EngineState dx3d::Game::getEngineState() const { return m_engineState; }

void dx3d::Game::onUpdate(float dt)
{
	if (m_engineState == EngineState::Playing)
	{
		if (dt > 0.0f)
		{
			m_physicsSystem->update(dt);
		}
	}
	m_graphicsEngine->onUpdate(dt);
}

void dx3d::Game::onRender()
{
	m_graphicsEngine->render(m_display->getSwapChain());
}