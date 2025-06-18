#include <DX3D/Game/Game.h>
#include <DX3D/Core/EngineTime.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Game/InputManager.h>
#include <Windows.h>
#include <algorithm>

void dx3d::Game::run()
{
	MSG msg{};
	while (m_isRunning)
	{
		EngineTime::logFrameStart();

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				m_isRunning = false;
				break;
			}

			InputManager::getInstance()->processMessage(msg);

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!m_isRunning) break;

		InputManager::getInstance()->update();

		float newScale = EngineTime::getTimeScale();
		if (InputManager::getInstance()->isKeyUp(VK_OEM_PLUS)) {
			newScale += 0.5f; EngineTime::setTimeScale(newScale);
		}
		else if (InputManager::getInstance()->isKeyUp(VK_OEM_MINUS)) {
			newScale -= 0.5f; EngineTime::setTimeScale(newScale);
		}
		else if (InputManager::getInstance()->isKeyUp('1')) {
			newScale = 1.0f; EngineTime::setTimeScale(newScale);
		}


		EngineTime::logFrameEnd();
		float dt = static_cast<float>(EngineTime::getDeltaTime());
		dt = std::min(dt, 1.0f / 60.0f);

		onUpdate(dt);
		onRender();
	}
}
