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
		static int a;
		typedef std::function<void(Types...)> Registree;
		typedef Registree* Handle;
	private:
		std::vector<std::shared_ptr<Registree>> Registrees = std::vector<std::shared_ptr<Registree>>();
		std::unordered_map<std::string, Handle> namedHandles = std::unordered_map<std::string, Handle>();

		inline Handle AddCallback(Registree& callback)
		{
			auto registree = std::shared_ptr<Registree>(new Registree(callback));
			Registrees.push_back(registree);
			return registree.get();
		}

	public:
		/// <summary>
		/// Register a new callback to the event under 'name'.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="registree"></param>
		void Register(std::string&& name, Registree callback)
		{
			namedHandles[name] = AddCallback(callback);
		}
		Handle Register(Registree callback)
		{
			return AddCallback(callback);
		}

		/// <summary>
		/// Remove an existing registree from the event.
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		bool Deregister(const std::string& name)
		{
			auto it = namedHandles.find(name);
			if (it == namedHandles.end())
			{
				return false;
			}
			Handle handle = it->second;
			auto pos = std::find_if(Registrees.begin(), Registrees.end(), [handle](std::shared_ptr<Registree> r) { return r.get() == handle; });
			if (pos != Registrees.end())
			{
				Registrees.erase(pos);
				namedHandles.erase(it);
				return true;
			}

			return false;
		}
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