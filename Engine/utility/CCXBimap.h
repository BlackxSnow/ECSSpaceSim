#pragma once

#include <unordered_map>

namespace CCX
{
	// TODO Consider automatic pointer based optimisations for large types.
	// Make mutable?
	template<typename T1, typename T2>
	class UnorderedBimap
	{
	private:
		std::unordered_map<T1, T2> _LeftView;
		std::unordered_map<T2, T1> _RightView;

	public:
		T2 at(T1 key) const { return _LeftView.at(key); }
		T1 at(T2 key) const { return _RightView.at(key); }

		UnorderedBimap(std::initializer_list<std::pair<T1, T2>> list)
		{
			for (auto& pair : list)
			{
				_LeftView[pair.first] = pair.second;
				_RightView[pair.second] = pair.first;
			}
		}
	};
}