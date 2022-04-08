#include "pch.h"
#include "CppUnitTest.h"

#include "../Engine/utility/CCXEvent.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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

		TEST_METHOD(Register_String)
		{
			CCX::Event<> e;
			e.Register("Test", []() {});
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

		TEST_METHOD(Remove_String)
		{
			CCX::Event<int&> e;
			int i = 5;
			e.Register("Test", [](int& a) { a += 5; });
			Assert::IsTrue(e.Deregister("Test"));
			e.Invoke(i);
			Assert::AreEqual(5, i);
		}
	};
}
