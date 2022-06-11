#pragma once

#include <vector>
#include <string>
#include <functional>
#include <GLFW/glfw3.h>
#include <typeinfo>
#include <stdexcept>

#include "../../utility/CCXDefs.h"

#undef DELETE

namespace Thera::Input
{
	/// <summary>
	/// Component count for output.
	/// </summary>
	enum class Output
	{
		Scalar = 1,
		Vector2,
		Vector3
	};
	/// <summary>
	/// Component size for output.
	/// </summary>
	enum class Precision
	{
		Single = 4,
		Double = 8
	};
	/// <summary>
	/// Atomic components of output data for composites.
	/// </summary>
	enum class Component
	{
		NegX = 0,
		PosX = 1,
		NegY = 2,
		PosY = 3,
		NegZ = 4,
		PosZ = 5
	};

	/// <summary>
	/// Converts underlying value of Output enum to string.
	/// </summary>
	inline std::unordered_map<int, std::string> outputToString
	{
		{ 1, "Scalar" },
		{ 2, "Vector2" },
		{ 3, "Vector3" }
	};
	/// <summary>
	/// Converts underlying value of Precision enum to string.
	/// </summary>
	inline std::unordered_map<int, std::string> precisionToString
	{
		{ 4, "Single" },
		{ 8, "Double" }
	};

	const int FirstKey = 0;
	/// <summary>
	/// Input key IDs.
	/// </summary>
	enum class Key
	{
		Escape = 0,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,
		PrintScreen,
		ScrollLock,
		Pause,
		Grave,
		Alpha1,
		Alpha2,
		Alpha3,
		Alpha4,
		Alpha5,
		Alpha6,
		Alpha7,
		Alpha8,
		Alpha9,
		Alpha0,
		Minus,
		Equal,
		Backspace,
		Insert,
		Home,
		PageUp,
		Tab,
		Q,
		W,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,
		BracketLeft,
		BracketRight,
		Backslash,
		Delete,
		End,
		PageDown,
		CapsLock,
		A,
		S,
		D,
		F,
		G,
		H,
		J,
		K,
		L,
		Semicolon,
		Apostrophe,
		Enter,
		ShiftLeft,
		Z,
		X,
		C,
		V,
		B,
		N,
		M,
		Comma,
		Period,
		Slash,
		ShiftRight,
		ControlLeft,
		SuperLeft,
		AltLeft,
		Space,
		AltRight,
		SuperRight,
		Menu,
		ControlRight,
		Up,
		Down,
		Left,
		Right,
		NumLock,
		KeypadSlash,
		KeypadAsterisk,
		KeypadMinus,
		Keypad7,
		Keypad8,
		Keypad9,
		KeypadPlus,
		Keypad4,
		Keypad5,
		Keypad6,
		Keypad1,
		Keypad2,
		Keypad3,
		KeypadEnter,
		Keypad0,
		KeypadPeriod,
	};

	const int FirstMouse = (int)Key::KeypadPeriod+1;
	/// <summary>
	/// Mouse input IDs.
	/// </summary>
	enum class Mouse
	{
		ButtonLeft = FirstMouse,
		ButtonRight,
		ButtonMiddle,
		Button4,
		Button5,
		Button6,
		Button7,
		Button8,
		Position,
		Delta
	};

	/// <summary>
	/// Last inbuilt input ID.
	/// </summary>
	const int LastBinding = (int)Mouse::Delta;

	/// <summary>
	/// Convert GLFW input ID to Key.
	/// </summary>
	/// <param name="glfwInput"></param>
	/// <returns></returns>
	Key GLFWInputToKey(int glfwInput);
	/// <summary>
	/// Convert GLFW input ID to Mouse.
	/// </summary>
	/// <param name="glfwInput"></param>
	/// <returns></returns>
	Mouse GLFWInputToMouse(const int glfwInput);
}