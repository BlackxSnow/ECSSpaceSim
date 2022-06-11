#pragma once

#include <flecs.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <tuple>

#include "../../Engine.h"
#include "../../utility/CCXDefs.h"
#include "../../utility/CCXEvent.h"
#include "../../utility/ConsoleLogging.h"
#include "InputDefs.h"
#include "Bindings.h"

// Improvement Notes:
// - I can implement binding processors via a base class that defines `T Process(T data)` for each output type.
//		The base implementation could directly return the input value, and useful processors could be implemented as inheritors by overriding their relevant overload(s).
// - I could store and send information about the binding which would help pre-processors be more generic and modular.
//		This data could include the expected bounds of the input device for example.
// - Possibly have event data contain a stack allocated float[6] (representative of the maximum data size) to improve data processing.
// - Implement `InputDevice GetDeviceType(int bindingID)` utility to convert binding indices to more legible device types.
// - Different action maps
//		- Priorities and overrides?
// - Gamepad bindings

namespace Thera
{
	namespace Input
	{
		/// <summary>
		/// Current mouse position relative to the window's bottom left corner.
		/// </summary>
		inline glm::dvec2 mousePosition;
		/// <summary>
		/// Distance between last mousePosition and current mousePosition.
		/// </summary>
		inline glm::dvec2 mouseDelta;

		/// <summary>
		/// Updates mousePosition and mouseDelta.
		/// </summary>
		static void UpdateMouse()
		{
			double x, y;
			glfwGetCursorPos(GetWindows()[0], &x, &y);
			mouseDelta = glm::dvec2(x - mousePosition.x, y - mousePosition.y);
			mousePosition = glm::dvec2(x, y);
		}

		/// <summary>
		/// Key callback for registration with GLFW.
		/// </summary>
		/// <param name="window"></param>
		/// <param name="key"></param>
		/// <param name="scanCode"></param>
		/// <param name="action"></param>
		/// <param name="mods"></param>
		void GLFWKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
		/// <summary>
		/// Mouse button callback for registration with GLFW.
		/// </summary>
		/// <param name="window"></param>
		/// <param name="button"></param>
		/// <param name="action"></param>
		/// <param name="mods"></param>
		void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		/// <summary>
		/// Invoked with raw GLFW input data on key input event.<para/>
		/// 
		/// <c>GLFWWindow*</c>: The window that received the input event.<para/>
		/// <c>int</c>: The button that was pressed or released.<para/>
		/// <c>int</c>: The action that was performed.<para/>
		/// <c>int</c>: The modifier keys that were pressed.<para/>
		/// </summary>
		inline CCX::Event<GLFWwindow*, int, int, int, int> RawKeyInputReceived;
		/// <summary>
		/// Invoked with raw GLFW input data on mouse button input event.<para/>
		/// 
		/// GLFWWindow*: The window that received the input event.<para/>
		/// int: The button that was pressed or released.<para/>
		/// int: The action that was performed.<para/>
		/// int: The modifier keys that were pressed.<para/>
		/// </summary>
		inline CCX::Event<GLFWwindow*, int, int, int> RawMouseInputReceived;

		/// <summary>
		/// Initialise the input system and register the GLFW callbacks.
		/// </summary>
		void Initialise();
		/// <summary>
		/// Reset the input system to a freshly initialised state.
		/// </summary>
		/// <param name="iterateBindings"></param>
		void Reset(bool iterateBindings = false);

		/// <summary>
		/// Create a new BindingInstance bound to the specified action.
		/// </summary>
		/// <param name="bindTo"></param>
		/// <param name="key"></param>
		/// <returns></returns>
		BindingInstance* CreateBinding(Action* bindTo, Key key);
		BindingInstance* CreateBinding(Action* bindTo, Mouse mouse);
		BindingInstance* CreateBinding(Key key);
		BindingInstance* CreateBinding(Mouse mouse);

		/// <summary>
		/// Create a new constituent binding for use in a CompositeBinding.
		/// </summary>
		/// <param name="key"></param>
		/// <param name="components">How the constituent contributes to its CompositeBinding. Component order is respective to the data's primitive index (eg. Vec2 is [float, float])</param>
		/// <returns></returns>
		Constituent CreateConstituent(Key key, std::initializer_list<Component> components);
		Constituent CreateConstituent(Mouse mouse, std::initializer_list<Component> components);
		Constituent CreateConstituent(Key mouse, std::initializer_list<int> components);
		Constituent CreateConstituent(Mouse mouse, std::initializer_list<int> components);
		/// <summary>
		/// Create a composite binding bound to the specified action and comprised of the specified constituents.
		/// </summary>
		/// <param name="bindTo"></param>
		/// <param name="_dataType"></param>
		/// <param name="dataPrecision"></param>
		/// <param name="constituents"></param>
		/// <returns></returns>
		CompositeBinding* CreateCompositeBinding(Action* bindTo, Output _dataType, Precision dataPrecision, std::vector<Constituent>&& constituents);
		CompositeBinding* CreateCompositeBinding(Output _dataType, Precision dataPrecision, std::vector<Constituent>&& constituents);

		/// <summary>
		/// Create a new empty input action.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="dataType"></param>
		/// <param name="precision"></param>
		/// <returns></returns>
		Action* CreateAction(std::string& name, Output dataType, Precision precision = Precision::Double);
		Action* CreateAction(std::string&& name, Output dataType, Precision precision = Precision::Double);

		/// <summary>
		/// Retrieve an existing action.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		Action* GetAction(std::string&& name);

		/// <summary>
		/// Set the action bind context. Allows calls to create bindings without specifying target action.
		/// </summary>
		/// <param name="context"></param>
		void SetBindContext(Action* context);

		/// <summary>
		/// Retrieve the current bind context.
		/// </summary>
		/// <returns></returns>
		Action* Context();
	};
}