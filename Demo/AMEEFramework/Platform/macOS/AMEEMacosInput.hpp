#ifndef __AMEE_MACOSINPUT_H__
#define __AMEE_MACOSINPUT_H__
#pragma once
#include "../../Core/Platform/IAMEEPlatformInput.hpp"

namespace AMEE {

class MacosInput : public IPlatformInput {
public:
    MacosInput();
    ~MacosInput() override;

    void Update() override;

    bool IsKeyDown(KeyCode Key) const override;
    bool IsKeyPressed(KeyCode Key) const override;
    bool IsKeyReleased(KeyCode Key) const override;

    void GetMousePosition(float& OutX, float& OutY) const override;
    void GetMouseDelta(float& OutDX, float& OutDY) const override;
    bool IsMouseButtonDown(MouseButton Button) const override;
    bool IsMouseButtonPressed(MouseButton Button) const override;
    bool IsMouseButtonReleased(MouseButton Button) const override;

private:
    static constexpr int KeyCount = (int)KeyCode::Count;
    static constexpr int MouseButtonCount = (int)MouseButton::Count;

    bool m_Keys[KeyCount];
    bool m_PrevKeys[KeyCount];
    bool m_MouseButtons[MouseButtonCount];
    bool m_PrevMouseButtons[MouseButtonCount];
    float m_MouseX, m_MouseY;
    float m_MouseDX, m_MouseDY;
    void* m_Monitor; // id
};

} // namespace AMEE

#endif // __AMEE_MACOSINPUT_H__
