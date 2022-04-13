#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "InputDefs.h"
#include "../../utility/ConsoleLogging.h"
#include "../../utility/CCXEvent.h"
#include "../../utility/CCXMath.h"

namespace ecse::Input
{
	class Action;
	class BindingInstance;

	class MasterBinding
	{
	public:
		const Output dataType;
		const Precision dataPrecision;

		void HandleEvent(GLFWwindow* window, int action, int mods);
		void Poll();
		void ForcePoll();
		void RemoveInstance(BindingInstance* remove);
		void ClearInstances();

		BindingInstance* CreateInstance(Action* bindTo);

	private:
		void* data;
		size_t lastUpdate = 0;
		std::vector<std::shared_ptr<BindingInstance>> instances;
		std::function<void(void*, Output, Precision)> poller;
		
	public:
		template<class T>
		T GetData(bool truncate = false) { static_assert(false, "Unhandled output type"); }
		template<>
		float GetData<float>(bool truncate)
		{
			CCXAssert(dataType == Output::Scalar, "float output only supported by scalar bindings.");
			if (!truncate && dataPrecision == Precision::Double) { LogWarning("Undeclared truncation of double to float."); }
			Poll();
			return *static_cast<float*>(data);
		}
		template<>
		double GetData<double>(bool truncate)
		{
			CCXAssert(dataType == Output::Scalar, "double output only supported by scalar bindings.");
			CCXAssert(dataPrecision == Precision::Double, "Cannot convert float to double");
			Poll();
			return *static_cast<double*>(data);
		}
		template<>
		glm::vec2 GetData<glm::vec2>(bool truncate)
		{
			bool isTruncating = dataPrecision == Precision::Double;
			CCXAssert(dataType == Output::Vector2, "vec2 output only supported by Vector2 bindings.");
			if (!truncate && isTruncating) { LogWarning("Undeclared truncation of double to float."); }
			Poll();
			float* _data = static_cast<float*>(data);
			int offsetMultiplier = isTruncating ? 2 : 1;
			return glm::vec2(_data[0], _data[1 * offsetMultiplier]);
		}
		template<>
		glm::dvec2 GetData<glm::dvec2>(bool truncate)
		{
			CCXAssert(dataType == Output::Vector2, "dvec2 output only supported by Vector2 bindings.");
			CCXAssert(dataPrecision == Precision::Double, "Cannot convert float to double");
			Poll();
			double* _data = static_cast<double*>(data);
			return glm::dvec2(_data[0], _data[1]);
		}
		template<>
		glm::vec3 GetData<glm::vec3>(bool truncate)
		{
			bool isTruncating = dataPrecision == Precision::Double;
			CCXAssert(dataType == Output::Vector3, "vec3 output only supported by Vector3 bindings.");
			if (!truncate && isTruncating) { LogWarning("Undeclared truncation of double to float."); }
			Poll();
			float* _data = static_cast<float*>(data);
			int offsetMultiplier = isTruncating ? 2 : 1;
			return glm::vec3(_data[0], _data[1 * offsetMultiplier], _data[2 * offsetMultiplier]);
		}
		template<>
		glm::dvec3 GetData<glm::dvec3>(bool truncate)
		{
			CCXAssert(dataType == Output::Vector2, "dvec3 output only supported by Vector3 bindings.");
			CCXAssert(dataPrecision == Precision::Double, "Cannot convert float to double");
			Poll();
			double* _data = static_cast<double*>(data);
			return glm::dvec3(_data[0], _data[1], _data[2]);
		}

		MasterBinding(Output _output, Precision _precision, std::function<void(void*, Output, Precision)> _poller)
			: dataType(_output), dataPrecision(_precision), poller(_poller)
		{
			data = new byte[(int)dataType * (int)dataPrecision];
			memset(data, 0, (int)dataType * (int)dataPrecision);
		}
	};
	
	class BindingInstance
	{
	public:
		MasterBinding& master;
		Action* boundAction;

		void HandleEvent(GLFWwindow* window, int action, int mods);

		template<class T>
		T GetData(bool truncate = false)
		{
			return master.GetData<T>(truncate);
		}

		BindingInstance(MasterBinding& master, Action* boundAction) : master(master), boundAction(boundAction) {}
	};

	class InputEventData
	{
	private:
		BindingInstance& source;
		
	public:
		inline Output DataType() { return source.master.dataType; }
		inline Precision DataPrecision() { return source.master.dataPrecision; }

		const int action;
		const int mods;

		template<class T>
		T GetData(bool truncate = false)
		{
			return master.GetData<T>();
		}

	public:
		InputEventData(BindingInstance& source, int action, int mods) : source(source), action(action), mods(mods) {}
	};
	


	class Action
	{
	private:
		std::vector<BindingInstance*> bindings;
	public:
		const Output dataType;
		const Precision dataPrecision;

		CCX::Event<InputEventData&> OnStart;
		CCX::Event<InputEventData&> OnRepeat;
		CCX::Event<InputEventData&> OnEnd;

		void HandleEvent(InputEventData& eventData);

		Action(Output _dataType, Precision _dataPrecision) : dataType(_dataType), dataPrecision(_dataPrecision) {}

		void AddBinding(BindingInstance* binding);

		~Action()
		{
			for (auto& bind : bindings)
			{
				bind->master.RemoveInstance(bind);
			}
		}

	private:
		template<class T>
		inline T GetLargestBinding(bool truncate = false)
		{
			T max = bindings[0]->GetData<T>(truncate);
			for (auto& bind : bindings)
			{
				max = CCX::Max(max, bind->GetData<T>(truncate));
			}
			return max;
		}

	public:
		template<class T>
		T GetData(bool truncate = false) { return GetLargestBinding<T>(truncate); }

	};

	extern std::vector<MasterBinding> bindings;
}