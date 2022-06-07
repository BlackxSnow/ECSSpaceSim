#pragma once

#include <queue>
#include <mutex>

namespace CCX
{
	template<typename T>
	class ConcurrentQueue
	{
	private:
		std::queue<T> _Queue;
		std::mutex _Mutex;
		
	public:
		bool Empty() const
		{
			std::lock_guard lock(_Mutex);
			return _Queue.empty();
		}
		size_t Size() const
		{
			std::lock_guard lock(_Mutex);
			return _Queue.size();
		}
		void Push(T value)
		{
			std::lock_guard lock(_Mutex);
			_Queue.push(value);
		}
		bool TryPop(T& value)
		{
			std::lock_guard lock(_Mutex);
			if (_Queue.empty()) return false;

			value = _Queue.front();
			_Queue.pop();
			return true;
		}
	};
}