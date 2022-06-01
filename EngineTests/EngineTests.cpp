#include "pch.h"
#include "CppUnitTest.h"

#include "../Engine/utility/CCXEvent.h"
#include "../Engine/Modules/Input/Input.h"
#include "../Engine/Engine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Utility
{
	inline const std::wstring Concat(wchar_t* str, double val)
	{
		return std::wstring(str) + std::to_wstring(val);
	}
}

namespace EngineTests
{
	TEST_CLASS(Event)
	{
	public:
		
		TEST_METHOD(Register)
		{
			CCX::Event<> e;
			e.Register([]() {});
		}

		TEST_METHOD(Invoke)
		{
			CCX::Event<int&> e;
			int i = 5;
			e.Register([](int& a) { a += 5; });
			e.Invoke(i);
			Assert::AreEqual(10, i);
		}

		TEST_METHOD(Remove)
		{
			CCX::Event<int&> e;
			int i = 5;
			CCX::Event<int&>::Handle h = e.Register([](int& a) { a += 5; });
			Assert::IsTrue(e.Deregister(h));
			e.Invoke(i);
			Assert::AreEqual(5, i);
		}
	};

	TEST_CLASS(Input)
	{
	public:
		TEST_CLASS_INITIALIZE(Init)
		{
			Thera::TestInit();
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			Thera::Input::Reset(true);
		}

		TEST_METHOD(Create_Key)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Key::A);
		}

		TEST_METHOD(Create_Mouse)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Mouse::ButtonLeft);
		}

		TEST_METHOD(GetData_Key)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Key::A);
			binding->master.ForcePoll();
			auto fData = binding->GetData<float>();
			Assert::AreEqual(0.0f, fData, 0.01f, L"Unexpected float value");
		}
		
		TEST_METHOD(GetData_MouseButton)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Mouse::ButtonLeft);
			binding->master.ForcePoll();
			auto fData = binding->GetData<float>();
			Assert::AreEqual(0.0f, fData, 0.01f, L"Unexpected float value");
		}

		// Note: may fail if mouse is moved too quickly during test.
		TEST_METHOD(GetData_MousePosition)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Vector2);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Mouse::Position);
			binding->master.ForcePoll();
			double expectedX, expectedY;
			glfwGetCursorPos(Thera::GetWindows()[0], &expectedX, &expectedY);
			auto vData = binding->GetData<glm::dvec2>();
			Assert::IsTrue(vData.x > -25000, Utility::Concat(L"x value below -25000, actual: ", vData.x).c_str());
			Assert::IsTrue(vData.x < 25000, Utility::Concat(L"x value above 25000, actual: ", vData.x).c_str());
			Assert::IsTrue(vData.y > -25000, Utility::Concat(L"y Value below -25000, actual: ", vData.y).c_str());
			Assert::IsTrue(vData.y < 25000, Utility::Concat(L"y value above 25000, actual: ", vData.y).c_str());
			Assert::AreEqual(expectedX, vData.x);
			Assert::AreEqual(expectedY, vData.y);
		}

		TEST_METHOD(Action_Create)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			Assert::IsNotNull(action);
		}

		TEST_METHOD(Callback_KeyStart)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Key::A);
			bool eventFired = false;
			action->OnStart.Register([&eventFired](Thera::Input::InputEventData& _) { eventFired = true; });
			Thera::Input::GLFWKeyCallback(Thera::GetWindows()[0], GLFW_KEY_A, GLFW_PRESS, GLFW_PRESS, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Callback_KeyRepeat)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Key::A);
			bool eventFired = false;
			action->OnRepeat.Register([&eventFired](Thera::Input::InputEventData& _) { eventFired = true; });
			Thera::Input::GLFWKeyCallback(Thera::GetWindows()[0], GLFW_KEY_A, GLFW_REPEAT, GLFW_REPEAT, 0);
			Assert::IsTrue(eventFired);
		}
		
		TEST_METHOD(Callback_KeyEnd)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			auto binding = Thera::Input::CreateBinding(action, Thera::Input::Key::A);
			bool eventFired = false;
			action->OnEnd.Register([&eventFired](Thera::Input::InputEventData& _) { eventFired = true; });
			Thera::Input::GLFWKeyCallback(Thera::GetWindows()[0], GLFW_KEY_A, GLFW_RELEASE, GLFW_RELEASE, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Context_Set)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			Thera::Input::SetBindContext(action);
			auto binding = Thera::Input::CreateBinding(Thera::Input::Key::A);
			bool eventFired = false;
			action->OnStart.Register([&eventFired](Thera::Input::InputEventData& _) { eventFired = true; });
			Thera::Input::GLFWKeyCallback(Thera::GetWindows()[0], GLFW_KEY_A, GLFW_PRESS, GLFW_PRESS, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Context_Register)
		{
			Thera::Input::SetBindContext(Thera::Input::CreateAction("test", Thera::Input::Output::Scalar));
			auto binding = Thera::Input::CreateBinding(Thera::Input::Key::A);
			bool eventFired = false;
			Thera::Input::Context()->OnStart.Register([&eventFired](Thera::Input::InputEventData& _) { eventFired = true; });
			Thera::Input::GLFWKeyCallback(Thera::GetWindows()[0], GLFW_KEY_A, GLFW_PRESS, GLFW_PRESS, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Composite_Create)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Vector2);
			auto composite = Thera::Input::CreateCompositeBinding(action, Thera::Input::Output::Vector2, Thera::Input::Precision::Single, 
				{
					Thera::Input::CreateConstituent(Thera::Input::Key::A, { Thera::Input::Component::NegX }),
					Thera::Input::CreateConstituent(Thera::Input::Key::D, { Thera::Input::Component::PosX }),
					Thera::Input::CreateConstituent(Thera::Input::Key::S, { Thera::Input::Component::NegY }),
					Thera::Input::CreateConstituent(Thera::Input::Key::W, { Thera::Input::Component::PosY })
				});
		}

		TEST_METHOD(Composite_Create_OverlappingComponents)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Scalar);
			Assert::ExpectException<std::runtime_error, std::function<void()>>([action]() {
				auto composite = Thera::Input::CreateCompositeBinding(action, Thera::Input::Output::Scalar, Thera::Input::Precision::Single,
				{
					Thera::Input::CreateConstituent(Thera::Input::Key::A, { Thera::Input::Component::PosX }),
					Thera::Input::CreateConstituent(Thera::Input::Key::D, { Thera::Input::Component::PosX })
				});
			}, L"Overlapping Components did not throw an exception and should.");
		}
		
		TEST_METHOD(Composite_Create_IncorrectType)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Vector2);
			Assert::ExpectException<std::runtime_error, std::function<void()>>([action]() {
				auto composite = Thera::Input::CreateCompositeBinding(action, Thera::Input::Output::Scalar, Thera::Input::Precision::Single,
				{
					Thera::Input::CreateConstituent(Thera::Input::Key::A, { Thera::Input::Component::PosX }),
					Thera::Input::CreateConstituent(Thera::Input::Key::D, { Thera::Input::Component::PosY })
				});
			}, L"Incorrect type did not throw an exception and should.");
		}
		
		TEST_METHOD(Composite_GetData)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Vector2);
			auto composite = Thera::Input::CreateCompositeBinding(action, Thera::Input::Output::Vector2, Thera::Input::Precision::Single,
			{
				Thera::Input::CreateConstituent(Thera::Input::Key::A, { Thera::Input::Component::NegX }),
				Thera::Input::CreateConstituent(Thera::Input::Key::D, { Thera::Input::Component::PosX }),
				Thera::Input::CreateConstituent(Thera::Input::Key::S, { Thera::Input::Component::NegY }),
				Thera::Input::CreateConstituent(Thera::Input::Key::W, { Thera::Input::Component::PosY })
			});
			Thera::Input::GLFWKeyCallback(nullptr, GLFW_KEY_A, 0, GLFW_PRESS, 0);
			Thera::Input::GLFWKeyCallback(nullptr, GLFW_KEY_W, 0, GLFW_PRESS, 0);
			glm::vec2 data = composite->GetData<glm::vec2>();
			Assert::AreEqual(-1.0f, data.x);
			Assert::AreEqual(1.0f, data.y);
		}

		TEST_METHOD(Composite_GetActionData)
		{
			auto action = Thera::Input::CreateAction("test", Thera::Input::Output::Vector2);
			auto composite = Thera::Input::CreateCompositeBinding(action, Thera::Input::Output::Vector2, Thera::Input::Precision::Single,
				{
					Thera::Input::CreateConstituent(Thera::Input::Key::A, { Thera::Input::Component::NegX }),
					Thera::Input::CreateConstituent(Thera::Input::Key::D, { Thera::Input::Component::PosX }),
					Thera::Input::CreateConstituent(Thera::Input::Key::S, { Thera::Input::Component::NegY }),
					Thera::Input::CreateConstituent(Thera::Input::Key::W, { Thera::Input::Component::PosY })
				});
			Thera::Input::GLFWKeyCallback(nullptr, GLFW_KEY_A, 0, GLFW_PRESS, 0);
			Thera::Input::GLFWKeyCallback(nullptr, GLFW_KEY_W, 0, GLFW_PRESS, 0);
			glm::vec2 data = action->GetData<glm::vec2>();
			Assert::AreEqual(-1.0f, data.x);
			Assert::AreEqual(1.0f, data.y);
		}
	};
}
