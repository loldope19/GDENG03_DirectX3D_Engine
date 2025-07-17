#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Core.h>
#include <memory> 

#include <DX3D/ECS/PhysicsSystem.h>

namespace dx3d
{
	class GraphicsEngine;
	class Logger;
	class Display;
	class PhysicsSystem;

	class Game final : public Base
	{
	public:
		enum class EngineState
		{
			Stopped,
			Playing,
			Paused
		};

		explicit Game(const GameDesc& desc);
		virtual ~Game() override;

		void run();

		void play();
		void pause();
		void stop();
		EngineState getEngineState() const;

	private:
		void onUpdate(float dt);
		void onRender();

	private:
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<Display> m_display{};
		std::unique_ptr<PhysicsSystem> m_physicsSystem;
		std::unique_ptr<GraphicsEngine> m_graphicsEngine{};
		EngineState m_engineState{ EngineState::Stopped };
		bool m_isRunning{ true };
	};
}