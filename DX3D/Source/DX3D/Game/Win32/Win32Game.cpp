#include <DX3D/Game/Game.h>
#include <DX3D/Core/EngineTime.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Game/InputManager.h>
#include <Windows.h>
#include <algorithm>

#include <IMGUI/imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void dx3d::Game::run()
{
	MSG msg{};
	const double targetFrameTime = 1.0 / 60.0;
	double frameDuration;

	while (m_isRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				m_isRunning = false;
				continue;
			}

			InputManager::getInstance()->processMessage(msg);

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float dt = static_cast<float>(EngineTime::getDeltaTime());

			EngineTime::logFrameStart();

			InputManager::getInstance()->update();
			onUpdate(dt);
			onRender();

			do
			{
				EngineTime::logFrameEnd();
				frameDuration = EngineTime::getUnscaledDeltaTime();
				if (targetFrameTime - frameDuration > 0.002) { Sleep(1); }
			} while (frameDuration < targetFrameTime);
		}
	}
}