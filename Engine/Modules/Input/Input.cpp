#include "Input.h"

ecse::Input::Action* context;

std::unordered_map<std::string, std::unique_ptr<ecse::Input::Action>> actions;

void ecse::Input::GLFWKeyCallback(GLFWwindow* window, int glfwKey, int scanCode, int action, int mods)
{
	Key key = GLFWInputToKey(glfwKey);
	bindings[(int)key].HandleEvent(window, action, mods);
}

void ecse::Input::GLFWMouseButtonCallback(GLFWwindow* window, int glfwMouse, int action, int mods)
{
	Mouse mouse = GLFWInputToMouse(glfwMouse);
	bindings[(int)mouse].HandleEvent(window, action, mods);
}

void ecse::Input::Initialise()
{
	OnInit.Register([]() {glfwSetKeyCallback(ecse::GetWindows()[0], ecse::Input::GLFWKeyCallback); });
	UpdateMouse();
	OnInputPoll.Register(UpdateMouse);

	// Validity check
	CCXAssert(ecse::Input::bindings.size() == ecse::Input::LastBinding + 1, "Misaligned ecse::Input::bindings count. Count: " + std::to_string(ecse::Input::bindings.size()) + " Expected: " + std::to_string(ecse::Input::LastBinding + 1));
}

void ecse::Input::Reset(bool iterateBindings)
{
	actions.clear();
	context = nullptr;
	for (auto& mb : bindings)
	{
		mb.ClearInstances();
	}
}

ecse::Input::BindingInstance* ecse::Input::CreateBinding(Action* bindTo, Key key)
{
	return bindings[static_cast<int>(key)].CreateInstance(bindTo);
}

ecse::Input::BindingInstance* ecse::Input::CreateBinding(Action* bindTo, Mouse mouse)
{
	return bindings[static_cast<int>(mouse)].CreateInstance(bindTo);
}

ecse::Input::BindingInstance* ecse::Input::CreateBinding(Key key)
{
	CCXAssert(context != nullptr, "Context cannot be null when using this overload. Use SetBindContext first.");
	return bindings[static_cast<int>(key)].CreateInstance(context);
}

ecse::Input::BindingInstance* ecse::Input::CreateBinding(Mouse mouse)
{
	CCXAssert(context != nullptr, "Context cannot be null when using this overload. Use SetBindContext first.");
	return bindings[static_cast<int>(mouse)].CreateInstance(context);
}

ecse::Input::Action* ecse::Input::CreateAction(std::string& name, Output dataType, Precision precision)
{
	auto insertResult = actions.insert({ name, std::make_unique<Action>(dataType, precision) });
	if (!insertResult.second)
	{
		LogError("Action '" + name + "' already exists.", true);
	}
	return insertResult.first->second.get();
}

ecse::Input::Action* ecse::Input::CreateAction(std::string&& name, Output dataType, Precision precision)
{
	auto insertResult = actions.insert({ name, std::make_unique<Action>(dataType, precision) });
	if (!insertResult.second)
	{
		LogError("Action '" + name + "' already exists.", true);
	}
	return insertResult.first->second.get();
}

ecse::Input::Action* ecse::Input::GetAction(std::string&& name)
{
	Action* action = nullptr;
	try
	{
		action = actions.at(name).get();
	}
	catch (const std::out_of_range&)
	{
		LogError("Action '" + name + "' does not exist.", true);
	}
	return action;
}

void ecse::Input::SetBindContext(Action* _context)
{
	context = _context;
}

ecse::Input::Action* ecse::Input::Context()
{
	return context;
}