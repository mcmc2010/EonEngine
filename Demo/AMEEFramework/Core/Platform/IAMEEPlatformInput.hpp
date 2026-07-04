#ifndef __AMEE_IPLATFORMINPUT_H__
#define __AMEE_IPLATFORMINPUT_H__
#pragma once
#include <cstdint>

namespace AMEE {

enum class KeyCode : int {
    Unknown = -1,

    // Letters
    A = 0, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Numbers
    Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,

    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // Navigation
    Left, Right, Up, Down,

    // Modifiers
    Shift, Control, Alt, Command,

    // Special
    Space, Escape, Return, Tab, Backspace, Delete,

    Count
};

enum class MouseButton : int {
    Left = 0,
    Right = 1,
    Middle = 2,
    Count
};

class IPlatformInput {
public:
    virtual ~IPlatformInput() = default;

    virtual void Update() = 0;

    // Keyboard
    virtual bool IsKeyDown(KeyCode Key) const = 0;
    virtual bool IsKeyPressed(KeyCode Key) const = 0;
    virtual bool IsKeyReleased(KeyCode Key) const = 0;

    // Mouse
    virtual void GetMousePosition(float& OutX, float& OutY) const = 0;
    virtual void GetMouseDelta(float& OutDX, float& OutDY) const = 0;
    virtual bool IsMouseButtonDown(MouseButton Button) const = 0;
    virtual bool IsMouseButtonPressed(MouseButton Button) const = 0;
    virtual bool IsMouseButtonReleased(MouseButton Button) const = 0;
};

} // namespace AMEE

#endif // __AMEE_IPLATFORMINPUT_H__
