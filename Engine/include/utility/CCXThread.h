#pragma once

#include <functional>

namespace CCX
{
	void RunTask(std::function<void()> task, std::shared_ptr<void> taskMemory, std::function<void()> cancelTask = nullptr);
	void RunTask(std::function<void()> task, std::function<void()> cancelTask = nullptr);
}