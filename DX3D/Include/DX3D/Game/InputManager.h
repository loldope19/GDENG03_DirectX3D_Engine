#pragma once
#include <Windows.h>

namespace dx3d
{
    class InputManager
    {
    public:
        static InputManager* getInstance();

        void processMessage(MSG& msg);
        void update();

        bool isKeyDown(int vk_code) const;
        bool isKeyUp(int vk_code) const;

        void getMouseDelta(float& deltaX, float& deltaY) const;
        bool isRightMouseDown() const;

    private:
        InputManager();
        ~InputManager() = default;
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        BYTE m_keyState[256] = {};
        BYTE m_prevKeyState[256] = {};

        bool m_rightMouseDown = false;
        POINT m_lastMousePos = {};
        float m_mouseDeltaX = 0;
        float m_mouseDeltaY = 0;
    };
}