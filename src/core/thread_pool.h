//
// Created by Lukáš Blažek on 14.09.2025.
//

#pragma once
#include <condition_variable>
#include <future>
#include <vector>
#include <thread>


class ThreadPool
{
public:
	explicit ThreadPool(size_t thread_count);
	~ThreadPool();

	// no copy
	ThreadPool(const ThreadPool&)            = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	void request_stop();

	template <class F, class... Args>
	auto enqueue(F&& f, Args&&... args)
		-> std::future<std::invoke_result_t<F, Args...>>
	{
		using R = std::invoke_result_t<F, Args...>;

		// wrap callable into a packaged task
			auto task = std::make_shared<std::packaged_task<R()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<R> fut = task->get_future();

		{
			std::scoped_lock lock(mutex_);
			tasks_.emplace([task]() { (*task)(); });
		}

		cv_.notify_one();
		return fut;
	}

private:
	std::vector<std::jthread>         workers_;
	std::queue<std::function<void()>> tasks_;

	std::mutex                  mutex_;
	std::condition_variable_any cv_;

	bool stopping_ = false;

	void worker_loop(std::stop_token st);
};
