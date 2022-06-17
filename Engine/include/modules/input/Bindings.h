#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iterator>
#include <algorithm>

#include "InputDefs.h"
#include "../../utility/ConsoleLogging.h"
#include "../../utility/CCXEvent.h"
#include "../../utility/CCXMath.h"
#include "../../utility/CCXType.h"

namespace Thera::Input
{
	class Action;
	class BindingInstance;

	/// <summary>
	/// Base class for input classes that manage their own data internally.
	/// </summary>
	class DataHandler
	{
	protected:
		void* data;
		size_t lastUpdate = 0;
		
	public:
		const Output dataType;
		const Precision dataPrecision;

		virtual void TryPoll() = 0;
		virtual void ForcePoll() = 0;

		template<class T>
		T GetData(bool truncate = false) 
		{ 
			static_assert(std::is_same<T, void*>::value, "Direct data access is only supported by 'const void*' type.");
			static_assert(CCX::false_type<T>(), "Unhandled output type");
		}
		template<>
		const void* GetData<const void*>(bool truncate) { return data; }
		template<>
		float GetData<float>(bool truncate)
		{
			CCXAssert(dataType == Output::Scalar, "float output only supported by scalar bindings.");
			if (!truncate && dataPrecision == Precision::Double) { LogWarning("Undeclared truncation of double to float."); }
			TryPoll();
			return *static_cast<float*>(data);
		}
		template<>
		double GetData<double>(bool truncate)
		{
			CCXAssert(dataType == Output::Scalar, "double output only supported by scalar bindings.");
			CCXAssert(dataPrecision == Precision::Double, "Cannot convert float to double");
			TryPoll();
			return *static_cast<double*>(data);
		}
		template<>
		glm::vec2 GetData<glm::vec2>(bool truncate)
		{
			bool isTruncating = dataPrecision == Precision::Double;
			CCXAssert(dataType == Output::Vector2, "vec2 output only supported by Vector2 bindings.");
			if (!truncate && isTruncating) { LogWarning("Undeclared truncation of double to float."); }
			TryPoll();
			float* _data = static_cast<float*>(data);
			int offsetMultiplier = isTruncating ? 2 : 1;
			return glm::vec2(_data[0], _data[1 * offsetMultiplier]);
		}
		template<>
		glm::dvec2 GetData<glm::dvec2>(bool truncate)
		{
			CCXAssert(dataType == Output::Vector2, "dvec2 output only supported by Vector2 bindings.");
			CCXAssert(dataPrecision == Precision::Double, "Cannot convert float to double");
			TryPoll();
			double* _data = static_cast<double*>(data);
			return glm::dvec2(_data[0], _data[1]);
		}
		template<>
		glm::vec3 GetData<glm::vec3>(bool truncate)
		{
			bool isTruncating = dataPrecision == Precision::Double;
			CCXAssert(dataType == Output::Vector3, "vec3 output only supported by Vector3 bindings.");
			if (!truncate && isTruncating) { LogWarning("Undeclared truncation of double to float."); }
			TryPoll();
			float* _data = static_cast<float*>(data);
			int offsetMultiplier = isTruncating ? 2 : 1;
			return glm::vec3(_data[0], _data[1 * offsetMultiplier], _data[2 * offsetMultiplier]);
		}
		template<>
		glm::dvec3 GetData<glm::dvec3>(bool truncate)
		{
			CCXAssert(dataType == Output::Vector2, "dvec3 output only supported by Vector3 bindings.");
			CCXAssert(dataPrecision == Precision::Double, "Cannot convert float to double");
			TryPoll();
			double* _data = static_cast<double*>(data);
			return glm::dvec3(_data[0], _data[1], _data[2]);
		}

		DataHandler(Output _output, Precision _precision) : dataType(_output), dataPrecision(_precision)
		{
			data = new byte[(int)dataType * (int)dataPrecision];
			memset(data, 0, (int)dataType * (int)dataPrecision);
			lastUpdate = 0;
		}
	};

	/// <summary>
	/// Primary handler for in-built input types such as keys and buttons.
	/// </summary>
	class MasterBinding : public DataHandler
	{
	public:
		void HandleEvent(GLFWwindow* window, int action, int mods);
		void RemoveInstance(BindingInstance* remove);
		void ClearInstances();

		void TryPoll() override;
		void ForcePoll() override;

		BindingInstance* CreateInstance(Action* bindTo);
		BindingInstance* CreateUnboundInstance();

	private:
		std::vector<std::shared_ptr<BindingInstance>> instances;
		std::function<void(void*, Output, Precision)> poller;
		
	public:
		MasterBinding(Output _output, Precision _precision, std::function<void(void*, Output, Precision)> _poller)
			: DataHandler(_output, _precision), poller(_poller) {}
	};

	/// <summary>
	/// Individual instance derived from a specified MasterBinding through CreateBinding().
	/// </summary>
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

		BindingInstance(MasterBinding& _master, Action* boundAction) : master(_master), boundAction(boundAction) {}
		BindingInstance(MasterBinding& _master) : master(_master) { boundAction = nullptr; }
	};

	/// <summary>
	/// Single constituent binding wrapper for CompositeBindings.
	/// </summary>
	struct Constituent
	{
		BindingInstance* const binding;
		std::vector<int> indices;

		Constituent(BindingInstance* binding, std::initializer_list<Component> _indices) : binding(binding) 
		{
			CCXAssert(_indices.size() <= (int)binding->master.dataType, "Too many indices for binding output type.");
			std::transform(_indices.begin(), _indices.end(), std::back_inserter(indices), [](Component c) { return (int)c; });
		}
		Constituent(BindingInstance* binding, std::vector<int> indices) : binding(binding), indices(indices)
		{
			CCXAssert(indices.size() <= (int)binding->master.dataType, "Too many indices for binding output type.");
		}
	};

	/// <summary>
	/// A composition of one or more bindings to form a larger data type with an API identical to a single binding.
	/// </summary>
	class CompositeBinding : public DataHandler
	{
	private:
		std::vector<Constituent> constituents;

		void PollConstituents();

		void ValidateConstituents();

	public:
		Action* boundAction;

		void TryPoll() override;
		void ForcePoll() override;

		CompositeBinding(Action* bindTo, Output _dataType, Precision _dataPrecision, std::vector<Constituent>&& _constituents)
			: DataHandler(_dataType, _dataPrecision), boundAction(bindTo), constituents(_constituents) 
		{
			CCXAssert(bindTo != nullptr, "bindTo cannot be nullptr.");
			ValidateConstituents();
		}
	};
	
	/// <summary>
	/// Data describing a single input event.
	/// </summary>
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
			return source.master.GetData<T>();
		}

	public:
		InputEventData(BindingInstance& source, int action, int mods) : source(source), action(action), mods(mods) {}
	};
	

	/// <summary>
	/// High level binding aggregate class. Allows easy polling and event registration for an arbitrary number of BindingInstances and/or CompositeBindings.
	/// </summary>
	class Action
	{
	private:
		std::vector<BindingInstance*> bindings;
		std::vector<CompositeBinding*> composites;
	public:
		const Output dataType;
		const Precision dataPrecision;

		CCX::Event<InputEventData&> OnStart;
		CCX::Event<InputEventData&> OnRepeat;
		CCX::Event<InputEventData&> OnEnd;

		void HandleEvent(InputEventData& eventData);

		Action(Output _dataType, Precision _dataPrecision) : dataType(_dataType), dataPrecision(_dataPrecision) {}

		void AddBinding(BindingInstance* binding);
		void AddBinding(CompositeBinding* composite);

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
			bool isFirst = true;
			T max;
			for (auto& bind : bindings)
			{
				if (isFirst)
				{
					max = bind->GetData<T>(truncate);
					isFirst = false;
				}
				else
				{
					max = CCX::Max(max, bind->GetData<T>(truncate));
				}
			}
			for (auto& comp : composites)
			{
				if (isFirst)
				{
					max = comp->GetData<T>(truncate);
					isFirst = false;
				}
				else
				{
					max = CCX::Max(max, comp->GetData<T>(truncate));
				}
			}
			return max;
		}

	public:
		template<class T>
		T GetData(bool truncate = false) { return GetLargestBinding<T>(truncate); }

	};

	extern std::vector<MasterBinding> bindings;
	inline std::vector<std::unique_ptr<CompositeBinding>> composites;
}