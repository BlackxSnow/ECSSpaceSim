#pragma once

#include <functional>
#include <forward_list>
#include <unordered_map>

namespace CCX
{

	/// <summary>
	/// Invokable and registerable generic event class.
	/// </summary>
	/// <typeparam name="...Types"></typeparam>
	template<typename... Types>
	class Event
	{
	public:
		typedef std::function<void(Types...)> Registree;
		typedef Registree* Handle;
	private:
		std::vector<std::shared_ptr<Registree>> Registrees = std::vector<std::shared_ptr<Registree>>();

		inline Handle AddCallback(Registree& callback)
		{
			auto registree = std::make_shared<Registree>(callback);
			Registrees.push_back(registree);
			return registree.get();
		}

	public:
		size_t Count()
		{
			return Registrees.size();
		}
		/// <summary>
		/// Register a new callback and return a handle to it.
		/// </summary>
		/// <param name="callback"></param>
		/// <returns></returns>
		Handle Register(Registree callback)
		{
			return AddCallback(callback);
		}

		/// <summary>
		/// Remove an existing registree from the event.
		/// </summary>
		/// <param name="name"></param>
		bool Deregister(const Handle handle)
		{
			auto pos = std::find_if(Registrees.begin(), Registrees.end(), [handle](std::shared_ptr<Registree> r) { return r.get() == handle; });
			if (pos != Registrees.end())
			{
				Registrees.erase(pos);
				return true;
			}

			return false;
		}
		
		/// <summary>
		/// Call all registrees of this event with the provided parameters.
		/// </summary>
		/// <param name="...types"></param>
		void Invoke(Types... types)
		{
			for (std::shared_ptr<Registree> registree : Registrees)
			{
				(*registree)(types...);
			}
		}
	};
}