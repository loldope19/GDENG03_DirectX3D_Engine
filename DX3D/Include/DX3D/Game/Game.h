#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Core.h>
#include <DX3D/Graphics/Camera.h>
#include <DX3D/Input/InputManager.h>

#include <vector>
#include <random> 
#include <chrono> 
#include <DirectXMath.h> 

namespace dx3d {
	struct BouncingCircle
	{
		DirectX::XMFLOAT2 position;
		DirectX::XMFLOAT2 velocity;
		float radius;
		DirectX::XMFLOAT4 color;
	};

	class Game : public Base
	{
	public:
		explicit Game(const GameDesc& desc);
		virtual ~Game() override;

		virtual void run() final;

	private:
		void onInternalUpdate();
		void handleInput();
		void spawnCircle();

	private:
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<GraphicsEngine> m_graphicsEngine{};
		std::unique_ptr<Display> m_display{};

		std::unique_ptr<Camera> m_camera{};
		std::unique_ptr<InputManager> m_inputManager{};

		bool m_isRunning{ true };

		std::vector<BouncingCircle> m_circles;
		std::chrono::steady_clock::time_point m_lastFrameTime;

		bool m_spaceWasDown = false;
		bool m_backspaceWasDown = false;

		// Random number generation for new circles
		std::mt19937 m_randomEngine;
	};

}

