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
#include <DX3D/Graphics/Model.h>
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
	
	auto teapot = std::make_unique<Model>(res_desc,
		"Assets/Models/teapot.obj", 
		L"Assets/Textures/lmao.png");
	teapot->setName("Teapot");
	teapot->setScale({ 0.1f, 0.1f, 0.1f });
	teapot->setPosition({ -4.0f, 0.0f, 0.0f });
	m_graphicsEngine->addGameObject(std::move(teapot));

	// 2. Load Stanford Bunny (no texture)
	auto bunny = std::make_unique<Model>(res_desc,
		"Assets/Models/bunny.obj");
	bunny->setName("Bunny");
	bunny->setScale({ 2.0f, 2.0f, 2.0f });
	bunny->setPosition({ 0.0f, -1.0f, 0.0f });
	m_graphicsEngine->addGameObject(std::move(bunny));

	// 3. Load Armadillo (no texture)
	auto armadillo = std::make_unique<Model>(res_desc,
		"Assets/Models/armadillo.obj");
	armadillo->setName("Armadillo");
	armadillo->setScale({ 0.1f, 0.1f, 0.1f });
	armadillo->setPosition({ 4.0f, -1.0f, 0.0f });
	m_graphicsEngine->addGameObject(std::move(armadillo));
	

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