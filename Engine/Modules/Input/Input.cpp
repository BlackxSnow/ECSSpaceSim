#include "Input.h"
using namespace Thera::Input;

Thera::Input::Action* context;

std::unordered_map<std::string, std::unique_ptr<Thera::Input::Action>> actions;

void Thera::Input::GLFWKeyCallback(GLFWwindow* window, int glfwKey, int scanCode, int action, int mods)
{
	Key key = GLFWInputToKey(glfwKey);
	RawKeyInputReceived.Invoke(window, glfwKey, scanCode, action, mods);
	bindings[(int)key].HandleEvent(window, action, mods);
}

void Thera::Input::GLFWMouseButtonCallback(GLFWwindow* window, int glfwMouse, int action, int mods)
{
	Mouse mouse = GLFWInputToMouse(glfwMouse);
	RawMouseInputReceived.Invoke(window, glfwMouse, action, mods);
	bindings[(int)mouse].HandleEvent(window, action, mods);
}

void Thera::Input::Initialise()
{
	glfwSetKeyCallback(Thera::GetWindows()[0], Thera::Input::GLFWKeyCallback);
	glfwSetMouseButtonCallback(Thera::GetWindows()[0], Thera::Input::GLFWMouseButtonCallback);
	UpdateMouse();
	OnInputPoll.Register(UpdateMouse);

	// Validity check
	CCXAssert(Thera::Input::bindings.size() == Thera::Input::LastBinding + 1, "Misaligned Thera::Input::bindings count. Count: " + std::to_string(Thera::Input::bindings.size()) + " Expected: " + std::to_string(Thera::Input::LastBinding + 1));
}

void Thera::Input::Reset(bool iterateBindings)
{
	actions.clear();
	composites.clear();
	context = nullptr;
	for (auto& mb : bindings)
	{
		mb.ClearInstances();
	}
}

Thera::Input::BindingInstance* Thera::Input::CreateBinding(Action* bindTo, Key key)
{
	CCXAssert(bindTo != nullptr, "bindTo cannot be null. If creating bindings for composites, use CreateConstituent instead.");
	return bindings[static_cast<int>(key)].CreateInstance(bindTo);
}

Thera::Input::BindingInstance* Thera::Input::CreateBinding(Action* bindTo, Mouse mouse)
{
	CCXAssert(bindTo != nullptr, "bindTo cannot be null. If creating bindings for composites, use CreateConstituent instead.");
	return bindings[static_cast<int>(mouse)].CreateInstance(bindTo);
}

Thera::Input::BindingInstance* Thera::Input::CreateBinding(Key key)
{
	CCXAssert(context != nullptr, "Context cannot be null when using this overload. Use SetBindContext first.");
	return bindings[static_cast<int>(key)].CreateInstance(context);
}

Thera::Input::BindingInstance* Thera::Input::CreateBinding(Mouse mouse)
{
	CCXAssert(context != nullptr, "Context cannot be null when using this overload. Use SetBindContext first.");
	return bindings[static_cast<int>(mouse)].CreateInstance(context);
}

Constituent Thera::Input::CreateConstituent(Key key, std::initializer_list<Component> components)
{
	return Constituent(bindings[static_cast<int>(key)].CreateUnboundInstance(), components);
}

Constituent Thera::Input::CreateConstituent(Mouse mouse, std::initializer_list<Component> components)
{
	return Constituent(bindings[static_cast<int>(mouse)].CreateUnboundInstance(), components);
}

CompositeBinding* Thera::Input::CreateCompositeBinding(Action* bindTo, Output _dataType, Precision dataPrecision, std::vector<Constituent>&& constituents)
{
	composites.push_back(std::make_unique<CompositeBinding>(bindTo, _dataType, dataPrecision, std::forward<std::vector<Constituent>>(constituents)));
	bindTo->AddBinding(composites.back().get());
	return composites.back().get();
}
CompositeBinding* Thera::Input::CreateCompositeBinding(Output _dataType, Precision dataPrecision, std::vector<Constituent>&& constituents)
{
	CCXAssert(context != nullptr, "Context cannot be null when using this overload. Use SetBindContext first.");
	composites.push_back(std::make_unique<CompositeBinding>(context, _dataType, dataPrecision, std::forward<std::vector<Constituent>>(constituents)));
	context->AddBinding(composites.back().get());
	return composites.back().get();
}

Thera::Input::Action* Thera::Input::CreateAction(std::string& name, Output dataType, Precision precision)
{
	auto insertResult = actions.insert({ name, std::make_unique<Action>(dataType, precision) });
	if (!insertResult.second)
	{
		LogError("Action '" + name + "' already exists.", true);
	}
	return insertResult.first->second.get();
}

Thera::Input::Action* Thera::Input::CreateAction(std::string&& name, Output dataType, Precision precision)
{
	auto insertResult = actions.insert({ name, std::make_unique<Action>(dataType, precision) });
	if (!insertResult.second)
	{
		LogError("Action '" + name + "' already exists.", true);
	}
	return insertResult.first->second.get();
}

Thera::Input::Action* Thera::Input::GetAction(std::string&& name)
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

void Thera::Input::SetBindContext(Action* _context)
{
	context = _context;
}

Thera::Input::Action* Thera::Input::Context()
{
	return context;
}