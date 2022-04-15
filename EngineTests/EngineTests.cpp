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
			ecse::TestInit();
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			ecse::Input::Reset(true);
		}

		TEST_METHOD(Create_Key)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Key::A);
		}

		TEST_METHOD(Create_Mouse)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Mouse::ButtonLeft);
		}

		TEST_METHOD(GetData_Key)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Key::A);
			binding->master.ForcePoll();
			auto fData = binding->GetData<float>();
			Assert::AreEqual(0.0f, fData, 0.01f, L"Unexpected float value");
		}
		
		TEST_METHOD(GetData_MouseButton)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Mouse::ButtonLeft);
			binding->master.ForcePoll();
			auto fData = binding->GetData<float>();
			Assert::AreEqual(0.0f, fData, 0.01f, L"Unexpected float value");
		}

		// Note: may fail if mouse is moved too quickly during test.
		TEST_METHOD(GetData_MousePosition)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Vector2);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Mouse::Position);
			binding->master.ForcePoll();
			double expectedX, expectedY;
			glfwGetCursorPos(ecse::GetWindows()[0], &expectedX, &expectedY);
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
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			Assert::IsNotNull(action);
		}

		TEST_METHOD(Callback_KeyStart)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Key::A);
			bool eventFired = false;
			action->OnStart.Register([&eventFired](ecse::Input::InputEventData& _) { eventFired = true; });
			ecse::Input::GLFWKeyCallback(ecse::GetWindows()[0], GLFW_KEY_A, GLFW_PRESS, GLFW_PRESS, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Callback_KeyRepeat)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Key::A);
			bool eventFired = false;
			action->OnRepeat.Register([&eventFired](ecse::Input::InputEventData& _) { eventFired = true; });
			ecse::Input::GLFWKeyCallback(ecse::GetWindows()[0], GLFW_KEY_A, GLFW_REPEAT, GLFW_REPEAT, 0);
			Assert::IsTrue(eventFired);
		}
		
		TEST_METHOD(Callback_KeyEnd)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			auto binding = ecse::Input::CreateBinding(action, ecse::Input::Key::A);
			bool eventFired = false;
			action->OnEnd.Register([&eventFired](ecse::Input::InputEventData& _) { eventFired = true; });
			ecse::Input::GLFWKeyCallback(ecse::GetWindows()[0], GLFW_KEY_A, GLFW_RELEASE, GLFW_RELEASE, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Context_Set)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			ecse::Input::SetBindContext(action);
			auto binding = ecse::Input::CreateBinding(ecse::Input::Key::A);
			bool eventFired = false;
			action->OnStart.Register([&eventFired](ecse::Input::InputEventData& _) { eventFired = true; });
			ecse::Input::GLFWKeyCallback(ecse::GetWindows()[0], GLFW_KEY_A, GLFW_PRESS, GLFW_PRESS, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Context_Register)
		{
			ecse::Input::SetBindContext(ecse::Input::CreateAction("test", ecse::Input::Output::Scalar));
			auto binding = ecse::Input::CreateBinding(ecse::Input::Key::A);
			bool eventFired = false;
			ecse::Input::Context()->OnStart.Register([&eventFired](ecse::Input::InputEventData& _) { eventFired = true; });
			ecse::Input::GLFWKeyCallback(ecse::GetWindows()[0], GLFW_KEY_A, GLFW_PRESS, GLFW_PRESS, 0);
			Assert::IsTrue(eventFired);
		}

		TEST_METHOD(Composite_Create)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Vector2);
			auto composite = ecse::Input::CreateCompositeBinding(action, ecse::Input::Output::Vector2, ecse::Input::Precision::Single, 
				{
					ecse::Input::CreateConstituent(ecse::Input::Key::A, { ecse::Input::Component::NegX }),
					ecse::Input::CreateConstituent(ecse::Input::Key::D, { ecse::Input::Component::PosX }),
					ecse::Input::CreateConstituent(ecse::Input::Key::S, { ecse::Input::Component::NegY }),
					ecse::Input::CreateConstituent(ecse::Input::Key::W, { ecse::Input::Component::PosY })
				});
		}

		TEST_METHOD(Composite_Create_OverlappingComponents)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Scalar);
			Assert::ExpectException<std::runtime_error, std::function<void()>>([action]() {
				auto composite = ecse::Input::CreateCompositeBinding(action, ecse::Input::Output::Scalar, ecse::Input::Precision::Single,
				{
					ecse::Input::CreateConstituent(ecse::Input::Key::A, { ecse::Input::Component::PosX }),
					ecse::Input::CreateConstituent(ecse::Input::Key::D, { ecse::Input::Component::PosX })
				});
			}, L"Overlapping Components did not throw an exception and should.");
		}
		
		TEST_METHOD(Composite_Create_IncorrectType)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Vector2);
			Assert::ExpectException<std::runtime_error, std::function<void()>>([action]() {
				auto composite = ecse::Input::CreateCompositeBinding(action, ecse::Input::Output::Scalar, ecse::Input::Precision::Single,
				{
					ecse::Input::CreateConstituent(ecse::Input::Key::A, { ecse::Input::Component::PosX }),
					ecse::Input::CreateConstituent(ecse::Input::Key::D, { ecse::Input::Component::PosY })
				});
			}, L"Incorrect type did not throw an exception and should.");
		}
		
		TEST_METHOD(Composite_GetData)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Vector2);
			auto composite = ecse::Input::CreateCompositeBinding(action, ecse::Input::Output::Vector2, ecse::Input::Precision::Single,
			{
				ecse::Input::CreateConstituent(ecse::Input::Key::A, { ecse::Input::Component::NegX }),
				ecse::Input::CreateConstituent(ecse::Input::Key::D, { ecse::Input::Component::PosX }),
				ecse::Input::CreateConstituent(ecse::Input::Key::S, { ecse::Input::Component::NegY }),
				ecse::Input::CreateConstituent(ecse::Input::Key::W, { ecse::Input::Component::PosY })
			});
			ecse::Input::GLFWKeyCallback(nullptr, GLFW_KEY_A, 0, GLFW_PRESS, 0);
			ecse::Input::GLFWKeyCallback(nullptr, GLFW_KEY_W, 0, GLFW_PRESS, 0);
			glm::vec2 data = composite->GetData<glm::vec2>();
			Assert::AreEqual(-1.0f, data.x);
			Assert::AreEqual(1.0f, data.y);
		}

		TEST_METHOD(Composite_GetActionData)
		{
			auto action = ecse::Input::CreateAction("test", ecse::Input::Output::Vector2);
			auto composite = ecse::Input::CreateCompositeBinding(action, ecse::Input::Output::Vector2, ecse::Input::Precision::Single,
				{
					ecse::Input::CreateConstituent(ecse::Input::Key::A, { ecse::Input::Component::NegX }),
					ecse::Input::CreateConstituent(ecse::Input::Key::D, { ecse::Input::Component::PosX }),
					ecse::Input::CreateConstituent(ecse::Input::Key::S, { ecse::Input::Component::NegY }),
					ecse::Input::CreateConstituent(ecse::Input::Key::W, { ecse::Input::Component::PosY })
				});
			ecse::Input::GLFWKeyCallback(nullptr, GLFW_KEY_A, 0, GLFW_PRESS, 0);
			ecse::Input::GLFWKeyCallback(nullptr, GLFW_KEY_W, 0, GLFW_PRESS, 0);
			glm::vec2 data = action->GetData<glm::vec2>();
			Assert::AreEqual(-1.0f, data.x);
			Assert::AreEqual(1.0f, data.y);
		}
	};
}
