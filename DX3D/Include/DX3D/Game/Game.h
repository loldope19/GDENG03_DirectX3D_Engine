#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Core.h>

namespace dx3d {
	class Game : public Base
	{
	public:
		explicit Game(const GameDesc& desc);
		virtual ~Game() override;

		virtual void run() final;

	private:
		void onUpdate(float dt, bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float deltaX, float deltaY);
		void onRender();

	private:
		std::unique_ptr<Logger> m_loggerPtr{};
		std::unique_ptr<GraphicsEngine> m_graphicsEngine{};
		std::unique_ptr<Display> m_display{};
		bool m_isRunning{ true };
	};

}

