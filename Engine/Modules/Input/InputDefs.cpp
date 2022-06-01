#include "InputDefs.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../../utility/ConsoleLogging.h"

using Key = Thera::Input::Key;

const std::unordered_map<int, Key> GLFWToKey
{
	{ GLFW_KEY_ESCAPE, Key::Escape },
	{ GLFW_KEY_F1, Key::F1 },
	{ GLFW_KEY_F2, Key::F2 },
	{ GLFW_KEY_F3, Key::F3 },
	{ GLFW_KEY_F4, Key::F4 },
	{ GLFW_KEY_F5, Key::F5 },
	{ GLFW_KEY_F6, Key::F6 },
	{ GLFW_KEY_F7, Key::F7 },
	{ GLFW_KEY_F8, Key::F8 },
	{ GLFW_KEY_F9, Key::F9 },
	{ GLFW_KEY_F10, Key::F10 },
	{ GLFW_KEY_F11, Key::F11 },
	{ GLFW_KEY_F12, Key::F12 },
	{ GLFW_KEY_F13, Key::F13 },
	{ GLFW_KEY_F14, Key::F14 },
	{ GLFW_KEY_F15, Key::F15 },
	{ GLFW_KEY_F16, Key::F16 },
	{ GLFW_KEY_F17, Key::F17 },
	{ GLFW_KEY_F18, Key::F18 },
	{ GLFW_KEY_F19, Key::F19 },
	{ GLFW_KEY_F20, Key::F20 },
	{ GLFW_KEY_F21, Key::F21 },
	{ GLFW_KEY_F22, Key::F22 },
	{ GLFW_KEY_F23, Key::F23 },
	{ GLFW_KEY_F24, Key::F24 },
	{ GLFW_KEY_F25, Key::F25 },
	{ GLFW_KEY_PRINT_SCREEN, Key::PrintScreen },
	{ GLFW_KEY_SCROLL_LOCK, Key::ScrollLock },
	{ GLFW_KEY_PAUSE, Key::Pause },
	{ GLFW_KEY_GRAVE_ACCENT, Key::Grave },
	{ GLFW_KEY_1, Key::Alpha1 },
	{ GLFW_KEY_2, Key::Alpha2 },
	{ GLFW_KEY_3, Key::Alpha3 },
	{ GLFW_KEY_4, Key::Alpha4 },
	{ GLFW_KEY_5, Key::Alpha5 },
	{ GLFW_KEY_6, Key::Alpha6 },
	{ GLFW_KEY_7, Key::Alpha7 },
	{ GLFW_KEY_8, Key::Alpha8 },
	{ GLFW_KEY_9, Key::Alpha9 },
	{ GLFW_KEY_0, Key::Alpha0 },
	{ GLFW_KEY_MINUS, Key::Minus },
	{ GLFW_KEY_EQUAL, Key::Equal },
	{ GLFW_KEY_BACKSPACE, Key::Backspace },
	{ GLFW_KEY_INSERT, Key::Insert },
	{ GLFW_KEY_HOME, Key::Home },
	{ GLFW_KEY_PAGE_UP, Key::PageUp},
	{ GLFW_KEY_TAB, Key::Tab },
	{ GLFW_KEY_Q, Key::Q },
	{ GLFW_KEY_W, Key::W },
	{ GLFW_KEY_E, Key::E },
	{ GLFW_KEY_R, Key::R },
	{ GLFW_KEY_T, Key::T },
	{ GLFW_KEY_Y, Key::Y },
	{ GLFW_KEY_U, Key::U },
	{ GLFW_KEY_I, Key::I },
	{ GLFW_KEY_O, Key::O },
	{ GLFW_KEY_P, Key::P },
	{ GLFW_KEY_LEFT_BRACKET, Key::BracketLeft },
	{ GLFW_KEY_RIGHT_BRACKET, Key::BracketRight },
	{ GLFW_KEY_BACKSLASH, Key::Backslash },
	{ GLFW_KEY_DELETE, Key::Delete },
	{ GLFW_KEY_END, Key::End },
	{ GLFW_KEY_PAGE_DOWN, Key::PageDown },
	{ GLFW_KEY_CAPS_LOCK, Key::CapsLock },
	{ GLFW_KEY_A, Key::A },
	{ GLFW_KEY_S, Key::S },
	{ GLFW_KEY_D, Key::D },
	{ GLFW_KEY_F, Key::F },
	{ GLFW_KEY_G, Key::G },
	{ GLFW_KEY_H, Key::H },
	{ GLFW_KEY_J, Key::J },
	{ GLFW_KEY_K, Key::K },
	{ GLFW_KEY_L, Key::L },
	{ GLFW_KEY_SEMICOLON, Key::Semicolon },
	{ GLFW_KEY_APOSTROPHE, Key::Apostrophe },
	{ GLFW_KEY_ENTER, Key::Enter },
	{ GLFW_KEY_LEFT_SHIFT, Key::ShiftLeft },
	{ GLFW_KEY_Z, Key::Z },
	{ GLFW_KEY_X, Key::X },
	{ GLFW_KEY_C, Key::C },
	{ GLFW_KEY_V, Key::V },
	{ GLFW_KEY_B, Key::B },
	{ GLFW_KEY_N, Key::N },
	{ GLFW_KEY_M, Key::M },
	{ GLFW_KEY_COMMA, Key::Comma },
	{ GLFW_KEY_PERIOD, Key::Period },
	{ GLFW_KEY_SLASH, Key::Slash },
	{ GLFW_KEY_RIGHT_SHIFT, Key::ShiftRight },
	{ GLFW_KEY_LEFT_CONTROL, Key::ControlLeft },
	{ GLFW_KEY_LEFT_SUPER, Key::SuperLeft },
	{ GLFW_KEY_LEFT_ALT, Key::AltLeft },
	{ GLFW_KEY_SPACE, Key::Space },
	{ GLFW_KEY_RIGHT_ALT, Key::AltRight },
	{ GLFW_KEY_RIGHT_SUPER, Key::SuperRight },
	{ GLFW_KEY_MENU, Key::Menu },
	{ GLFW_KEY_RIGHT_CONTROL, Key::ControlRight },
	{ GLFW_KEY_UP, Key::Up },
	{ GLFW_KEY_DOWN, Key::Down },
	{ GLFW_KEY_LEFT, Key::Left },
	{ GLFW_KEY_RIGHT, Key::Right },
		
	{ GLFW_KEY_NUM_LOCK, Key::NumLock },
	{ GLFW_KEY_KP_DIVIDE, Key::KeypadSlash },
	{ GLFW_KEY_KP_MULTIPLY, Key::KeypadAsterisk },
	{ GLFW_KEY_KP_SUBTRACT, Key::KeypadMinus },
	{ GLFW_KEY_KP_7, Key::Keypad7 },
	{ GLFW_KEY_KP_8, Key::Keypad8 },
	{ GLFW_KEY_KP_9, Key::Keypad9 },
	{ GLFW_KEY_KP_ADD, Key::KeypadPlus },
	{ GLFW_KEY_KP_4, Key::Keypad4 },
	{ GLFW_KEY_KP_5, Key::Keypad5 },
	{ GLFW_KEY_KP_6, Key::Keypad6 },
	{ GLFW_KEY_KP_1, Key::Keypad1 },
	{ GLFW_KEY_KP_2, Key::Keypad2 },
	{ GLFW_KEY_KP_3, Key::Keypad3 },
	{ GLFW_KEY_KP_ENTER, Key::KeypadEnter },
	{ GLFW_KEY_KP_0, Key::Keypad0 },
	{ GLFW_KEY_KP_DECIMAL, Key::KeypadPeriod },
};

using Mouse = Thera::Input::Mouse;

const std::unordered_map<int, Mouse> GLFWToMouse
{
	{ GLFW_MOUSE_BUTTON_1, Mouse::ButtonLeft },
	{ GLFW_MOUSE_BUTTON_2, Mouse::ButtonRight },
	{ GLFW_MOUSE_BUTTON_3, Mouse::ButtonMiddle },
	{ GLFW_MOUSE_BUTTON_4, Mouse::Button4 },
	{ GLFW_MOUSE_BUTTON_5, Mouse::Button5 },
	{ GLFW_MOUSE_BUTTON_6, Mouse::Button6 },
	{ GLFW_MOUSE_BUTTON_7, Mouse::Button7 },
	{ GLFW_MOUSE_BUTTON_8, Mouse::Button8 }
};

Key Thera::Input::GLFWInputToKey(const int glfwInput)
{
	return GLFWToKey.at(glfwInput);
}

Mouse Thera::Input::GLFWInputToMouse(const int glfwInput)
{
	return GLFWToMouse.at(glfwInput);
}
