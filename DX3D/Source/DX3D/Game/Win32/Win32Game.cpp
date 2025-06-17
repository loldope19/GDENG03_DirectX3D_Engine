#include <DX3D/Game/Game.h>
#include <DX3D/Core/EngineTime.h>
#include <Windows.h>
#include <DX3D/Graphics/GraphicsEngine.h>

void dx3d::Game::run()
{
	bool rightMouseButtonDown = false;
	POINT lastMousePos = {};
	bool key_w = false, key_s = false, key_a = false, key_d = false;

	MSG msg{};
	while (m_isRunning)
	{
		EngineTime::logFrameStart();
		float mouseDeltaX = 0, mouseDeltaY = 0;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				m_isRunning = false;
				break;
			}

			switch (msg.message)
			{
			case WM_RBUTTONDOWN:
				rightMouseButtonDown = true;
				GetCursorPos(&lastMousePos);
				ShowCursor(FALSE);
				break;
			case WM_RBUTTONUP:
				rightMouseButtonDown = false;
				ShowCursor(TRUE);
				break;
			case WM_MOUSEMOVE:
				if (rightMouseButtonDown)
				{
					POINT currentMousePos;
					GetCursorPos(&currentMousePos);
					mouseDeltaX = static_cast<float>(currentMousePos.x - lastMousePos.x);
					mouseDeltaY = static_cast<float>(currentMousePos.y - lastMousePos.y);
					SetCursorPos(lastMousePos.x, lastMousePos.y);
				}
				break;
			case WM_KEYDOWN:
			{
				if (msg.wParam == 'W') key_w = true;
				if (msg.wParam == 'S') key_s = true;
				if (msg.wParam == 'A') key_a = true;
				if (msg.wParam == 'D') key_d = true;

				if (msg.wParam == VK_SPACE) { GraphicsEngine::toggleRotation(); }

				// Time scale logic
				float newScale = EngineTime::getTimeScale();
				if (msg.wParam == VK_OEM_PLUS) { newScale += 0.5f; EngineTime::setTimeScale(newScale); }
				else if (msg.wParam == VK_OEM_MINUS) { newScale -= 0.5f; EngineTime::setTimeScale(newScale); }
				else if (msg.wParam == '1') { newScale = 1.0f; EngineTime::setTimeScale(newScale); }
				break;
			}
			case WM_KEYUP:
			{
				if (msg.wParam == 'W') key_w = false;
				if (msg.wParam == 'S') key_s = false;
				if (msg.wParam == 'A') key_a = false;
				if (msg.wParam == 'D') key_d = false;
				break;
			}
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!m_isRunning) break;

		EngineTime::logFrameEnd();
		float dt = static_cast<float>(EngineTime::getDeltaTime());

		onUpdate(dt, key_w, key_s, key_a, key_d, mouseDeltaX, mouseDeltaY);
		onRender();
	}
}
