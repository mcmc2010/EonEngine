#import "AMEEMacosInput.hpp"
#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>
#import "../../Core/Log/AMEELog.hpp"
#include <cstring>

namespace AMEE {

static int kVKToKeyCode(CGKeyCode vk)
{
    switch (vk) {
        case kVK_ANSI_A: return (int)KeyCode::A;    case kVK_ANSI_B: return (int)KeyCode::B;
        case kVK_ANSI_C: return (int)KeyCode::C;    case kVK_ANSI_D: return (int)KeyCode::D;
        case kVK_ANSI_E: return (int)KeyCode::E;    case kVK_ANSI_F: return (int)KeyCode::F;
        case kVK_ANSI_G: return (int)KeyCode::G;    case kVK_ANSI_H: return (int)KeyCode::H;
        case kVK_ANSI_I: return (int)KeyCode::I;    case kVK_ANSI_J: return (int)KeyCode::J;
        case kVK_ANSI_K: return (int)KeyCode::K;    case kVK_ANSI_L: return (int)KeyCode::L;
        case kVK_ANSI_M: return (int)KeyCode::M;    case kVK_ANSI_N: return (int)KeyCode::N;
        case kVK_ANSI_O: return (int)KeyCode::O;    case kVK_ANSI_P: return (int)KeyCode::P;
        case kVK_ANSI_Q: return (int)KeyCode::Q;    case kVK_ANSI_R: return (int)KeyCode::R;
        case kVK_ANSI_S: return (int)KeyCode::S;    case kVK_ANSI_T: return (int)KeyCode::T;
        case kVK_ANSI_U: return (int)KeyCode::U;    case kVK_ANSI_V: return (int)KeyCode::V;
        case kVK_ANSI_W: return (int)KeyCode::W;    case kVK_ANSI_X: return (int)KeyCode::X;
        case kVK_ANSI_Y: return (int)KeyCode::Y;    case kVK_ANSI_Z: return (int)KeyCode::Z;
        case kVK_ANSI_0: return (int)KeyCode::Key0; case kVK_ANSI_1: return (int)KeyCode::Key1;
        case kVK_ANSI_2: return (int)KeyCode::Key2; case kVK_ANSI_3: return (int)KeyCode::Key3;
        case kVK_ANSI_4: return (int)KeyCode::Key4; case kVK_ANSI_5: return (int)KeyCode::Key5;
        case kVK_ANSI_6: return (int)KeyCode::Key6; case kVK_ANSI_7: return (int)KeyCode::Key7;
        case kVK_ANSI_8: return (int)KeyCode::Key8; case kVK_ANSI_9: return (int)KeyCode::Key9;
        case kVK_F1:  return (int)KeyCode::F1;  case kVK_F2:  return (int)KeyCode::F2;
        case kVK_F3:  return (int)KeyCode::F3;  case kVK_F4:  return (int)KeyCode::F4;
        case kVK_F5:  return (int)KeyCode::F5;  case kVK_F6:  return (int)KeyCode::F6;
        case kVK_F7:  return (int)KeyCode::F7;  case kVK_F8:  return (int)KeyCode::F8;
        case kVK_F9:  return (int)KeyCode::F9;  case kVK_F10: return (int)KeyCode::F10;
        case kVK_F11: return (int)KeyCode::F11; case kVK_F12: return (int)KeyCode::F12;
        case kVK_LeftArrow:  return (int)KeyCode::Left;  case kVK_RightArrow: return (int)KeyCode::Right;
        case kVK_UpArrow:    return (int)KeyCode::Up;    case kVK_DownArrow:  return (int)KeyCode::Down;
        case kVK_Shift:      return (int)KeyCode::Shift;
        case kVK_Control:    return (int)KeyCode::Control;
        case kVK_Option:     return (int)KeyCode::Alt;
        case kVK_Command:    return (int)KeyCode::Command;
        case kVK_Space:      return (int)KeyCode::Space;
        case kVK_Escape:     return (int)KeyCode::Escape;
        case kVK_Return:     return (int)KeyCode::Return;
        case kVK_Tab:        return (int)KeyCode::Tab;
        case kVK_Delete:     return (int)KeyCode::Backspace;
        case kVK_ForwardDelete: return (int)KeyCode::Delete;
        default: return (int)KeyCode::Unknown;
    }
}

MacosInput::MacosInput()
    : m_Monitor(nullptr)
{
    std::memset(m_Keys, 0, sizeof(m_Keys));
    std::memset(m_PrevKeys, 0, sizeof(m_PrevKeys));
    std::memset(m_MouseButtons, 0, sizeof(m_MouseButtons));
    std::memset(m_PrevMouseButtons, 0, sizeof(m_PrevMouseButtons));
    m_MouseX = m_MouseY = m_MouseDX = m_MouseDY = 0;

    m_Monitor = (__bridge_retained void*)[NSEvent addLocalMonitorForEventsMatchingMask:
        NSEventMaskKeyDown | NSEventMaskKeyUp |
        NSEventMaskLeftMouseDown | NSEventMaskLeftMouseUp |
        NSEventMaskRightMouseDown | NSEventMaskRightMouseUp |
        NSEventMaskOtherMouseDown | NSEventMaskOtherMouseUp |
        NSEventMaskMouseMoved | NSEventMaskLeftMouseDragged |
        NSEventMaskRightMouseDragged | NSEventMaskOtherMouseDragged |
        NSEventMaskFlagsChanged
        handler:^NSEvent*(NSEvent* event) {
            switch ([event type]) {
                case NSEventTypeKeyDown: {
                    int idx = kVKToKeyCode((CGKeyCode)[event keyCode]);
                    if (idx >= 0 && idx < KeyCount) m_Keys[idx] = true;
                    break;
                }
                case NSEventTypeKeyUp: {
                    int idx = kVKToKeyCode((CGKeyCode)[event keyCode]);
                    if (idx >= 0 && idx < KeyCount) m_Keys[idx] = false;
                    break;
                }
                case NSEventTypeFlagsChanged: {
                    // Modifier keys: detect from modifierFlags
                    m_Keys[(int)KeyCode::Shift]   = ([event modifierFlags] & NSEventModifierFlagShift) != 0;
                    m_Keys[(int)KeyCode::Control] = ([event modifierFlags] & NSEventModifierFlagControl) != 0;
                    m_Keys[(int)KeyCode::Alt]     = ([event modifierFlags] & NSEventModifierFlagOption) != 0;
                    m_Keys[(int)KeyCode::Command] = ([event modifierFlags] & NSEventModifierFlagCommand) != 0;
                    break;
                }
                case NSEventTypeLeftMouseDown:    m_MouseButtons[0] = true;  break;
                case NSEventTypeLeftMouseUp:      m_MouseButtons[0] = false; break;
                case NSEventTypeRightMouseDown:   m_MouseButtons[1] = true;  break;
                case NSEventTypeRightMouseUp:     m_MouseButtons[1] = false; break;
                case NSEventTypeOtherMouseDown:   m_MouseButtons[2] = true;  break;
                case NSEventTypeOtherMouseUp:     m_MouseButtons[2] = false; break;
                case NSEventTypeMouseMoved:
                case NSEventTypeLeftMouseDragged:
                case NSEventTypeRightMouseDragged:
                case NSEventTypeOtherMouseDragged:
                    m_MouseDX += (float)[event deltaX];
                    m_MouseDY += (float)[event deltaY];
                    break;
                default: break;
            }
            return event;
        }];
}

MacosInput::~MacosInput()
{
    if (m_Monitor) {
        [NSEvent removeMonitor:(__bridge id)m_Monitor];
        CFRelease(m_Monitor);
        m_Monitor = nullptr;
    }
}

void MacosInput::Update()
{
    std::memcpy(m_PrevKeys, m_Keys, sizeof(m_Keys));
    std::memcpy(m_PrevMouseButtons, m_MouseButtons, sizeof(m_MouseButtons));

    m_MouseX += m_MouseDX;
    m_MouseY += m_MouseDY;
    m_MouseDX = 0;
    m_MouseDY = 0;
}

bool MacosInput::IsKeyDown(KeyCode Key) const
{
    int idx = (int)Key;
    return idx >= 0 && idx < KeyCount && m_Keys[idx];
}

bool MacosInput::IsKeyPressed(KeyCode Key) const
{
    int idx = (int)Key;
    return idx >= 0 && idx < KeyCount && m_Keys[idx] && !m_PrevKeys[idx];
}

bool MacosInput::IsKeyReleased(KeyCode Key) const
{
    int idx = (int)Key;
    return idx >= 0 && idx < KeyCount && !m_Keys[idx] && m_PrevKeys[idx];
}

void MacosInput::GetMousePosition(float& OutX, float& OutY) const
{
    OutX = m_MouseX;
    OutY = m_MouseY;
}

void MacosInput::GetMouseDelta(float& OutDX, float& OutDY) const
{
    OutDX = m_MouseDX;
    OutDY = m_MouseDY;
}

bool MacosInput::IsMouseButtonDown(MouseButton Button) const
{
    int idx = (int)Button;
    return idx >= 0 && idx < MouseButtonCount && m_MouseButtons[idx];
}

bool MacosInput::IsMouseButtonPressed(MouseButton Button) const
{
    int idx = (int)Button;
    return idx >= 0 && idx < MouseButtonCount && m_MouseButtons[idx] && !m_PrevMouseButtons[idx];
}

bool MacosInput::IsMouseButtonReleased(MouseButton Button) const
{
    int idx = (int)Button;
    return idx >= 0 && idx < MouseButtonCount && !m_MouseButtons[idx] && m_PrevMouseButtons[idx];
}

} // namespace AMEE
