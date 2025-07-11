#include <DX3D/Game/InputManager.h>

namespace dx3d
{
    InputManager* InputManager::getInstance()
    {
        static InputManager instance;
        return &instance;
    }

    InputManager::InputManager()
    {
        if (!::GetKeyboardState(m_keyState))
        {
            memset(m_keyState, 0, sizeof(m_keyState));
        }

        memcpy(m_prevKeyState, m_keyState, sizeof(m_keyState));
    }

    void InputManager::processMessage(MSG& msg)
    {
        switch (msg.message)
        {
        case WM_RBUTTONDOWN:
            m_rightMouseDown = true;
            break;
        case WM_RBUTTONUP:
            m_rightMouseDown = false;
            break;
        }
    }

    void InputManager::update()
    {
        memcpy(m_prevKeyState, m_keyState, sizeof(m_keyState));

        if (!::GetKeyboardState(m_keyState))
        {
            memset(m_keyState, 0, sizeof(m_keyState));
        }

        POINT currentMousePos;
        ::GetCursorPos(&currentMousePos);

        if (m_rightMouseDown)
        {
            m_mouseDeltaX = static_cast<float>(currentMousePos.x - m_lastMousePos.x);
            m_mouseDeltaY = static_cast<float>(currentMousePos.y - m_lastMousePos.y);
        }
        else
        {
            m_mouseDeltaX = 0;
            m_mouseDeltaY = 0;
        }
        m_lastMousePos = currentMousePos;
    }

    bool InputManager::isKeyDown(int vk_code) const
    {
        return (m_keyState[vk_code] & 0x80) != 0;
    }

    bool InputManager::isKeyUp(int vk_code) const
    {
        // True if key was down before but is up now
        return (m_prevKeyState[vk_code] & 0x80) != 0 && (m_keyState[vk_code] & 0x80) == 0;
    }

    void InputManager::getMouseDelta(float& deltaX, float& deltaY) const
    {
        deltaX = m_mouseDeltaX;
        deltaY = m_mouseDeltaY;
    }

    bool InputManager::isRightMouseDown() const
    {
        return m_rightMouseDown;
    }
}