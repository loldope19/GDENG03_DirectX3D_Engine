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
		InputManager::getInstance()->update();

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

		float newScale = EngineTime::getTimeScale();
		if (InputManager::getInstance()->isKeyUp(VK_OEM_PLUS)) {
			DX3DLogInfo("Engine Time increased.");
			newScale += 0.5f; EngineTime::setTimeScale(newScale);
		}
		else if (InputManager::getInstance()->isKeyUp(VK_OEM_MINUS)) {
			DX3DLogInfo("Engine Time decreased.");
			newScale -= 0.5f; EngineTime::setTimeScale(newScale);
		}

		if (InputManager::getInstance()->isKeyUp('1')) {
			m_graphicsEngine->selectObject(0); // Select Plane
		}
		if (InputManager::getInstance()->isKeyUp('2')) {
			m_graphicsEngine->selectObject(1); // Select Cube 1
		}
		if (InputManager::getInstance()->isKeyUp('3')) {
			m_graphicsEngine->selectObject(2); // Select Cube 2
		}
		if (InputManager::getInstance()->isKeyUp('0')) {
			m_graphicsEngine->selectObject(-1); // Deselect
		}

		EngineTime::logFrameEnd();
		float dt = static_cast<float>(EngineTime::getDeltaTime());
		dt = std::min(dt, 1.0f / 60.0f);

		onUpdate(dt);
		onRender();
	}
}
