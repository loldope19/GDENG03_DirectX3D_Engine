#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Core.h>
#include <memory> 

namespace dx3d
{
	class GraphicsEngine;
	class Logger;
	class Display;

	class Game final : public Base
	{
	public:
		explicit Game(const GameDesc& desc);
		virtual ~Game() override;

		void run();

	private:
		void onUpdate(float dt);
		void onRender();

	private:
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<GraphicsEngine> m_graphicsEngine{};
		std::unique_ptr<Display> m_display{};
		bool m_isRunning{ true };
	};
}