#include <modules/input/Bindings.h>
#include <modules/input/Input.h>
#include <Engine.h>

void Thera::Input::MasterBinding::HandleEvent(GLFWwindow* window, int action, int mods)
{
	/*ForcePoll();*/
	
	CCXAssert(dataType == Output::Scalar, "Non-scalar bindings should not recieve dataless events.");

	if (action == GLFW_RELEASE)
	{
		memset(data, 0, (int)dataType * (int)dataPrecision);
	}
	else
	{
		if (dataPrecision == Precision::Single)
		{
			*static_cast<float*>(data) = 1.0f;
		}
		else if (dataPrecision == Precision::Double)
		{
			*static_cast<double*>(data) = 1.0;
		}
	}
	lastUpdate = Thera::FrameCount();

	for (auto& instance : instances)
	{
		instance->HandleEvent(window, action, mods);
	}
}

void Thera::Input::MasterBinding::TryPoll()
{
	size_t frameCount = Thera::FrameCount();
	if (lastUpdate < frameCount)
	{
		poller(data, dataType, dataPrecision);
		lastUpdate = frameCount;
	}
}

void Thera::Input::MasterBinding::ForcePoll()
{
	poller(data, dataType, dataPrecision);
	lastUpdate = Thera::FrameCount();
}

template<class O, class C>
void ReadConstituent(void* outData, Thera::Input::Constituent& constituent)
{
	O* castData = static_cast<O*>(outData);
	const C* bindingData = static_cast<const C*>(constituent.binding->GetData<const void*>());

	for (int i = 0; i < constituent.indices.size(); i++)
	{
		int index = floor(constituent.indices[i] / 2);
		castData[index] += bindingData[i] * (constituent.indices[i] % 2 ? 1 : -1);
	}
}

void Thera::Input::CompositeBinding::PollConstituents()
{
	memset(data, 0, (int)dataType * (int)dataPrecision);
	for (auto& constituent : constituents)
	{
		Precision bindPrecision = constituent.binding->master.dataPrecision;
		// This could probably be cleaned up
		if (dataPrecision == Precision::Single)
		{
			if (bindPrecision == Precision::Single)
			{
				ReadConstituent<float, float>(data, constituent);
			}
			else if (bindPrecision == Precision::Double)
			{
				ReadConstituent<float, double>(data, constituent);
			}
		}
		else if (dataPrecision == Precision::Double)
		{
			if (bindPrecision == Precision::Single)
			{
				ReadConstituent<double, float>(data, constituent);
			}
			else if (bindPrecision == Precision::Double)
			{
				ReadConstituent<double, double>(data, constituent);
			}
		}
	}
}

void Thera::Input::CompositeBinding::TryPoll()
{
	size_t frameCount = Thera::FrameCount();
	if (lastUpdate < frameCount)
	{
		PollConstituents();
		lastUpdate = frameCount;
	}
}

void Thera::Input::CompositeBinding::ForcePoll()
{
	PollConstituents();
	lastUpdate = Thera::FrameCount();
}

void Thera::Input::CompositeBinding::ValidateConstituents()
{
	byte filledComponents = 0;
	for (auto& constituent : constituents)
	{
		for (int& component : constituent.indices)
		{
			CCXAssert(!(filledComponents & (1 << component)), "Constituent component index '" + std::to_string(component)  + "' is already used");
			filledComponents |= (1 << component);
		}
	}
}

void Thera::Input::MasterBinding::RemoveInstance(BindingInstance* remove)
{
	instances.erase(std::find_if(instances.begin(), instances.end(), [remove](std::shared_ptr<BindingInstance> inst) { return remove == inst.get(); }));
}

void Thera::Input::MasterBinding::ClearInstances()
{
	instances.clear();
}

void Thera::Input::BindingInstance::HandleEvent(GLFWwindow* window, int action, int mods)
{
	if (boundAction == nullptr)
	{
		return;
	}
	// TODO: Preprocessing here

	InputEventData eventData = InputEventData(*this, action, mods);
	boundAction->HandleEvent(eventData);
}

void Thera::Input::Action::HandleEvent(InputEventData& eventData)
{
	// TODO: Apply processors

	switch (eventData.action)
	{
	case GLFW_PRESS:
		OnStart.Invoke(eventData);
		break;
	case GLFW_REPEAT:
		OnRepeat.Invoke(eventData);
		break;
	case GLFW_RELEASE:
		OnEnd.Invoke(eventData);
		break;
	default:
		LogError("Unknown GLFW action type", true);
		break;
	}
}

void Thera::Input::Action::AddBinding(BindingInstance* binding)
{
	CCXAssert(dataType == binding->master.dataType, "Data type mismatch. Expected: " + std::to_string(static_cast<int>(dataType)) + " Got: " + std::to_string(static_cast<int>(binding->master.dataType)));
	bindings.push_back(binding);
}

void Thera::Input::Action::AddBinding(CompositeBinding* composite)
{
	CCXAssert(dataType == composite->dataType, "Data type mismatch. Expected: " + std::to_string(static_cast<int>(dataType)) + " Got: " + std::to_string(static_cast<int>(composite->dataType)));
	composites.push_back(composite);
}

Thera::Input::BindingInstance* Thera::Input::MasterBinding::CreateInstance(Action* bindTo)
{
	CCXAssert(bindTo->dataType == dataType, "Action type '" + outputToString[(int)bindTo->dataType] + "' does not match binding type '" + outputToString[(int)dataType] + "'.");
	//CCXAssert((int)bindTo->dataPrecision <= (int)dataPrecision, "Precision of action must be less than or equal to the binding precision.");

	instances.push_back(std::make_unique<BindingInstance>(*this, bindTo));
	BindingInstance* inst = instances.back().get();
	if (bindTo != nullptr)
	{
		bindTo->AddBinding(inst);
	}
	return inst;
}

Thera::Input::BindingInstance* Thera::Input::MasterBinding::CreateUnboundInstance()
{
	instances.push_back(std::make_unique<BindingInstance>(*this));
	return instances.back().get();
}



using MB = Thera::Input::MasterBinding;
#define V2D Thera::Input::Output::Vector2, Thera::Input::Precision::Double
#define INPUTS void* data, Thera::Input::Output type, Thera::Input::Precision precision
#define KEYPARAMS(glfw_code) Thera::Input::Output::Scalar, Thera::Input::Precision::Single, [](void* data, Thera::Input::Output type, Thera::Input::Precision precision) { *static_cast<float*>(data) = glfwGetKey(Thera::GetWindows()[0], glfw_code); }
#define MOUSEBUTTONPARAMS(glfw_code) Thera::Input::Output::Scalar, Thera::Input::Precision::Single, [](void* data, Thera::Input::Output type, Thera::Input::Precision precision) { *static_cast<float*>(data) = glfwGetMouseButton(Thera::GetWindows()[0], glfw_code); }

std::vector<Thera::Input::MasterBinding> Thera::Input::bindings
{
	// Keys
	MB(KEYPARAMS(GLFW_KEY_ESCAPE)),
	MB(KEYPARAMS(GLFW_KEY_F1)),
	MB(KEYPARAMS(GLFW_KEY_F2)),
	MB(KEYPARAMS(GLFW_KEY_F3)),
	MB(KEYPARAMS(GLFW_KEY_F4)),
	MB(KEYPARAMS(GLFW_KEY_F5)),
	MB(KEYPARAMS(GLFW_KEY_F6)),
	MB(KEYPARAMS(GLFW_KEY_F7)),
	MB(KEYPARAMS(GLFW_KEY_F8)),
	MB(KEYPARAMS(GLFW_KEY_F9)),
	MB(KEYPARAMS(GLFW_KEY_F10)),
	MB(KEYPARAMS(GLFW_KEY_F11)),
	MB(KEYPARAMS(GLFW_KEY_F12)),
	MB(KEYPARAMS(GLFW_KEY_F13)),
	MB(KEYPARAMS(GLFW_KEY_F14)),
	MB(KEYPARAMS(GLFW_KEY_F15)),
	MB(KEYPARAMS(GLFW_KEY_F16)),
	MB(KEYPARAMS(GLFW_KEY_F17)),
	MB(KEYPARAMS(GLFW_KEY_F18)),
	MB(KEYPARAMS(GLFW_KEY_F19)),
	MB(KEYPARAMS(GLFW_KEY_F20)),
	MB(KEYPARAMS(GLFW_KEY_F21)),
	MB(KEYPARAMS(GLFW_KEY_F22)),
	MB(KEYPARAMS(GLFW_KEY_F23)),
	MB(KEYPARAMS(GLFW_KEY_F24)),
	MB(KEYPARAMS(GLFW_KEY_F25)),
	MB(KEYPARAMS(GLFW_KEY_PRINT_SCREEN)),
	MB(KEYPARAMS(GLFW_KEY_SCROLL_LOCK)),
	MB(KEYPARAMS(GLFW_KEY_PAUSE)),
	MB(KEYPARAMS(GLFW_KEY_GRAVE_ACCENT)),
	MB(KEYPARAMS(GLFW_KEY_1)),
	MB(KEYPARAMS(GLFW_KEY_2)),
	MB(KEYPARAMS(GLFW_KEY_3)),
	MB(KEYPARAMS(GLFW_KEY_4)),
	MB(KEYPARAMS(GLFW_KEY_5)),
	MB(KEYPARAMS(GLFW_KEY_6)),
	MB(KEYPARAMS(GLFW_KEY_7)),
	MB(KEYPARAMS(GLFW_KEY_8)),
	MB(KEYPARAMS(GLFW_KEY_9)),
	MB(KEYPARAMS(GLFW_KEY_0)),
	MB(KEYPARAMS(GLFW_KEY_MINUS)),
	MB(KEYPARAMS(GLFW_KEY_EQUAL)),
	MB(KEYPARAMS(GLFW_KEY_BACKSPACE)),
	MB(KEYPARAMS(GLFW_KEY_INSERT)),
	MB(KEYPARAMS(GLFW_KEY_HOME)),
	MB(KEYPARAMS(GLFW_KEY_PAGE_UP)),
	MB(KEYPARAMS(GLFW_KEY_TAB)),
	MB(KEYPARAMS(GLFW_KEY_Q)),
	MB(KEYPARAMS(GLFW_KEY_W)),
	MB(KEYPARAMS(GLFW_KEY_E)),
	MB(KEYPARAMS(GLFW_KEY_R)),
	MB(KEYPARAMS(GLFW_KEY_T)),
	MB(KEYPARAMS(GLFW_KEY_Y)),
	MB(KEYPARAMS(GLFW_KEY_U)),
	MB(KEYPARAMS(GLFW_KEY_I)),
	MB(KEYPARAMS(GLFW_KEY_O)),
	MB(KEYPARAMS(GLFW_KEY_P)),
	MB(KEYPARAMS(GLFW_KEY_LEFT_BRACKET)),
	MB(KEYPARAMS(GLFW_KEY_RIGHT_BRACKET)),
	MB(KEYPARAMS(GLFW_KEY_BACKSLASH)),
	MB(KEYPARAMS(GLFW_KEY_DELETE)),
	MB(KEYPARAMS(GLFW_KEY_END)),
	MB(KEYPARAMS(GLFW_KEY_PAGE_DOWN)),
	MB(KEYPARAMS(GLFW_KEY_CAPS_LOCK)),
	MB(KEYPARAMS(GLFW_KEY_A)),
	MB(KEYPARAMS(GLFW_KEY_S)),
	MB(KEYPARAMS(GLFW_KEY_D)),
	MB(KEYPARAMS(GLFW_KEY_F)),
	MB(KEYPARAMS(GLFW_KEY_G)),
	MB(KEYPARAMS(GLFW_KEY_H)),
	MB(KEYPARAMS(GLFW_KEY_J)),
	MB(KEYPARAMS(GLFW_KEY_K)),
	MB(KEYPARAMS(GLFW_KEY_L)),
	MB(KEYPARAMS(GLFW_KEY_SEMICOLON)),
	MB(KEYPARAMS(GLFW_KEY_APOSTROPHE)),
	MB(KEYPARAMS(GLFW_KEY_ENTER)),
	MB(KEYPARAMS(GLFW_KEY_LEFT_SHIFT)),
	MB(KEYPARAMS(GLFW_KEY_Z)),
	MB(KEYPARAMS(GLFW_KEY_X)),
	MB(KEYPARAMS(GLFW_KEY_C)),
	MB(KEYPARAMS(GLFW_KEY_V)),
	MB(KEYPARAMS(GLFW_KEY_B)),
	MB(KEYPARAMS(GLFW_KEY_N)),
	MB(KEYPARAMS(GLFW_KEY_M)),
	MB(KEYPARAMS(GLFW_KEY_COMMA)),
	MB(KEYPARAMS(GLFW_KEY_PERIOD)),
	MB(KEYPARAMS(GLFW_KEY_SLASH)),
	MB(KEYPARAMS(GLFW_KEY_RIGHT_SHIFT)),
	MB(KEYPARAMS(GLFW_KEY_LEFT_CONTROL)),
	MB(KEYPARAMS(GLFW_KEY_LEFT_SUPER)),
	MB(KEYPARAMS(GLFW_KEY_LEFT_ALT)),
	MB(KEYPARAMS(GLFW_KEY_SPACE)),
	MB(KEYPARAMS(GLFW_KEY_RIGHT_ALT)),
	MB(KEYPARAMS(GLFW_KEY_RIGHT_SUPER)),
	MB(KEYPARAMS(GLFW_KEY_MENU)),
	MB(KEYPARAMS(GLFW_KEY_RIGHT_CONTROL)),
	MB(KEYPARAMS(GLFW_KEY_UP)),
	MB(KEYPARAMS(GLFW_KEY_DOWN)),
	MB(KEYPARAMS(GLFW_KEY_LEFT)),
	MB(KEYPARAMS(GLFW_KEY_RIGHT)),
		
	// Numpad
	MB(KEYPARAMS(GLFW_KEY_NUM_LOCK)),
	MB(KEYPARAMS(GLFW_KEY_KP_DIVIDE)),
	MB(KEYPARAMS(GLFW_KEY_KP_MULTIPLY)),
	MB(KEYPARAMS(GLFW_KEY_KP_SUBTRACT)),
	MB(KEYPARAMS(GLFW_KEY_KP_7)),
	MB(KEYPARAMS(GLFW_KEY_KP_8)),
	MB(KEYPARAMS(GLFW_KEY_KP_9)),
	MB(KEYPARAMS(GLFW_KEY_KP_ADD)),
	MB(KEYPARAMS(GLFW_KEY_KP_4)),
	MB(KEYPARAMS(GLFW_KEY_KP_5)),
	MB(KEYPARAMS(GLFW_KEY_KP_6)),
	MB(KEYPARAMS(GLFW_KEY_KP_1)),
	MB(KEYPARAMS(GLFW_KEY_KP_2)),
	MB(KEYPARAMS(GLFW_KEY_KP_3)),
	MB(KEYPARAMS(GLFW_KEY_KP_ENTER)),
	MB(KEYPARAMS(GLFW_KEY_KP_0)),
	MB(KEYPARAMS(GLFW_KEY_KP_DECIMAL)),
	
	// Mouse
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_1)),
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_2)),
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_3)),
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_4)),
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_5)),
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_6)),
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_7)),
	MB(MOUSEBUTTONPARAMS(GLFW_MOUSE_BUTTON_8)),
	
	MB(V2D, [](INPUTS) { *static_cast<glm::dvec2*>(data) = Thera::Input::mousePosition; }),
	MB(V2D, [](INPUTS) { *static_cast<glm::dvec2*>(data) = Thera::Input::mouseDelta; })
	
};

