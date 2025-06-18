#include <DX3D/Game/Game.h>
#include <Windows.h>

#include <chrono>   // for frame rate limiting
#include <thread>

void dx3d::Game::run()
{
	constexpr std::chrono::nanoseconds desiredFrameTime(1'000'000'000 / 60);	// 60fps

	MSG msg{};
	while (m_isRunning) 
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				m_isRunning = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (m_isRunning)
		{
			onInternalUpdate();
		}

		auto endTime = std::chrono::high_resolution_clock::now();
		auto frameDuration = endTime - startTime;

		if (frameDuration < desiredFrameTime)
		{
			std::this_thread::sleep_for(desiredFrameTime - frameDuration);
		}
	}
}
