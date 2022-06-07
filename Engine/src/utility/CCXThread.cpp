#include <utility/CCXThread.h>

#include <thread>
#include <condition_variable>
#include <unordered_set>
#include <future>
#include <sstream>

#include <utility/ConcurrentQueue.h>
#include <utility/ConsoleLogging.h>


namespace CCX
{
	class Worker
	{
	public:

	private:
		std::mutex _Mutex;
		std::condition_variable _Condition;
		std::thread _Thread;
		std::function<void()> _Task;
		std::function<void()> _CancelTask;
		std::shared_ptr<void> _OwnedMemory;
		std::atomic<bool> _IsAlive;

		void Run();

	public:
		Worker() : _Task(nullptr), _IsAlive(true), _Thread(&Worker::Run, this) {}
		Worker(const std::function<void()> task, std::function<void()> cancelTask = nullptr) : _Task(task), _CancelTask(cancelTask), _IsAlive(true), _Thread(&Worker::Run, this) {}
		Worker(const std::function<void()> task, std::shared_ptr<void> ownedMemory, std::function<void()> cancelTask = nullptr) 
			: _Task(task), _OwnedMemory(ownedMemory), _CancelTask(cancelTask), _IsAlive(true), _Thread(&Worker::Run, this) {}

		void Assign(std::function<void()> task, std::shared_ptr<void> ownedMemory, std::function<void()> cancelTask = nullptr)
		{
			std::lock_guard lock(_Mutex);
			_Task = task;
			_CancelTask = cancelTask;
			_OwnedMemory = ownedMemory;
			_Condition.notify_all();
		}
		void Assign(std::function<void()> task, std::function<void()> cancelTask = nullptr)
		{
			Assign(task, nullptr, cancelTask);
		}
		void Kill()
		{
			_IsAlive = false;
			_Condition.notify_all();
			_Thread.join();
		}
		bool TryCancel()
		{
			if (_CancelTask)
			{
				_CancelTask();
				return true;
			}
			return false;
		}
		std::thread::id ThreadID()
		{
			return _Thread.get_id();
		}
	};

	class WorkerPool
	{
	public:
		inline static CCX::ConcurrentQueue<Worker*> AvailableWorkers;
		inline static std::unordered_set<Worker*> Pool;

	public:
		~WorkerPool()
		{
			for (auto& worker : Pool)
			{
				auto killTask = std::async(std::launch::async, &Worker::Kill, worker);
				auto status = killTask.wait_for(std::chrono::seconds(2));
				if (status != std::future_status::ready)
				{
					std::ostringstream killLog;
					killLog << "Thread id " << worker->ThreadID() << " has taken longer than 2 seconds to join.";
					if (worker->TryCancel())
					{
						killLog << " Thread's cancel function was available and called. It is recommended to handle this within the application.";
					}
					else
					{
						killLog << " No cancel availabe. Program may hang indefinitely.";
					}
					LogWarning(killLog.str());
					killTask.wait();
				}
				delete worker;
			}
		}
	};

	void Worker::Run()
	{
		std::unique_lock lock(_Mutex);
		while (_IsAlive)
		{
			if (_Task) _Task();
			_Task = nullptr;
			_CancelTask = nullptr;
			if (!_IsAlive) break;
			WorkerPool::AvailableWorkers.Push(this);
			_Condition.wait(lock);
		}
	}

	std::shared_ptr<WorkerPool> _WorkerPool;

	void RunTask(std::function<void()> task, std::shared_ptr<void> taskMemory, std::function<void()> cancelTask)
	{
		if (!_WorkerPool)
		{
			_WorkerPool = std::make_shared<WorkerPool>();
		}

		Worker* worker;
		if (WorkerPool::AvailableWorkers.TryPop(worker))
		{
			worker->Assign(task, taskMemory, cancelTask);
		}
		else
		{
			worker = new Worker(task, taskMemory, cancelTask);
			WorkerPool::Pool.insert(worker);
		}
	}
	void RunTask(std::function<void()> task, std::function<void()> cancelTask)
	{
		RunTask(task, nullptr, cancelTask);
	}
}