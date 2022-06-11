#pragma once

#include <functional>

namespace CCX
{
	/// <summary>
	/// Run a task using a threadpool, ensuring the task's lifespan through the provided smart pointer.
	/// </summary>
	/// <param name="task"></param>
	/// <param name="taskMemory">Ownership of the data required to perform the task.</param>
	/// <param name="cancelTask">Function to call for forceable cancellation of the task.</param>
	void RunTask(std::function<void()> task, std::shared_ptr<void> taskMemory, std::function<void()> cancelTask = nullptr);
	void RunTask(std::function<void()> task, std::function<void()> cancelTask = nullptr);
}